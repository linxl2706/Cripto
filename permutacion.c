#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

// qui lettura del plaintext anche dopo la prima matrice (con 'int *rep')
void getMatrix (char mat[],int len_v1, int len_v2, char text[], bool *endTxt, int *rep){
    int numbMat = (*rep) * len_v1 * len_v2;   // permette di ricordare quante matrici abbiamo creato
    for (int row=0; row<len_v1; row++){
        for (int column=0; column<len_v2; column++){
            if (text[numbMat + row*len_v2 + column] != '\0')
                mat[row*len_v2 + column] = text[numbMat + row*len_v2 + column];
            else{                            // qui se avanzano delle celle della matrice da riempire (padding)
                (*rep)++;
                int index = row*len_v2 + column;
                *endTxt = true;
                while (index < (len_v1 * len_v2)){
                    mat[index++] = 'X';
                    // index++;
                }
                return;
            }
        }
    }
    // se il testo coincide con un numero di matrici finite
    if (text[++(*rep) * len_v1 * len_v2] == '\0')    // controllo che il prossimo elemento sia effettivamente '\0' pre incrementando rep
        *endTxt = true;
}

// verifica che il vettore sia una permutazione valida di {1, ..., len}
// ogni numero da 1 a len deve comparire esattamente una volta
bool isValidPermutation(int vect[], int len){
    int* count = calloc(len + 1, sizeof *count);  // calloc inizializza tutto a 0
    if (count == NULL) return false;

    for (int i = 0; i < len; i++){
        if (vect[i] < 1 || vect[i] > len){   // fuori range
            free(count);
            return false;
        }
        count[vect[i]]++;
    }

    for (int i = 1; i <= len; i++){
        if (count[i] != 1){                  // numero mancante o duplicato
            free(count);
            return false;
        }
    }

    free(count);
    return true;
}

void permutationRow (int permutation_Vect[], char mat[], int len_v1, int len_v2){
    char temp_mat[len_v1][len_v2];

    // copio la riga corretta nella matrice di appoggio in posizione corretta
    for (int row=0; row<len_v1; row++){
        for (int column=0; column<len_v2; column++){
                temp_mat[row][column] = mat[(permutation_Vect[row]-1)*len_v2 + column]; 
            }
        }

    // modifico la matrice copiando quella di appoggio
     for (int row=0; row<len_v1; row++){
        for (int column=0; column<len_v2; column++){
                mat[row*len_v2 + column] = temp_mat[row][column];
            }
        }
}

void permutationColumn (int permutation_Vect[], char mat[], int len_v1, int len_v2){
    char temp_mat[len_v1][len_v2];

    // copio la colonna corretta nella matrice di appoggio in posizione corretta
    for (int column=0; column<len_v2; column++){
        for (int row=0; row<len_v1; row++){
                temp_mat[row][column] = mat[row*len_v2 + (permutation_Vect[column]-1)]; 
            }
        }

    // modifico la matrice copiando quella di appoggio
    for (int column=0; column<len_v2; column++){
        for (int row=0; row<len_v1; row++){
                mat[row*len_v2 + column] = temp_mat[row][column];
            }
        }
}

int* inv_vect (int vect[], int len_vect){
    int* inv = malloc(sizeof *inv * len_vect);
    if (inv == NULL) return NULL;
    for (int i=0; i<len_vect; i++){
        inv[vect[i]-1] = i+1;
    }
    return inv;
}

// legge tutto il contenuto di un FILE* in un buffer dinamico, restituisce la stringa
char* readText(FILE *fin){
    int capacity = 256;
    int length = 0;
    char* buffer = malloc(sizeof *buffer * capacity);

    if (buffer == NULL) return NULL;

    int read_char;  // int perché fgetc restituisce int (per distinguere EOF da un carattere valido)
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

// scrive la matrice (riga per riga) sul file di output
void writeMatrix(FILE *fout, char mat[], int len_v1, int len_v2){
    for (int k = 0; k < len_v1 * len_v2; k++){
        fputc(mat[k], fout);
    }
}


int main(int argc, char *argv[]){
    // ./permutacion {-C | -D} {-k1 K1 -k2 K2} [-i file_in] [-o file_out]

    int len_K1 = 0;
    int len_K2 = 0;
    int cap_K1 = 2;             // capacità attuale dell'array K1 (per realloc)
    int cap_K2 = 2;             // capacità attuale dell'array K2 (per realloc)
    bool end_text = false;
    int matrix_count = 0;
    char mode = 0;              // 'C' per cifratura, 'D' per decifratura
    char* input_file = NULL;    // nome del file di input (NULL = stdin)
    char* output_file = NULL;   // nome del file di output (NULL = stdout)
    int* K1 = malloc(sizeof *K1 * cap_K1);
    int* K2 = malloc(sizeof *K2 * cap_K2);
    

    // acquisizione dei parametri (esempio: -C -k1 3 1 2 -k2 2 1 3 -i input.txt -o output.txt)
    for (int i = 1; i < argc; i++){
        if (strcmp(argv[i], "-C") == 0){
            mode = 'C';
        }
        else if (strcmp(argv[i], "-D") == 0){
            mode = 'D';
        }
        else if (strcmp(argv[i], "-k1") == 0 && i + 1 < argc){

            // leggo tutti i numeri che seguono -k1 fino al prossimo flag (che inizia con '-')
            int j = i + 1;
            while (j < argc && argv[j][0] != '-'){
                if (len_K1 >= cap_K1){
                    cap_K1 *= 2;
                    K1 = realloc(K1, sizeof *K1 * cap_K1);
                }
                K1[len_K1++] = atoi(argv[j]);
                j++;
            }
            i = j - 1;  // sposto i all'ultimo elemento letto, poi il for farà i++
            if (!isValidPermutation(K1, len_K1)){
                fprintf(stderr, "Errore: K1 non è una permutazione valida di {1, ..., %d}\n", len_K1);
                return 1;
            }
        }
        else if (strcmp(argv[i], "-k2") == 0 && i + 1 < argc){

            // stessa logica di -k1
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
                fprintf(stderr, "Errore: K2 non è una permutazione valida di {1, ..., %d}\n", len_K2);
                return 1;
            }
        }
        else if (strcmp(argv[i], "-i") == 0 && i + 1 < argc){
            input_file = argv[++i];   // salvo il nome del file, ++i salta al prossimo argomento
        }
        else if (strcmp(argv[i], "-o") == 0 && i + 1 < argc){
            output_file = argv[++i];
        }
    }


    // validazione parametri
    if (mode == 0){
        fprintf(stderr, "Errore: specificare -C o -D\n");
        return 1;
    }
    if (len_K1 == 0 || len_K2 == 0){
        fprintf(stderr, "Errore: specificare -k1 e -k2\n");
        return 1;
    }

    // apertura file di input (stdin se non specificato)
    FILE *fin = stdin;
    if (input_file != NULL){
        fin = fopen(input_file, "r");   // fopen apre il file e restituisce un puntatore FILE*
        if (fin == NULL){
            fprintf(stderr,"Errore apertura file input\n");
            return 1;
        }
    }

    // apertura file di output (stdout se non specificato)
    FILE *fout = stdout;
    if (output_file != NULL){
        fout = fopen(output_file, "w");
        if (fout == NULL){
            fprintf(stderr,"Errore apertura file output\n");
            return 1;
        }
    }

    // lettura del testo completo in memoria
    char* text = readText(fin);
    if (text == NULL){
        fprintf(stderr, "Errore lettura testo\n");
        return 1;
    }
    if (fin != stdin) fclose(fin);

    // allocazione della matrice con la dimensione corretta (dopo aver letto K1 e K2)
    int matrix_size = len_K1 * len_K2;
    char* matrix = malloc(sizeof *matrix * matrix_size);
    if (mode == 'C'){

        // cifratura del plain text
        while (!end_text){
            getMatrix(matrix, len_K1, len_K2, text, &end_text, &matrix_count);
            permutationRow(K1, matrix, len_K1, len_K2);
            permutationColumn(K2, matrix, len_K1, len_K2);
            writeMatrix(fout, matrix, len_K1, len_K2);
        }
    }
    else if (mode == 'D'){

        // calcolo le permutazioni inverse per la decifratura
        int* K1_inv = inv_vect(K1, len_K1);
        int* K2_inv = inv_vect(K2, len_K2);
        if (K1_inv == NULL || K2_inv == NULL){
            fprintf(stderr, "Errore di allocazione\n");
            return 1;
        }

        // decifratura del cipher text (ordine inverso: prima colonne, poi righe)
        while (!end_text){
            getMatrix(matrix, len_K1, len_K2, text, &end_text, &matrix_count);            
            permutationColumn(K2_inv, matrix, len_K1, len_K2);
            permutationRow(K1_inv, matrix, len_K1, len_K2);
            writeMatrix(fout, matrix, len_K1, len_K2);
            }
        free(K1_inv);
        free(K2_inv);
    }

    // pulizia memoria
    if (fout != stdout) fclose(fout);
    free(text);
    free(matrix);
    free(K1);
    free(K2);
    return 0;
}