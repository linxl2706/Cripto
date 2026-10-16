#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#include <ctype.h>

#include <gmp.h>

#include "utils.h"

int vigenere(int val, int m, int k);

int main(int argc, char *argv[])
{
    FILE *entrada = stdin, *salida = stdout;

    int encode = 0, ret = 0, param = 0;

    char code[TAM], k[TAM], *let = NULL;

    int m, val, k_val, res, k_num[TAM], idx;

    int i, j, len_k, len_w;

    for (i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-C") == 0)
        {
            encode = 0;
            if (param >= 100)
            {
                fprintf(stderr, "Error: no se puede codificar y decodificar al mismo tiempo\n");
                return -1;
            }
            param += 100;
        }
        else if (strcmp(argv[i], "-D") == 0)
        {
            encode = 1;
            if (param >= 100)
            {
                fprintf(stderr, "Error: no se puede codificar y decodificar al mismo tiempo\n");
                return -1;
            }
            param += 100;
        }
        else if (strcmp(argv[i], "-m") == 0 && i + 1 < argc)
        {
            m = atoi(argv[i + 1]);
            i++;
            param++;
        }
        else if (strcmp(argv[i], "-k") == 0 && i + 1 < argc)
        {
            strcpy(k, argv[i + 1]);
            i++;
            param += 10;
        }
        else if (strcmp(argv[i], "-i") == 0 && i + 1 < argc)
        {
            entrada = fopen(argv[i + 1], "r");
            if (!entrada)
            {
                fprintf(stderr, "Error al abrir el fichero de entrada\n");
                return -1;
            }
            i++;
        }
        else if (strcmp(argv[i], "-o") == 0 && i + 1 < argc)
        {
            salida = fopen(argv[i + 1], "w");
            if (!salida)
            {
                fprintf(stderr, "Error al abrir el fichero de salida\n");
                return -1;
            }
            i++;
        }
    }
    if (param != 111)
    {
        fprintf(stderr, "Uso -C/-D -m [tamano del espacio de texto cifrado] -k [cadena de caracteres usada como clave] -i [fichero de entrada] -o [fichero de salida] -p [(opcional) numero a permutar]\n");
        return -1;
    }
    len_k = strlen(k) - 1;
    for (i = 0; i < len_k; i++)
    {
        k_num[i] = char_to_number(k[i]);
        if (k_num[i] < 0)
        {
            fprintf(stderr, "La clave contiene caracteres no válidos\n");
            ret = -1;
            break;
        }
    }

    let = (char *)malloc(TAM * sizeof(char));
    fgets(let, TAM, entrada);
    len_w = strlen(let);
    if (len_w > 0 && let[len_w - 1] == '\n')
    {
        let[len_w - 1] = '\0';
        len_w--;
    }
    idx = 0;
    for (i = 0, j = 0; i < len_w; i++)
    {
        val = char_to_number(let[i]);
        j = j % len_k;
        if (val >= 0)
        {
            if (!encode)
                res = vigenere(val, m, k_num[j]);
            else
                res = vigenere(val, m, -k_num[j]);
            code[idx] = number_to_char(res);
            j++;
            idx++;
        }
    }

    code[idx] = '\0';

    fprintf(salida, "%s\n", code);

    if (entrada != stdin)
        fclose(entrada);

    if (salida != stdout)
        fclose(salida);

    if (let != NULL)
        free(let);

    return ret;
}

int vigenere(int val, int m, int k)
{
    int ret;
    ret = (val + k) % m;
    if (ret < 0)
    {
        ret += m;
    }
    return ret;
}