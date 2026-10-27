#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define L1 25
#define L2 27
#define L3 28
#define TOTAL_KEY (L1 + L2 + L3) // 80 bit

int reg1[L1];
int reg2[L2];
int reg3[L3];
int secret_key[TOTAL_KEY];

// Legge gli 80 bit dal file della chiave ignorando spazi e a capo (\n)
int loadKeyFromFile(const char* filename, int key_array[], int expected_bits){
    FILE *fkey = fopen(filename, "r");
    if (fkey == NULL){
        fprintf(stderr,"Errore apertura file chiave\n");
        return 0;
    }
    
    int count = 0;
    int ch;

    while ((ch = fgetc(fkey)) != EOF && count < expected_bits){
        if (ch == '0' || ch == '1'){
            key_array[count++] = ch - '0';
        }
        // ignora spazi, tabulazioni e caratteri di newline (\n, \r)
    }
    fclose(fkey);

    if (count != expected_bits){
        fprintf(stderr, "Errore: il file chiave contiene solo %d bit (richiesti %d)\n", count, expected_bits);
        return 0;
    }
    return 1; 
}

// Inizializza i 3 registri distribuendo gli 80 bit della chiave
void initialize_Reg(){
    int k = 0;
    for (int i = 0; i < L1; i++) reg1[i] = secret_key[k++];
    for (int i = 0; i < L2; i++) reg2[i] = secret_key[k++];
    for (int i = 0; i < L3; i++) reg3[i] = secret_key[k++];
}

// Genera il bit di keystream combinando i 3 registri con funzione non-lineare
unsigned int sequenceCiphering(){
    unsigned int z1 = reg1[L1 - 1]; // bit uscente R1
    unsigned int z2 = reg2[L2 - 1]; // bit uscente R2
    unsigned int z3 = reg3[L3 - 1]; // bit uscente R3

    // Funzione non lineare: distrugge la linearità di Berlekamp-Massey
    return (z1 | z2) ^ (z2 | z3) ^ (z1 | z3);
}

// Avanza lo stato dei 3 registri secondo i rispettivi polinomi primitivi
void newStateRegister(){

    // R1 (Grado 25: tap a 24 e 2)
    int bit_0_r1 = reg1[L1 - 1] ^ reg1[2];
    for (int i = L1 - 1; i >= 1; i--) reg1[i] = reg1[i - 1];
    reg1[0] = bit_0_r1;

    // R2 (Grado 27: tap a 26, 7, 6, 0)
    int bit_0_r2 = reg2[L2 - 1] ^ reg2[7] ^ reg2[6] ^ reg2[0];
    for (int i = L2 - 1; i >= 1; i--) reg2[i] = reg2[i - 1];
    reg2[0] = bit_0_r2;

    // R3 (Grado 28: tap a 27 e 2)
    int bit_0_r3 = reg3[L3 - 1] ^ reg3[2];
    for (int i = L3 - 1; i >= 1; i--) reg3[i] = reg3[i - 1];
    reg3[0] = bit_0_r3;
}

// Cifra/decifra il bit i-esimo del byte
char encryptionFunction(unsigned int z, unsigned char bit_pt, int i){
    return (z << i) ^ bit_pt;
}

// legge tutto il contenuto di un FILE* in un buffer dinamico, restituisce la stringa
char* readText(FILE fin, int length){
    int capacity = 256;
    (*length) = 0;
    char* buffer = malloc(sizeof *buffer * capacity);

    if (buffer == NULL) return NULL;

    int read_char;  // int perché fgetc restituisce int (per distinguere EOF da un carattere valido)
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

void writeText (FILE* fout, int length, unsigned char* txt){
    for (int i=0; i<length; i++){
        fputc(txt[i], fout);
    }
}

int main(int argc, char* argv[]){
    // ./flujo.c {-C | -D} {-k K} [-i file_in] [-o file_out]

    int count;
    int index;
    unsigned int z_out;
    int len_text = 0;

    char mode = 0;      // 'C' per cifratura, 'D' per decifratura
    char* input_file = NULL;
    char* output_file = NULL;
    char* key_file = NULL;
    int sKey[80] = {0};  // inizializzo la chiave a 0 senza for

    for (int i=1; i<argc; i++){
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

    // validazione parametri
    if (mode == 0){
        fprintf(stderr, "Errore: specificare -C o -D\n");
        return 1;
    }

    if (key_file == NULL){
        fprintf(stderr, "Errore: specificare il file della chiave con -k\n");
        return 1;
    }

    // Caricamento chiave dal file
    if (!loadKeyFromFile(key_file, secret_key, TOTAL_KEY)){
        return 1;
    }

    // apertura file di input (stdin se non specificato)
    FILE *fin = stdin;
    if (input_file != NULL){
        fin = fopen(input_file, "rb");   // fopen apre il file e restituisce un puntatore FILE*
        if (fin == NULL){
            fprintf(stderr, "Errore apertura file input\n");
            return 1;
        }
    }

    // apertura file di output (stdout se non specificato)
    FILE *fout = stdout;
    if (output_file != NULL){
        fout = fopen(output_file, "wb");
        if (fout == NULL){
            fprintf(stderr, "Errore apertura file output\n");
            return 1;
        }
    }

    // lettura del testo completo in memoria
    char* text = readText(fin, &len_text);
    if (text == NULL){
        fprintf(stderr, "Errore lettura testo\n");
        return 1;
    }
    if (fin != stdin) fclose(fin);
    
    unsigned char plain_text[len_text]; 
    unsigned char cipher_text[len_text];

    if (mode == 'C'){

        // Encrypting 
        initialize_Reg();
        count = 8;
        index = 0;
        z_out = 0;

        while (index<len_text){
            cipher_text[index] = text[index];          // preservo intatto il plain_text
            while(count>=1){
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
        
        // Decrypting
        initialize_Reg();
        count = 8;
        index = 0;
        z_out = 0;

        while (index<len_text){
            plain_text[index] = text[index];          // preservo intatto il cipher_text (text[])
            while(count>=1){
                z_out = sequenceCiphering();
                plain_text[index] = encryptionFunction(z_out, plain_text[index], --count);
                newStateRegister();
            }
            index++;
            count = 8;
        }
        writeText(fout, len_text, plain_text);
    }

    // pulizia memoria
    if (fout != stdout) fclose(fout);
    free(text);
    return 0;
}