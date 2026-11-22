#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#include <ctype.h>

#include <math.h>

#define ALPHABET 26
#define TAM 4096

int main(int argc, char *argv[])
{
    FILE *entrada = stdin, *salida = stdout;

    int met = -1, ret = 0, param = 0, err = 0;

    char *let = NULL;

    int i, j, len_w, n[ALPHABET];

    double prob_x[ALPHABET], prob_y[ALPHABET], prob_k[ALPHABET], prob_conj[ALPHABET][ALPHABET];

    for (i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-P") == 0)
        {
            met = 1;
            if (param >= 100)
            {
                fprintf(stderr, "Error: no se puede usar el método equiprobable y no equiprobable al mismo tiempo\n");
                return -1;
            }
            param += 100;
        }
        else if (strcmp(argv[i], "-I") == 0)
        {
            met = 0;
            if (param >= 100)
            {
                fprintf(stderr, "Error: no se puede usar el método equiprobable y no equiprobable al mismo tiempo\n");
                return -1;
            }
            param += 100;
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
    if (met < 0)
    {
        fprintf(stderr, "Uso -P(claves equiprobables)/-I(claves no equiprobables) -i [fichero de entrada] -o [fichero de salida]\n");
        return -1;
    }

    for (i = 0; i < ALPHABET; i++)
    {
        prob_k[i] = met ? 1.0 / ALPHABET : 0.5 / (ALPHABET - 1);
        n[i] = 0;
    }

    if (!met)
        prob_k[0] += 0.5;

    let = (char *)malloc(TAM * sizeof(char));

    while (fgets(let, TAM, entrada) != NULL)
    {
        for (i = 0; let[i] != '\0'; i++)
        {
            if (isalpha((unsigned char)let[i]))
            {
                n[toupper((unsigned char)let[i]) - 'A']++;
                len_w++;
            }
        }
    }

    for (i = 0; i < ALPHABET; i++)
    {
        prob_x[i] = (double)n[i] / len_w;
    }

    for (i = 0; i < ALPHABET; i++)
    {
        prob_y[i] = 0;

        for (j = 0; j < ALPHABET; j++)
        {
            prob_conj[j][i] = prob_x[j] * prob_k[(i - j + ALPHABET) % ALPHABET]; /*P(X,Y)*/
            prob_y[i] += prob_conj[j][i];
        }
    }

    for (i = 0; i < ALPHABET; i++)
    {
        for (j = 0; j < ALPHABET; j++)
        {
            if (prob_y[j] > 0)
                prob_conj[i][j] /= prob_y[j]; /*  P(X,Y)/P(Y)  */
            else
                prob_conj[i][j] = 0;

            if (prob_y[j] > 0 && fabs(prob_conj[i][j] - prob_x[i]) > 0.00001)
                err++;
        }
    }

    for (i = 0; i < ALPHABET; i++)
    {
        fprintf(salida, "P(%c) = %lf\n", i + 'A', prob_x[i]);
    }

    for (i = 0; i < ALPHABET; i++)
    {
        for (j = 0; j < ALPHABET; j++)
        {
            fprintf(salida, "P(%c|%c) = %lf", i + 'A', j + 'A', prob_conj[i][j]);
        }
        fprintf(salida, "\n");
    }

    fprintf(salida, "Modo %s | Numero de fallos: %d/%d | Longitud del texto: %d\n", met ? "equiprobable" : "no equiprobable", err, ALPHABET * ALPHABET, len_w);

    if (entrada != stdin)
        fclose(entrada);

    if (salida != stdout)
        fclose(salida);

    if (let != NULL)
        free(let);

    return ret;
}
