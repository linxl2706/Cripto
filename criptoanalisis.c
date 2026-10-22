#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#include <ctype.h>

#include <math.h>

#include <gmp.h>

#include "utils.h"

#define ALPHABET 26
#define TK 100

typedef struct
{
    double times;
    char let;
} Freq;

typedef struct
{
    double tam[TK];
    double freq[TK];
    Freq let_freq[TK][ALPHABET];
} Lang;

int kasiski(int l, char *let, int len, int i);
int ic(int l, char *let, int len, Lang lang_freq, Lang *lf);
void calculo_k(int l, char *let, int len, int *k, int k_len, Lang lang, Lang *lf);
void clear(char *let, int *k, Lang *lf);

int main(int argc, char *argv[])
{
    FILE *entrada = stdin, *salida = stdout;

    int l;

    int param = 0, ret = 0;

    int *k = NULL;

    char *let = NULL, code[TK], ch;

    int i, j, len, k_len, k_ic;

    double t_eng = 0, t_esp = 0;

    Lang *lf;

    Lang esp = {
        {0},
        {0},
        {{{1196, 'A'}, {92, 'B'}, {292, 'C'}, {687, 'D'}, {1678, 'E'}, {52, 'F'}, {73, 'G'}, {89, 'H'}, {415, 'I'}, {30, 'J'}, {0, 'K'}, {837, 'L'}, {212, 'M'}, {701, 'N'}, {869, 'O'}, {277, 'P'}, {153, 'Q'}, {494, 'R'}, {788, 'S'}, {331, 'T'}, {480, 'U'}, {39, 'V'}, {0, 'W'}, {6, 'X'}, {154, 'Y'}, {15, 'Z'}}}};

    Lang eng = {
        {0},
        {0},
        {{{804, 'A'}, {154, 'B'}, {306, 'C'}, {399, 'D'}, {1251, 'E'}, {230, 'F'}, {196, 'G'}, {549, 'H'}, {726, 'I'}, {16, 'J'}, {67, 'K'}, {414, 'L'}, {253, 'M'}, {709, 'N'}, {760, 'O'}, {200, 'P'}, {11, 'Q'}, {612, 'R'}, {654, 'S'}, {925, 'T'}, {271, 'U'}, {99, 'V'}, {192, 'W'}, {19, 'X'}, {173, 'Y'}, {19, 'Z'}}}};

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
    let = (char *)malloc(TK * sizeof(char));
    if (!let)
    {
        clear(let, k, lf);
    }
    fgets(let, TK, entrada);
    len = strlen(let);
    if (len > 0 && let[len - 1] == '\n')
    {
        let[len - 1] = '\0';
        len--;
    }
    lf = (Lang *)malloc(sizeof(Lang));
    if (!lf)
    {
        clear(let, k, lf);
    }
    for (i = 0; i < ALPHABET; i++)
    {
        eng.tam[0] += eng.let_freq[0][i].times;
        esp.tam[0] += esp.let_freq[0][i].times;
    }
    for (i = 0; i < ALPHABET; i++)
    {
        t_eng = eng.let_freq[0][i].times / esp.tam[0];
        t_esp = esp.let_freq[0][i].times / esp.tam[0];
        eng.freq[0] += t_eng * t_eng;
        esp.freq[0] += t_esp * t_esp;
    }
    k_ic = ic(l, let, len, esp, lf);
    for (i = 0; i < len; i++)
    {
        k_len = kasiski(l, let, len, i);
        if (k_len == k_ic)
            break;
    }
    if (k_ic < 1)
    {
        clear(let, k, lf);
        return -1;
    }
    k = (int *)malloc(k_ic * sizeof(int));
    if (!k)
    {
        clear(let, k, lf);
    }

    calculo_k(l, let, len, k, k_ic, esp, lf);
    for (i = 0, j = 0; i < len; i++, j++)
    {
        ch = let[i] + k[j];
        if (ch > 'z')
            ch -= 26;
        code[i] = ch;
    }
    code[i] = '\0';
    fprintf(salida, "longitud de la clave: %d\n", k_len);
    fprintf(salida, "clave: {");
    for (i = 0; i < k_len; i++)
    {
        fprintf(salida, "%d", k[i]);
        if (i + 1 < k_len)
            fprintf(salida, ", ");
    }
    fprintf(salida, "}\ntexto descifrado:\n%s\n", code);
    clear(let, k, lf);
    return ret;
}

int kasiski(int l, char *let, int len, int i)
{
    char str[l + 1], st[l + 1];
    int j, k;
    int ret = -1;
    int dis[TK], d = 0;
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
        for (k = 0; k < l; k++)
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
    if (d <= 2)
        return ret;
    ret = mcd(dis[0], dis[1]);
    for (j = 2 ;j < d; j++)
    {
        ret = mcd(ret, dis[j]);
    }

    return ret;
}

int ic(int l, char *let, int len, Lang lang_freq, Lang *lf)
{
    int i, j, k;
    double f;
    int find = 0;
    double rel;
    int ch;
    for (k = 1; k <= TK; k++)
    {
        for (j = 0; j < k; j++)
        {
            for (i = 0; i < ALPHABET; i++)
            {
                lf->let_freq[j][i].let = 'a' + i;
                lf->let_freq[j][i].times = 0;
            }
            for (i = j; i < len; i += k)
            {
                ch = char_to_number(let[i]);
                if (ch < 0 || ch >= ALPHABET)
                    continue;

                lf->let_freq[j][ch].times++;
            }

            for (i = 0; i < ALPHABET; i++)
            {
                lf->tam[j] += lf->let_freq[j][i].times;
            }

            lf->freq[j] = 0;
            for (i = 0; i < ALPHABET; i++)
            {
                f = lf->let_freq[j][i].times / lf->tam[j];
                lf->freq[j] += f * f;
            }
        }
        for (j = 0; j < k; j++)
        {
            rel = lf->freq[j] / lang_freq.freq[0];
            if (rel > 0)
                rel = -rel;
            if (rel < 0.9)
                find = 1;
        }
        if (find)
            break;
    }

    return k;
}

void calculo_k(int l, char *let, int len, int *k, int k_len, Lang lang, Lang *lf)
{
    int i, j;
    int ki;
    double total, best;
    for (i = 0; i < k_len; i++)
    {
        for (ki = 0; ki < ALPHABET; ki++)
        {
            best = 0;
            total = 0;
            for (j = 0; j + i * k_len < ALPHABET; j++)
            {
                total += lang.let_freq[0][j].times * lf->let_freq[i][(j + ki) % ALPHABET].times / lf->tam[i];
            }

            if (!ki)
            {
                k[i] = ki;
                best = total;
            }
            else if (fabs(total - lang.freq[0]) < fabs(best - lang.freq[0]))
            {
                best = total;
                k[i] = ki;
            }
        }
    }
}

void clear(char *let, int *k, Lang *lf)
{
    if (let)
        free(let);

    if (k)
        free(k);

    if (lf)
        free(lf);
}