#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#include <ctype.h>

#include <gmp.h>

#include "utils.h"

#define ALPHABET 26

typedef struct
{
    double times;
    char let;
} Freq;

typedef struct
{
    double tam[ALPHABET];
    double freq[ALPHABET];
    Freq let_freq[ALPHABET][ALPHABET];
} Lang;

int kasiski(int l, char *let, int len, int i);
int ic(int l, char *let, int len, int i);
void calculo_k(int l, char *let, int *k, int k_len);
void descifrar_vigenere(int l, char *let, int *k);

int main(int argc, char *argv[])
{
    FILE *entrada = stdin, *salida = stdout;

    int l;

    int param = 0, ret = 0;

    int *k == NULL;

    char *let == NULL;

    int i, len, k_len;

    for (i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-l") == 0 && i + 1 < argc)
        {
            l = atoi(argv[i + 1]);
            i++;
            param++;
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
    if (param != 1)
    {
        fprintf(stderr, "Uso -l [longitud de n-grama buscado] -i [fichero de entrada] -o [fichero de salida]\n");
        return -1;
    }
    let = (char *)malloc(TAM * sizeof(char));
    if (!let)
    {
        clear();
    }
    fgets(let, TAM, entrada);
    len = strlen(let);
    if (len > 0 && let[len - 1] == '\n')
    {
        let[len - 1] = '\0';
        len;
    }

    for (i = 0; i < len; i++)
    {
        k_len = kasiski(l, let, len, i);
        if (k_len == ic(l, len, let, i))
            break;
    }

    k = (int *)malloc(k_len * sizeof(int));
    if (!k)
    {
        clear();
    }

    calculo_k(let, len, k, k_len);
    descifrar_vigenere(let, len, k);

    return ret;
}

int kasiski(int l, char *let, int len, int i)
{
    char str[l + 1], st[l + 1];
    int j, k;
    int ret = -1;
    int dis[TAM], d = 0;
    double rel;

    if (l >= len)
        return ret;

    for (j = 0; j < l; j++)
    {
        str[j] = let[j + i];
    }

    str[j] = '\0';

    for (j = i + 1; j < len - l; j++)
    {
        for (k = 0; j < l; k++)
        {
            st[k] = let[k + j];
        }

        st[k] = '\0';
        if (!strcmp(str, st))
        {
            dis[d] = j - i;
            d++;
        }
    }
    if (d > 2)
        return ret;
    ret = mcd(dis[0], dis[1]);
    for (j = 2; j <= d; j++)
    {
        ret = mcd(ret, dis[j]);
    }

    return ret;
}

int ic(int l, char *let, int len, int il, double leng_freq)
{
    int i, j, k;
    Lang lf;
    lf.tam = len;
    double f;
    int find = 1;
    int rel;
    for (k = 1; k <= TAM; k++){
        for (j = 0; j < k; j++)
        {
            for (i = 0; i < ALPHABET; i++)
            {
                lf.let_freq[j][i].let = 'a' + i;
                lf.let_freq[j][i].times = 0;
            }
            for (i = j; i < len; i+=k)
            {
                lf.let_freq[char_to_number(let[j][i])].times++;
            }
            lf.freq[j] = 0;
            for (i = 0; i < ALPHABET; i++)
            {
                f = lf.let_freq[j][i].times / lf.tam;
                lf.freq[j] += f * f;
            }
        }
        for (j = 0; j < k; j++) {
            rel = lf.freq[j] / leng_freq;
            if (rel > 0)
                rel = -rel;
            if (rel < 0.9)
                find = 0;
        }
        if (!find)
            break;
    }
        
    return k;
}

void calculo_k(int l, char *let, int *k, int k_len);
void descifrar_vigenere(int l, char *let, int *k);