#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

// Extracts an M x N matrix block from text with 'X' padding if necessary
void getMatrix(char mat[], int len_v1, int len_v2, char text[], bool *endTxt, int *rep){
    int numbMat = (*rep) * len_v1 * len_v2;   // Offset tracking how many matrix blocks have been processed
    for (int row = 0; row < len_v1; row++){
        for (int column = 0; column < len_v2; column++){
            if (text[numbMat + row * len_v2 + column] != '\0')
                mat[row * len_v2 + column] = text[numbMat + row * len_v2 + column];
            else{                            // Padding remaining cells in the matrix with 'X'
                (*rep)++;
                int index = row * len_v2 + column;
                *endTxt = true;
                while (index < (len_v1 * len_v2)){
                    mat[index++] = 'X';
                }
                return;
            }
        }
    }
    // Check if the text length matches an exact number of full matrices
    if (text[++(*rep) * len_v1 * len_v2] == '\0')    // Check if the next character is '\0' after pre-incrementing rep
        *endTxt = true;
}

// Validates that the vector is a valid permutation of {1, ..., len}
// Each number from 1 to len must appear exactly once
bool isValidPermutation(int vect[], int len){
    int* count = calloc(len + 1, sizeof *count);  // calloc initializes all elements to 0
    if (count == NULL) return false;

    for (int i = 0; i < len; i++){
        if (vect[i] < 1 || vect[i] > len){   // Out of range
            free(count);
            return false;
        }
        count[vect[i]]++;
    }

    for (int i = 1; i <= len; i++){
        if (count[i] != 1){                  // Missing or duplicate number
            free(count);
            return false;
        }
    }

    free(count);
    return true;
}

// Row permutation according to the permutation vector K1
void permutationRow(int permutation_Vect[], char mat[], int len_v1, int len_v2){
    char temp_mat[len_v1][len_v2];

    // Copy the selected row into the temporary matrix at the permuted index
    for (int row = 0; row < len_v1; row++){
        for (int column = 0; column < len_v2; column++){
            temp_mat[row][column] = mat[(permutation_Vect[row] - 1) * len_v2 + column]; 
        }
    }

    // Copy the temporary matrix back to the original matrix
    for (int row = 0; row < len_v1; row++){
        for (int column = 0; column < len_v2; column++){
            mat[row * len_v2 + column] = temp_mat[row][column];
        }
    }
}

// Column permutation according to the permutation vector K2
void permutationColumn(int permutation_Vect[], char mat[], int len_v1, int len_v2){
    char temp_mat[len_v1][len_v2];

    // Copy the selected column into the temporary matrix at the permuted index
    for (int column = 0; column < len_v2; column++){
        for (int row = 0; row < len_v1; row++){
            temp_mat[row][column] = mat[row * len_v2 + (permutation_Vect[column] - 1)]; 
        }
    }

    // Copy the temporary matrix back to the original matrix
    for (int column = 0; column < len_v2; column++){
        for (int row = 0; row < len_v1; row++){
            mat[row * len_v2 + column] = temp_mat[row][column];
        }
    }
}

// Computes the inverse permutation vector K_inv such that K_inv[K[i] - 1] = i + 1
int* inv_vect(int vect[], int len_vect){
    int* inv = malloc(sizeof *inv * len_vect);
    if (inv == NULL) return NULL;
    for (int i = 0; i < len_vect; i++){
        inv[vect[i] - 1] = i + 1;
    }
    return inv;
}

// Reads the entire content of a FILE* into a dynamic buffer, returns null-terminated string
char* readText(FILE *fin){
    int capacity = 256;
    int length = 0;
    char* buffer = malloc(sizeof *buffer * capacity);

    if (buffer == NULL) return NULL;

    int read_char;  // int because fgetc returns int (to distinguish EOF from valid byte)
    while ((read_char = fgetc(fin)) != EOF){
        if (length >= capacity - 1){
            capacity *= 2;
            char* new_buffer = realloc(buffer, sizeof *buffer * capacity);
            if (new_buffer == NULL){ free(buffer); return NULL; }
            buffer = new_buffer;
        }
        buffer[length++] = read_char;
    }

    buffer[length] = '\0';
    return buffer;
}

// Writes the matrix block (row by row) to the output file
void writeMatrix(FILE *fout, char mat[], int len_v1, int len_v2){
    for (int k = 0; k < len_v1 * len_v2; k++){
        fputc(mat[k], fout);
    }
}

int main(int argc, char *argv[]){
    // Usage: ./permutacion {-C | -D} -k1 K1 -k2 K2 [-i file_in] [-o file_out]

    int len_K1 = 0;
    int len_K2 = 0;
    int cap_K1 = 2;             // Current capacity of K1 array (for realloc)
    int cap_K2 = 2;             // Current capacity of K2 array (for realloc)
    bool end_text = false;
    int matrix_count = 0;
    char mode = 0;              // 'C' for encryption, 'D' for decryption
    char* input_file = NULL;    // Input file name (NULL = stdin)
    char* output_file = NULL;   // Output file name (NULL = stdout)
    int* K1 = malloc(sizeof *K1 * cap_K1);
    int* K2 = malloc(sizeof *K2 * cap_K2);

    // Parse command line arguments (e.g. -C -k1 3 1 2 -k2 2 1 3 -i input.txt -o output.txt)
    for (int i = 1; i < argc; i++){
        if (strcmp(argv[i], "-C") == 0){
            mode = 'C';
        }
        else if (strcmp(argv[i], "-D") == 0){
            mode = 'D';
        }
        else if (strcmp(argv[i], "-k1") == 0 && i + 1 < argc){
            // Read all numbers following -k1 until the next option flag (starting with '-')
            int j = i + 1;
            while (j < argc && argv[j][0] != '-'){
                if (len_K1 >= cap_K1){
                    cap_K1 *= 2;
                    K1 = realloc(K1, sizeof *K1 * cap_K1);
                }
                K1[len_K1++] = atoi(argv[j]);
                j++;
            }
            i = j - 1;  // Adjust i to the last read argument before for-loop increments it
            if (!isValidPermutation(K1, len_K1)){
                fprintf(stderr, "Error: K1 is not a valid permutation of {1, ..., %d}\n", len_K1);
                return 1;
            }
        }
        else if (strcmp(argv[i], "-k2") == 0 && i + 1 < argc){
            // Same logic for -k2
            int j = i + 1;
            while (j < argc && argv[j][0] != '-'){
                if (len_K2 >= cap_K2){
                    cap_K2 *= 2;
                    K2 = realloc(K2, sizeof *K2 * cap_K2);
                }
                K2[len_K2++] = atoi(argv[j]);
                j++;
            }
            i = j - 1;
            if (!isValidPermutation(K2, len_K2)){
                fprintf(stderr, "Error: K2 is not a valid permutation of {1, ..., %d}\n", len_K2);
                return 1;
            }
        }
        else if (strcmp(argv[i], "-i") == 0 && i + 1 < argc){
            input_file = argv[++i];   // Save input file name, ++i skips to next argument
        }
        else if (strcmp(argv[i], "-o") == 0 && i + 1 < argc){
            output_file = argv[++i];
        }
    }

    // Parameter validation
    if (mode == 0){
        fprintf(stderr, "Error: specify -C or -D\n");
        return 1;
    }
    if (len_K1 == 0 || len_K2 == 0){
        fprintf(stderr, "Error: specify both -k1 and -k2\n");
        return 1;
    }

    // Open input file (stdin if not specified)
    FILE *fin = stdin;
    if (input_file != NULL){
        fin = fopen(input_file, "r");
        if (fin == NULL){
            fprintf(stderr, "Error opening input file\n");
            return 1;
        }
    }

    // Open output file (stdout if not specified)
    FILE *fout = stdout;
    if (output_file != NULL){
        fout = fopen(output_file, "w");
        if (fout == NULL){
            fprintf(stderr, "Error opening output file\n");
            return 1;
        }
    }

    // Read entire text into memory
    char* text = readText(fin);
    if (text == NULL){
        fprintf(stderr, "Error reading input text\n");
        return 1;
    }
    if (fin != stdin) fclose(fin);

    // Allocate matrix with correct dimensions (M * N)
    int matrix_size = len_K1 * len_K2;
    char* matrix = malloc(sizeof *matrix * matrix_size);

    if (mode == 'C'){
        // Plaintext encryption
        while (!end_text){
            getMatrix(matrix, len_K1, len_K2, text, &end_text, &matrix_count);
            permutationRow(K1, matrix, len_K1, len_K2);
            permutationColumn(K2, matrix, len_K1, len_K2);
            writeMatrix(fout, matrix, len_K1, len_K2);
        }
    }
    else if (mode == 'D'){
        // Compute inverse permutations for decryption
        int* K1_inv = inv_vect(K1, len_K1);
        int* K2_inv = inv_vect(K2, len_K2);
        if (K1_inv == NULL || K2_inv == NULL){
            fprintf(stderr, "Memory allocation error\n");
            return 1;
        }

        // Decryption of ciphertext (reverse order: columns first, then rows)
        while (!end_text){
            getMatrix(matrix, len_K1, len_K2, text, &end_text, &matrix_count);            
            permutationColumn(K2_inv, matrix, len_K1, len_K2);
            permutationRow(K1_inv, matrix, len_K1, len_K2);
            writeMatrix(fout, matrix, len_K1, len_K2);
        }
        free(K1_inv);
        free(K2_inv);
    }

    // Memory cleanup
    if (fout != stdout) fclose(fout);
    free(text);
    free(matrix);
    free(K1);
    free(K2);
    return 0;
}