#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#include <ctype.h>

#include <gmp.h>

#include "utils.h"

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

int main(int argc, char *argv[])
{
    FILE *entrada = stdin, *salida = stdout;

    int encode = 0, ret = 0;

    char code[TAM];

    int m, val, k_val, res;

    char k[TAM];

    int k_num[TAM];

    int i, j, len;

    char ch;

    for (i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-C") == 0)
        {
            encode = 0;
        }
        else if (strcmp(argv[i], "-D") == 0)
        {
            encode = 1;
        }
        else if (strcmp(argv[i], "-m") == 0 && i + 1 < argc)
        {
            m = atoi(argv[i + 1]);
            i++;
        }
        else if (strcmp(argv[i], "-k") == 0 && i + 1 < argc)
        {
            strcpy(k, argv[i + 1]);
            i++;
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

    len = strlen(k) - 1;
    for (i = 0; i < len; i++)
    {
        k_num[i] = char_to_number(k[i]);
        if (k_num[i] < 0)
        {
            fprintf(stderr, "La clave contiene caracteres no válidos\n");
            ret = -1;
            break;
        }
    }

    for (i = 0, j = 0; i < TAM; i++, j++)
    {
        if ((ch = fgetc(entrada)) != EOF)
        {
            val = char_to_number(ch);
            j = j % len;
            if (val >= 0)
            {
                if (encode)
                    res = vigenere(val, m, k_num[j]);
                else
                    res = vigenere(val, m, -k_num[j]);
                code[i] = number_to_char(res);
            }
            else
            {
                i--;
                j--;
            }
        }
        else
        {
            code[i] = '\0';
            break;
        }
    }

    fprintf(salida, "%s\n", code);

    if (entrada != stdin)
        fclose(entrada);

    if (salida != stdout)
        fclose(salida);

    return ret;
}