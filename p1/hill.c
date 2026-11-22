#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#include <ctype.h>

#include <gmp.h>

#include "utils.h"

#define MAX_N 4

/*#include "gmp.h"*/

int det(int **k, int n);
void matriz_adjunta_traspuesta(int **k, int n, int **adj, int m);
void matriz_inversa(int **k, int n, int m, int **inv);
void hill(int *x, int **k, int n, int m, int *res);
void clear(FILE *entrada, FILE *salida, FILE *matriz, char *code, char *let, int **k, int *res, int **inv, int n);

int main(int argc, char *argv[])
{
    FILE *entrada = stdin, *salida = stdout, *matriz = NULL;

    int encode = 0, ret = 0, param = 0;

    char *code = NULL, *let = NULL;

    int m, n = -1, **k = NULL, **inv = NULL;

    int ch = 0, i, j, len, hill_val[MAX_N], *res = NULL, rv, end = 0, total_blocks;

    int idx = 0, num, **key = NULL;

    for (i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-C") == 0)
        {
            encode = 0;
            if (param >= 1000)
            {
                fprintf(stderr, "Error: no se puede codificar y decodificar al mismo tiempo\n");
                return -1;
            }
            param += 1000;
        }
        else if (strcmp(argv[i], "-D") == 0)
        {
            encode = 1;
            if (param >= 1000)
            {
                fprintf(stderr, "Error: no se puede codificar y decodificar al mismo tiempo\n");

                return -1;
            }
            param += 1000;
        }
        else if (strcmp(argv[i], "-m") == 0 && i + 1 < argc)
        {
            m = atoi(argv[i + 1]);
            i++;
            param++;
        }
        else if (strcmp(argv[i], "-n") == 0 && i + 1 < argc)
        {
            n = atoi(argv[i + 1]);
            i++;
            param += 10;
        }
        else if (strcmp(argv[i], "-k") == 0 && i + 1 < argc)
        {
            matriz = fopen(argv[i + 1], "r");
            if (!matriz)
            {
                fprintf(stderr, "Error al abrir el fichero de la matriz\n");
                clear(entrada, salida, matriz, code, let, k, res, inv, n);
                return -1;
            }
            i++;
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
    if (param != 1111 || n <= 0 || n > 4)
    {
        fprintf(stderr, "Uso -C/-D -m [cardinalidad de Zm] -n [dimension de la matriz de transformacion] -k [fichero que contiene la matriz de transformacion] -i [fichero de entrada] -o [fichero de salida]\n");
        return -1;
    }
    if ((k = malloc(n * sizeof(int *))) == NULL)
    {
        fprintf(stderr, "Error al reservar memoria\n");
        clear(entrada, salida, matriz, code, let, k, res, inv, n);
        ret = -1;
    }
    for (i = 0; i < n; i++)
    {
        if ((k[i] = malloc(n * sizeof(int))) == NULL)
        {
            fprintf(stderr, "Error al reservar memoria\n");
            clear(entrada, salida, matriz, code, let, k, res, inv, n);
            ret = -1;
        }
    }
    for (i = 0; i < n; i++)
    {
        for (j = 0; j < n; j++)
        {
            fscanf(matriz, "%d", &k[i][j]);
        }
    }
    if ((res = malloc(n * sizeof(int))) == NULL)
    {
        fprintf(stderr, "Error al reservar memoria\n");
        clear(entrada, salida, matriz, code, let, k, res, inv, n);
        ret = -1;
    }
    if (mcd(det(k, n), m) != 1)
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
        else if (!(let = malloc(TAM * sizeof(char))))
        {
            fprintf(stderr, "Error al reservar memoria\n");
            ret = -1;
        }
        else
        {
            if (!encode)
                key = k;
            else
            {
                if ((inv = malloc(n * sizeof(int *))) == NULL)
                {
                    fprintf(stderr, "Error al reservar memoria para la inversa\n");
                    clear(entrada, salida, matriz, code, let, k, res, inv, n);
                    return -1;
                }
                for (i = 0; i < n; i++)
                {
                    if ((inv[i] = malloc(n * sizeof(int))) == NULL)
                    {
                        fprintf(stderr, "Error al reservar memoria para la inversa\n");
                        clear(entrada, salida, matriz, code, let, k, res, inv, n);
                        return -1;
                    }
                }
                matriz_inversa(k, n, m, inv);
                key = inv;
            }

            fgets(let, TAM, entrada);
            len = strlen(let);
            if (len > 0 && let[len -1] == '\n') {
                let[len - 1] = '\0';
                len--;
            }
            rv = 0;

            total_blocks = (len + n - 1) / n;
            if (total_blocks == 0) total_blocks = 1;

            for (i = 0; i < total_blocks; i++)
            {
                for (j = 0; j < n; j++)
                {
                    if (rv > len)
                    {
                        end = 1;
                    }
                    if (!end)
                    {
                        hill_val[j] = char_to_number(let[rv]);
                        rv++;
                    }
                    else{
                        hill_val[j] = 'w' - 'a';
                    }

                    if (hill_val[j] < 0)
                    {
                        j--;
                    }
                }

                hill(hill_val, key, n, m, res);
                for (j = 0; j < n; j++)
                {
                    code[idx] = number_to_char(res[j]);
                    idx++;
                }

                if (end)
                    break;
            }
            code[idx] = '\0';

            fprintf(salida, "%s\n", code);
        }
    }

    clear(entrada, salida, matriz, code, let, k, res, inv, n);
    return ret;
}

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

void matriz_adjunta_traspuesta(int **k, int n, int **adj, int m)
{
    int i, j, sign;
    int x, y;
    int ti, tj;
    if (n == 1)
    {
        adj[0][0] = 1;
        return;
    }

    int **temp = (int **)malloc((n - 1) * sizeof(int *));
    for (i = 0; i < n - 1; i++)
    {
        temp[i] = (int *)malloc((n - 1) * sizeof(int));
    }

    for (i = 0; i < n; i++)
    {
        for (j = 0; j < n; j++)
        {
            ti = 0;
            for (x = 0; x < n; x++)
            {
                if (x == i)
                    continue;
                tj = 0;
                for (y = 0; y < n; y++)
                {
                    if (y != j)
                    {
                        temp[ti][tj] = k[x][y];
                        tj++;
                    }
                }
                ti++;
            }
            sign = ((i + j) % 2 == 0) ? 1 : -1;
            adj[j][i] = (sign * det(temp, n - 1)) % m;
        }
    }

    for (i = 0; i < n - 1; i++)
    {
        free(temp[i]);
    }
    free(temp);
}

void matriz_inversa(int **k, int n, int m, int **inv)
{
    int i, j;
    int d = det(k, n) % m;
    if (d < 0)
        d += m;

    matriz_adjunta_traspuesta(k, n, inv, m);
    for (i = 0; i < n; i++)
    {
        for (j = 0; j < n; j++)
        {
            inv[i][j] = (inv[i][j] * euclides_extendido(m, d)) % m;
            if (inv[i][j] < 0)
            {
                inv[i][j] += m;
            }
        }
    }
}

void hill(int *x, int **k, int n, int m, int *res)
{
    int i, j;
    for (i = 0; i < n; i++)
    {
        res[i] = 0;
        for (j = 0; j < n; j++)
        {
            res[i] += k[i][j] * x[j];
        }
        res[i] %= m;
    }
}

void clear(FILE *entrada, FILE *salida, FILE *matriz, char *code, char *let, int **k, int *res, int **inv, int n)
{
    int i;

    if (entrada && entrada != stdin)
        fclose(entrada);

    if (salida && salida != stdout)
        fclose(salida);

    if (matriz)
        fclose(matriz);

    if (code)
        free(code);

    if (let)
        free(let);

    if (k)
    {
        for (i = 0; i < n; i++)
        {
            if (k[i])
                free(k[i]);
        }

        free(k);
    }

    if (inv)
    {
        for (i = 0; i < n; i++)
        {
            if (inv[i])
                free(inv[i]);
        }

        free(inv);
    }

    if (res)
        free(res);
}