#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#include <ctype.h>

#include <stdint.h>

#include <math.h>

#define BYTES_PER_WORD 4
#define ROWS_PER_SBOX 16
#define COLUMNS_PER_SBOX 16
#define HEX_STRING_SIZE 3
#define POLINOMIO_AES 0x11B

static const char DIRECT_SBOX[ROWS_PER_SBOX][COLUMNS_PER_SBOX][HEX_STRING_SIZE] = {
	{ "63", "7c", "77", "7b", "f2", "6b", "6f", "c5", "30", "01", "67", "2b", "fe", "d7", "ab", "76" },
	{ "ca", "82", "c9", "7d", "fa", "59", "47", "f0", "ad", "d4", "a2", "af", "9c", "a4", "72", "c0" },
	{ "b7", "fd", "93", "26", "36", "3f", "f7", "cc", "34", "a5", "e5", "f1", "71", "d8", "31", "15" },
	{ "04", "c7", "23", "c3", "18", "96", "05", "9a", "07", "12", "80", "e2", "eb", "27", "b2", "75" },
	{ "09", "83", "2c", "1a", "1b", "6e", "5a", "a0", "52", "3b", "d6", "b3", "29", "e3", "2f", "84" },
	{ "53", "d1", "00", "ed", "20", "fc", "b1", "5b", "6a", "cb", "be", "39", "4a", "4c", "58", "cf" },
	{ "d0", "ef", "aa", "fb", "43", "4d", "33", "85", "45", "f9", "02", "7f", "50", "3c", "9f", "a8" },
	{ "51", "a3", "40", "8f", "92", "9d", "38", "f5", "bc", "b6", "da", "21", "10", "ff", "f3", "d2" },
	{ "cd", "0c", "13", "ec", "5f", "97", "44", "17", "c4", "a7", "7e", "3d", "64", "5d", "19", "73" },
	{ "60", "81", "4f", "dc", "22", "2a", "90", "88", "46", "ee", "b8", "14", "de", "5e", "0b", "db" },
	{ "e0", "32", "3a", "0a", "49", "06", "24", "5c", "c2", "d3", "ac", "62", "91", "95", "e4", "79" },
	{ "e7", "c8", "37", "6d", "8d", "d5", "4e", "a9", "6c", "56", "f4", "ea", "65", "7a", "ae", "08" },
	{ "ba", "78", "25", "2e", "1c", "a6", "b4", "c6", "e8", "dd", "74", "1f", "4b", "bd", "8b", "8a" },
	{ "70", "3e", "b5", "66", "48", "03", "f6", "0e", "61", "35", "57", "b9", "86", "c1", "1d", "9e" },
	{ "e1", "f8", "98", "11", "69", "d9", "8e", "94", "9b", "1e", "87", "e9", "ce", "55", "28", "df" },
	{ "8c", "a1", "89", "0d", "bf", "e6", "42", "68", "41", "99", "2d", "0f", "b0", "54", "bb", "16" }
};

static const char INVERSE_SBOX[ROWS_PER_SBOX][COLUMNS_PER_SBOX][HEX_STRING_SIZE] = {
	{ "52", "09", "6a", "d5", "30", "36", "a5", "38", "bf", "40", "a3", "9e", "81", "f3", "d7", "fb" },
	{ "7c", "e3", "39", "82", "9b", "2f", "ff", "87", "34", "8e", "43", "44", "c4", "de", "e9", "cb" },
	{ "54", "7b", "94", "32", "a6", "c2", "23", "3d", "ee", "4c", "95", "0b", "42", "fa", "c3", "4e" },
	{ "08", "2e", "a1", "66", "28", "d9", "24", "b2", "76", "5b", "a2", "49", "6d", "8b", "d1", "25" },
	{ "72", "f8", "f6", "64", "86", "68", "98", "16", "d4", "a4", "5c", "cc", "5d", "65", "b6", "92" },
	{ "6c", "70", "48", "50", "fd", "ed", "b9", "da", "5e", "15", "46", "57", "a7", "8d", "9d", "84" },
	{ "90", "d8", "ab", "00", "8c", "bc", "d3", "0a", "f7", "e4", "58", "05", "b8", "b3", "45", "06" },
	{ "d0", "2c", "1e", "8f", "ca", "3f", "0f", "02", "c1", "af", "bd", "03", "01", "13", "8a", "6b" },
	{ "3a", "91", "11", "41", "4f", "67", "dc", "ea", "97", "f2", "cf", "ce", "f0", "b4", "e6", "73" },
	{ "96", "ac", "74", "22", "e7", "ad", "35", "85", "e2", "f9", "37", "e8", "1c", "75", "df", "6e" },
	{ "47", "f1", "1a", "71", "1d", "29", "c5", "89", "6f", "b7", "62", "0e", "aa", "18", "be", "1b" },
	{ "fc", "56", "3e", "4b", "c6", "d2", "79", "20", "9a", "db", "c0", "fe", "78", "cd", "5a", "f4" },
	{ "1f", "dd", "a8", "33", "88", "07", "c7", "31", "b1", "12", "10", "59", "27", "80", "ec", "5f" },
	{ "60", "51", "7f", "a9", "19", "b5", "4a", "0d", "2d", "e5", "7a", "9f", "93", "c9", "9c", "ef" },
	{ "a0", "e0", "3b", "4d", "ae", "2a", "f5", "b0", "c8", "eb", "bb", "3c", "83", "53", "99", "61" },
	{ "17", "2b", "04", "7e", "ba", "77", "d6", "26", "e1", "69", "14", "63", "55", "21", "0c", "7d" }
};

static const char MIX_COLUMN_MATRIX[BYTES_PER_WORD][BYTES_PER_WORD][HEX_STRING_SIZE] = {
	{ "02", "03", "01", "01" },
	{ "01", "02", "03", "01" },
	{ "01", "01", "02", "03" },
	{ "03", "01", "01", "02" }
};

static const char INV_MIX_COLUMN_MATRIX[BYTES_PER_WORD][BYTES_PER_WORD][HEX_STRING_SIZE] = {
	{ "0E", "0B", "0D", "09" },
	{ "09", "0E", "0B", "0D" },
	{ "0D", "09", "0E", "0B" },
	{ "0B", "0D", "09", "0E" }
};

/* =========================================================
   APARTADO B: OPERACIONES CON POLINOMIOS
   ========================================================= */

/* Devuelve el grado del polinomio */
int grado_polinomio(uint16_t p)
{
    int grado = -1;

    while (p != 0) {
        p >>= 1;
        grado++;
    }

    return grado;
}

/* Division de polinomios en GF(2) */
uint16_t division_polinomios(uint16_t a, uint16_t b,
                             uint16_t *resto)
{
    uint16_t cociente = 0;
    int diferencia;

    if (b == 0) {
        *resto = a;
        return 0;
    }

    while (a != 0 &&
           grado_polinomio(a) >= grado_polinomio(b)) {

        diferencia = grado_polinomio(a) -
                     grado_polinomio(b);

        cociente ^= (uint16_t)(1U << diferencia);

        a ^= (uint16_t)(b << diferencia);
    }

    *resto = a;

    return cociente;
}

/* Multiplicacion de polinomios */
uint16_t multiplicar_polinomios(uint16_t a, uint16_t b)
{
    uint16_t resultado = 0;

    while (b != 0) {

        if (b & 1)
            resultado ^= a;

        a <<= 1;
        b >>= 1;
    }

    return resultado;
}

/* Modulo de dos polinomios */
uint16_t modulo_polinomio(uint16_t a, uint16_t m)
{
    uint16_t resto;

    division_polinomios(a, m, &resto);

    return resto;
}

/* =========================================================
   ALGORITMO DE EUCLIDES
   ========================================================= */

uint16_t euclides(uint16_t a, uint16_t b)
{
    uint16_t resto;

    while (b != 0) {

        division_polinomios(a, b, &resto);

        a = b;
        b = resto;
    }

    return a;
}

/* =========================================================
   ALGORITMO DE EUCLIDES EXTENDIDO
   ========================================================= */

int inverso_multiplicativo(uint8_t a, uint8_t *inverso)
{
    uint16_t r0 = POLINOMIO_AES;
    uint16_t r1 = a;

    uint16_t t0 = 0;
    uint16_t t1 = 1;

    uint16_t cociente;
    uint16_t resto;
    uint16_t t2;

    if (a == 0)
        return 0;

    if (euclides(a, POLINOMIO_AES) != 1)
        return 0;

    while (r1 != 0) {

        cociente = division_polinomios(r0, r1, &resto);

        r0 = r1;
        r1 = resto;

        t2 = t0 ^ multiplicar_polinomios(cociente, t1);

        t0 = t1;

        t1 = modulo_polinomio(t2, POLINOMIO_AES);
    }

    if (r0 != 1)
        return 0;

    *inverso = (uint8_t)modulo_polinomio(t0, POLINOMIO_AES);

    return 1;
}

/* =========================================================
   TRANSFORMACION AFIN
   ========================================================= */

uint8_t rotar_izquierda(uint8_t valor, int n)
{
    return (uint8_t)((valor << n) | (valor >> (8 - n)));
}

uint8_t transformacion_afin(uint8_t a)
{
    return a ^
           rotar_izquierda(a, 1) ^
           rotar_izquierda(a, 2) ^
           rotar_izquierda(a, 3) ^
           rotar_izquierda(a, 4) ^
           0x63;
}

/* =========================================================
   GENERACION DE LAS S-BOXES
   ========================================================= */

int generar_sbox(uint8_t sbox[256])
{
    int i;
    uint8_t inverso;

    for (i = 0; i < 256; i++) {

        if (i == 0) {
            inverso = 0;
        }
        else {
            if (!inverso_multiplicativo((uint8_t)i, &inverso))
                return 0;
        }

        sbox[i] = transformacion_afin(inverso);
    }

    return 1;
}

/* Generacion de la S-box inversa */
void generar_sbox_inversa(uint8_t sbox[256],
                          uint8_t inversa[256])
{
    int i;

    for (i = 0; i < 256; i++)
        inversa[sbox[i]] = (uint8_t)i;
}

/* =========================================================
   COMPROBACION CON LAS TABLAS OFICIALES
   ========================================================= */

void convertir_tabla(const char tabla[16][16][3],
                     uint8_t resultado[256])
{
    int fila, columna;

    for (fila = 0; fila < 16; fila++) {

        for (columna = 0; columna < 16; columna++) {

            resultado[fila * 16 + columna] =
                (uint8_t)strtoul(
                    tabla[fila][columna], NULL, 16
                );
        }
    }
}

int comprobar_sbox(uint8_t generada[256],
                   uint8_t oficial[256])
{
    int i;

    for (i = 0; i < 256; i++) {

        if (generada[i] != oficial[i]) {

            fprintf(stderr,
                    "Error posicion %02X: generada=%02X oficial=%02X\n",
                    i, generada[i], oficial[i]);

            return 0;
        }
    }

    return 1;
}

/* Imprimir S-box */
void imprimir_sbox(uint8_t sbox[256], FILE *salida)
{
    int i;

    for (i = 0; i < 256; i++) {

        fprintf(salida, "%02X ", sbox[i]);

        if ((i + 1) % 16 == 0)
            fprintf(salida, "\n");
    }
}

/* =========================================================
   APARTADO A: NO LINEALIDAD
   ========================================================= */

/* Calcula la paridad de un byte */
int paridad(uint8_t valor)
{
    int resultado = 0;

    while (valor != 0) {

        resultado ^= (valor & 1);

        valor >>= 1;
    }

    return resultado;
}

/* Calcula la no linealidad minima de las funciones
   booleanas componentes de la S-box */
int calcular_no_linealidad(uint8_t sbox[256], FILE *salida)
{
    int a, b, x;
    int walsh;
    int max_walsh;
    int no_linealidad;
    int minima = 128;

    for (b = 1; b < 256; b++) {

        max_walsh = 0;

        for (a = 0; a < 256; a++) {

            walsh = 0;

            for (x = 0; x < 256; x++) {

                int f = paridad((uint8_t)(b & sbox[x]));
                int g = paridad((uint8_t)(a & x));

                if (f == g)
                    walsh++;
                else
                    walsh--;
            }

            if (abs(walsh) > max_walsh)
                max_walsh = abs(walsh);
        }

        no_linealidad = 128 - max_walsh / 2;

        if (no_linealidad < minima)
            minima = no_linealidad;
    }

    fprintf(salida,
            "No linealidad minima de la S-box: %d\n",
            minima);

    return minima;
}

/* =========================================================
   CRITERIO SAC
   ========================================================= */

void comprobar_SAC(uint8_t sbox[256], FILE *salida)
{
    int entrada, bit_in, bit_out;
    int contador;

    uint8_t diferencia;

    fprintf(salida, "\nMatriz SAC (porcentajes):\n");

    for (bit_in = 0; bit_in < 8; bit_in++) {

        for (bit_out = 0; bit_out < 8; bit_out++) {

            contador = 0;

            for (entrada = 0; entrada < 256; entrada++) {

                diferencia =
                    sbox[entrada] ^
                    sbox[entrada ^ (1 << bit_in)];

                if (diferencia & (1 << bit_out))
                    contador++;
            }

            fprintf(salida, "%6.2f ",
                    contador * 100.0 / 256);
        }

        fprintf(salida, "\n");
    }
}

/* =========================================================
   MAIN
   ========================================================= */

int main(int argc, char *argv[])
{
    uint8_t sbox[256];
    uint8_t inversa[256];
    uint8_t oficial[256];

    FILE *salida = stdout;

    char *archivo = NULL;

    int modo = 0;
    int i;

    /* Procesamiento de argumentos */
    for (i = 1; i < argc; i++) {

        if (strcmp(argv[i], "-C") == 0) {

            if (modo != 0) {
                fprintf(stderr, "Error: modos incompatibles.\n");
                return 1;
            }

            modo = 1;
        }

        else if (strcmp(argv[i], "-D") == 0) {

            if (modo != 0) {
                fprintf(stderr, "Error: modos incompatibles.\n");
                return 1;
            }

            modo = 2;
        }

        else if (strcmp(argv[i], "-N") == 0) {

            if (modo != 0) {
                fprintf(stderr, "Error: modos incompatibles.\n");
                return 1;
            }

            modo = 3;
        }

        else if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) {

            if (archivo != NULL) {
                fprintf(stderr, "Error: -o repetido.\n");
                return 1;
            }

            archivo = argv[++i];
        }

        else {
            fprintf(stderr, "Argumento incorrecto: %s\n", argv[i]);
            return 1;
        }
    }

    if (modo == 0) {

        fprintf(stderr,
                "Uso:\n"
                "./SBOX_AES -C [-o archivo]\n"
                "./SBOX_AES -D [-o archivo]\n"
                "./SBOX_AES -N [-o archivo]\n");

        return 1;
    }

    /* Generar S-box directa */
    if (!generar_sbox(sbox)) {
        fprintf(stderr, "Error generando la S-box.\n");
        return 1;
    }

    /* Generar S-box inversa */
    generar_sbox_inversa(sbox, inversa);

    /* Abrir archivo de salida */
    if (archivo != NULL) {

        salida = fopen(archivo, "w");

        if (salida == NULL) {
            perror("Error abriendo archivo");
            return 1;
        }
    }

    /* Apartado B: S-box directa */
    if (modo == 1) {

        convertir_tabla(DIRECT_SBOX, oficial);

        if (!comprobar_sbox(sbox, oficial)) {

            fprintf(stderr, "S-box directa incorrecta.\n");

            if (archivo != NULL)
                fclose(salida);

            return 1;
        }

        fprintf(salida, "S-box directa correcta.\n\n");

        imprimir_sbox(sbox, salida);
    }

    /* Apartado B: S-box inversa */
    else if (modo == 2) {

        convertir_tabla(INVERSE_SBOX, oficial);

        if (!comprobar_sbox(inversa, oficial)) {

            fprintf(stderr, "S-box inversa incorrecta.\n");

            if (archivo != NULL)
                fclose(salida);

            return 1;
        }

        fprintf(salida, "S-box inversa correcta.\n\n");

        imprimir_sbox(inversa, salida);
    }

    /* Apartado A: no linealidad */
    else if (modo == 3) {

        calcular_no_linealidad(sbox, salida);

        comprobar_SAC(sbox, salida);
    }

    if (archivo != NULL)
        fclose(salida);

    return 0;
}
