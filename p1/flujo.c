#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define L1 25
#define L2 27
#define L3 28
#define TOTAL_KEY (L1 + L2 + L3) // 80 bits

int reg1[L1];
int reg2[L2];
int reg3[L3];
int secret_key[TOTAL_KEY];

// Reads the 80 bits from the key file, ignoring spaces, tabs, and newlines (\n, \r)
int loadKeyFromFile(const char* filename, int key_array[], int expected_bits){
    FILE *fkey = fopen(filename, "r");
    if (fkey == NULL){
        fprintf(stderr, "Error: cannot open key file\n");
        return 0;
    }
    
    int count = 0;
    int ch;

    // Ignore whitespace, tabs, and newline characters
    while ((ch = fgetc(fkey)) != EOF && count < expected_bits){
        if (ch == '0' || ch == '1'){
            key_array[count++] = ch - '0';
        }
    }
    fclose(fkey);

    if (count != expected_bits){
        fprintf(stderr, "Error: key file contains only %d bits (%d required)\n", count, expected_bits);
        return 0;
    }
    return 1; 
}

// Initializes the 3 registers by distributing the 80 bits of the secret key
// Returns 1 on success, 0 if any register segment is all zeros (degenerate forbidden state)
int initialize_Reg(){
    int k = 0;
    int sum1 = 0, sum2 = 0, sum3 = 0;

    for (int i = 0; i < L1; i++){
        reg1[i] = secret_key[k++];
        sum1 += reg1[i];
    }
    for (int i = 0; i < L2; i++){
        reg2[i] = secret_key[k++];
        sum2 += reg2[i];
    }
    for (int i = 0; i < L3; i++){
        reg3[i] = secret_key[k++];
        sum3 += reg3[i];
    }

    // Check for the degenerate all-zero state
    if (sum1 == 0 || sum2 == 0 || sum3 == 0){
        fprintf(stderr, "Error: invalid key! One of the register segments is all zeros.\n");
        return 0;
    }
    return 1;
}

// Generates the keystream bit by combining the 3 registers with a non-linear function
unsigned int sequenceCiphering(){
    unsigned int z1 = reg1[L1 - 1]; // output bit from R1
    unsigned int z2 = reg2[L2 - 1]; // output bit from R2
    unsigned int z3 = reg3[L3 - 1]; // output bit from R3

    // Non-linear function (Majority): breaks Berlekamp-Massey linearity
    return (z1 | z2) ^ (z2 | z3) ^ (z1 | z3);
}

// Advances the state of the 3 registers according to their respective primitive polynomials
void newStateRegister(){
    // R1 (Degree 25: taps at 24 and 2)
    int bit_0_r1 = reg1[L1 - 1] ^ reg1[2];
    for (int i = L1 - 1; i >= 1; i--) reg1[i] = reg1[i - 1];
    reg1[0] = bit_0_r1;

    // R2 (Degree 27: taps at 26, 7, 6, 0)
    int bit_0_r2 = reg2[L2 - 1] ^ reg2[7] ^ reg2[6] ^ reg2[0];
    for (int i = L2 - 1; i >= 1; i--) reg2[i] = reg2[i - 1];
    reg2[0] = bit_0_r2;

    // R3 (Degree 28: taps at 27 and 2)
    int bit_0_r3 = reg3[L3 - 1] ^ reg3[2];
    for (int i = L3 - 1; i >= 1; i--) reg3[i] = reg3[i - 1];
    reg3[0] = bit_0_r3;
}

// Encrypts/decrypts the i-th bit of the byte
char encryptionFunction(unsigned int z, unsigned char bit_pt, int i){
    return (z << i) ^ bit_pt;
}

// Reads the entire content of a FILE* into a dynamic buffer, returns the buffer pointer
char* readText(FILE *fin, int* length){
    int capacity = 256;
    (*length) = 0;
    char* buffer = malloc(sizeof *buffer * capacity);

    if (buffer == NULL) return NULL;

    int read_char;  // int because fgetc returns int (to distinguish EOF from valid byte)
    while ((read_char = fgetc(fin)) != EOF){
        if ((*length) >= capacity - 1){
            capacity *= 2;
            char* new_buffer = realloc(buffer, sizeof *buffer * capacity);
            if (new_buffer == NULL){ free(buffer); return NULL; }
            buffer = new_buffer;
        }
        buffer[(*length)++] = read_char;
    }

    buffer[(*length)] = '\0';
    return buffer;
}

// Writes the byte buffer to the output file
void writeText(FILE* fout, int length, unsigned char* txt){
    for (int i = 0; i < length; i++){
        fputc(txt[i], fout);
    }
}

int main(int argc, char* argv[]){
    // Usage: ./flujo {-C | -D} -k key_file [-i file_in] [-o file_out]

    int count;
    int index;
    unsigned int z_out;
    int len_text = 0;

    char mode = 0;              // 'C' for encryption, 'D' for decryption
    char* input_file = NULL;
    char* output_file = NULL;
    char* key_file = NULL;

    for (int i = 1; i < argc; i++){
        if (strcmp(argv[i], "-C") == 0)
            mode = 'C';
        else if (strcmp(argv[i], "-D") == 0)
            mode = 'D';
        else if (strcmp(argv[i], "-k") == 0 && i + 1 < argc)
            key_file = argv[++i];
        else if (strcmp(argv[i], "-i") == 0 && i + 1 < argc)
            input_file = argv[++i];
        else if (strcmp(argv[i], "-o") == 0 && i + 1 < argc)
            output_file = argv[++i];
    }

    // Parameter validation
    if (mode == 0){
        fprintf(stderr, "Error: specify -C or -D\n");
        return 1;
    }

    if (key_file == NULL){
        fprintf(stderr, "Error: specify the key file with -k\n");
        return 1;
    }

    // Load key from file
    if (!loadKeyFromFile(key_file, secret_key, TOTAL_KEY)){
        return 1;
    }

    // Check for the degenerate all-zero state
    if (!initialize_Reg()){
        return 1;
    }

    // Open input file (stdin if not specified)
    FILE *fin = stdin;
    if (input_file != NULL){
        fin = fopen(input_file, "rb");   // open in binary read mode
        if (fin == NULL){
            fprintf(stderr, "Error opening input file\n");
            return 1;
        }
    }

    // Open output file (stdout if not specified)
    FILE *fout = stdout;
    if (output_file != NULL){
        fout = fopen(output_file, "wb");  // open in binary write mode
        if (fout == NULL){
            fprintf(stderr, "Error opening output file\n");
            return 1;
        }
    }

    // Read entire text into memory
    char* text = readText(fin, &len_text);
    if (text == NULL){
        fprintf(stderr, "Error reading input text\n");
        return 1;
    }
    if (fin != stdin) fclose(fin);
    
    unsigned char plain_text[len_text]; 
    unsigned char cipher_text[len_text];

    if (mode == 'C'){
        // Encryption
        initialize_Reg();
        count = 8;
        index = 0;
        z_out = 0;

        while (index < len_text){
            cipher_text[index] = text[index];          // preserve original plain_text
            while (count >= 1){
                z_out = sequenceCiphering();
                cipher_text[index] = encryptionFunction(z_out, cipher_text[index], --count);
                newStateRegister();
            }
            index++;
            count = 8;
        }
        writeText(fout, len_text, cipher_text);
    } 
    else if (mode == 'D'){
        // Decryption
        initialize_Reg();
        count = 8;
        index = 0;
        z_out = 0;

        while (index < len_text){
            plain_text[index] = text[index];          // preserve original cipher_text (text[])
            while (count >= 1){
                z_out = sequenceCiphering();
                plain_text[index] = encryptionFunction(z_out, plain_text[index], --count);
                newStateRegister();
            }
            index++;
            count = 8;
        }
        writeText(fout, len_text, plain_text);
    }

    // Memory cleanup
    if (fout != stdout) fclose(fout);
    free(text);
    return 0;
}
