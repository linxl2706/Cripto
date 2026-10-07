#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#include <ctype.h>

#include <gmp.h>

#include "utils.h"

#define MAX_N 4

/*#include "gmp.h"*/
int det(int **k, int n)
{
    int d = 0;
    if (n == 1)
    {
        d = k[0][0];
    }
    else if (n == 2)
    {
        d = k[0][0] * k[1][1] - k[0][1] * k[1][0];
    }
    else if (n == 3)
    {
        d = k[0][0] * (k[1][1] * k[2][2] - k[1][2] * k[2][1]) -
            k[0][1] * (k[1][0] * k[2][2] - k[1][2] * k[2][0]) +
            k[0][2] * (k[1][0] * k[2][1] - k[1][1] * k[2][0]);
    }
    else if (n == 4)
    {
        d = k[0][0] * (k[1][1] * (k[2][2] * k[3][3] - k[2][3] * k[3][2]) -
                       k[1][2] * (k[2][1] * k[3][3] - k[2][3] * k[3][1]) +
                       k[1][3] * (k[2][1] * k[3][2] - k[2][2] * k[3][1])) -
            k[0][1] * (k[1][0] * (k[2][2] * k[3][3] - k[2][3] * k[3][2]) -
                       k[1][2] * (k[2][0] * k[3][3] - k[2][3] * k[3][0]) +
                       k[1][3] * (k[2][0] * k[3][2] - k[2][2] * k[3][0])) +
            k[0][2] * (k[1][0] * (k[2][1] * k[3][3] - k[2][3] * k[3][1]) -
                       k[1][1] * (k[2][0] * k[3][3] - k[2][3] * k[3][0]) +
                       k[1][3] * (k[2][0] * k[3][1] - k[2][1] * k[3][0])) -
            k[0][3] * (k[1][0] * (k[2][1] * k[3][2] - k[2][2] * k[3][1]) -
                       k[1][1] * (k[2][0] * k[3][2] - k[2][2] * k[3][0]) +
                       k[1][2] * (k[2][0] * k[3][1] - k[2][1] * k[3][0]));
    }
    return d;
}

void matriz_invertida(int **k, int n, int m)
{   

}

void hill(int *x, int **k, int n, int m, int *res)
{
    for (int i = 0; i < n; i++)
    {
        res[i] = 0;
        for (int j = 0; j < n; j++)
        {
            res[i] += k[i][j] * x[j];
        }
        res[i] %= m;
    }
}

int main(int argc, char *argv[])
{
    FILE *entrada = stdin, *salida = stdout, *matriz = NULL;

    int encode = 0, ret = 0;

    char *code = NULL;

    int m, n, **k = NULL;

    int ch = 0, i, j, end, hill_val[MAX_N], *res = NULL;

    for (int i = 1; i < argc; i++)
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
        else if (strcmp(argv[i], "-n") == 0 && i + 1 < argc)
        {
            n = atoi(argv[i + 1]);
            i++;
        }
        else if (strcmp(argv[i], "-k") == 0 && i + 1 < argc)
        {
            matriz = fopen(argv[i + 1], "r");
            if (!matriz)
            {
                fprintf(stderr, "Error al abrir el fichero de entrada\n");
                return 1;
            }
            i++;
        }
        else if (strcmp(argv[i], "-i") == 0 && i + 1 < argc)
        {
            entrada = fopen(argv[i + 1], "r");
            if (!entrada)
            {
                fprintf(stderr, "Error al abrir el fichero de entrada\n");
                return 1;
            }
            i++;
        }
        else if (strcmp(argv[i], "-o") == 0 && i + 1 < argc)
        {
            salida = fopen(argv[i + 1], "w");
            if (!salida)
            {
                fprintf(stderr, "Error al abrir el fichero de salida\n");
                return 1;
            }
            i++;
        }
    }
    if (n <= 0 || n > 4)
    {
        fprintf(stderr, "El valor de n debe estar entre 1 y 4\n");
        fclose(matriz);
        fclose(entrada);
        fclose(salida);
        return -1;
    }
    if (k = malloc(n * sizeof(int *)) == NULL)
    {
        fprintf(stderr, "Error al reservar memoria\n");
        fclose(matriz);
        fclose(entrada);
        fclose(salida);
        ret = -1;
    }
    for (int i = 0; i < n; i++)
    {
        if (k[i] = malloc(n * sizeof(int)) == NULL)
        {
            fprintf(stderr, "Error al reservar memoria\n");
            for (int j = 0; j < i; j++)
            {
                free(k[j]);
            }
            free(k);
            fclose(matriz);
            fclose(entrada);
            fclose(salida);
            ret = -1;
        }
    }
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            fscanf(matriz, "%d", &k[i][j]);
        }
    }
    if ((res = malloc(n * sizeof(int))) == NULL)
    {
        fprintf(stderr, "Error al reservar memoria\n");
        ç for (int j = 0; j < n; j++)
        {
            free(k[j]);
        }
        free(k);
        fclose(matriz);
        fclose(entrada);
        fclose(salida);
        ret = -1;
    }

    if (mcd(det(k), m) != 1)
    {
        fprintf(stderr, "No existe inyectividad\n");
        ret = -1;
    }
    else
    {
        if (!(code = malloc(TAM * sizeof(char))))
        {
            fprintf(stderr, "Error al reservar memoria\n");
            ret = -1;
        }

        else
        {
            if (!encode)
            {
                for (i = 0; i < TAM / n - 1; i++)
                {
                    end = 0;
                    for (int j = 0; j < n; j++)
                    {
                        if ((ch = fgetc(entrada)) != EOF)
                        {
                            hill_val[j] = char_to_number(ch);
                        }
                        else
                        {
                            hill_val[j] = 25;
                            end = 1;
                        }
                    }
                    hill(hill_val, k, n, m, res);
                    for (int j = 0; j < n; j++)
                    {
                        code[i * n + j] = number_to_char(res[j]);
                    }
                    if (end)
                    {
                        code[i * n + j] = '\0';
                        break;
                    }
                }
            }
            else
            {
                matriz_invertida(k, n, m);
                for (i = 0; i < TAM / n - 1; i++)
                {
                    end = 0;
                    for (int j = 0; j < n; j++)
                    {
                        if ((ch = fgetc(entrada)) != EOF)
                        {
                            hill_val[j] = char_to_number(ch);
                        }
                        else
                        {
                            hill_val[j] = 25;
                            end = 1;
                        }
                    }

                    hill(hill_val, k, n, m, res);
                    for (int j = 0; j < n; j++)
                    {
                        code[i * n + j] = number_to_char(res[j]);
                    }
                    if (end)
                    {
                        code[i * n + j] = '\0';
                        break;
                    }
                }
            }
            fprintf(salida, "%s\n", code);
            free(code);
        }
    }

    if (entrada != stdin)
        fclose(entrada);

    if (salida != stdout)
        fclose(salida);

    for (int i = 0; i < n; i++)
    {
        free(k[i]);
    }

    free(k);

    free(res);

    fclose(matriz);
    return ret;
}
