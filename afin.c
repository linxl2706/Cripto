#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#include <ctype.h>

#include <gmp.h>

#include "utils.h"

/*#include "gmp.h"*/

int euclides(mpz_t a, mpz_t b);
void euclides_ext(mpz_t r0, mpz_t r1, mpz_t mod, mpz_t a_inv);
int afin_enc(int num, mpz_t a, mpz_t b, mpz_t m);
int afin_dec(int num, mpz_t a, mpz_t b, mpz_t m);
void permute(int n, char *s, int encode);
void clear(FILE *entrada, FILE *salida, char *code, char *let, mpz_t a, mpz_t b, mpz_t m, int param);

int main(int argc, char *argv[])
{
  mpz_t a, b, m;

  FILE *entrada = stdin, *salida = stdout;

  int encode, param = 0, ret = 0;

  int i, idx = 0;

  char *code = NULL, *let = NULL;

  int (*func)(int, mpz_t, mpz_t, mpz_t), num;

  int p = 1;

  mpz_init(a);
  mpz_init(b);
  mpz_init(m);

  for (i = 1; i < argc; i++)
  {
    if (strcmp(argv[i], "-C") == 0)
    {
      encode = 0;
      if (param >= 1000)
      {
        fprintf(stderr, "Error: no se puede codificar y decodificar al mismo tiempo\n");
        clear(entrada, salida, code, let, a, b, m, param);
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
        clear(entrada, salida, code, let, a, b, m, param);
        return -1;
      }
      param += 1000;
    }
    else if (strcmp(argv[i], "-m") == 0 && i + 1 < argc)
    {
      mpz_set_str(m, argv[i + 1], 10);
      i++;
      param++;
    }
    else if (strcmp(argv[i], "-a") == 0 && i + 1 < argc)
    {
      mpz_set_str(a, argv[i + 1], 10);
      i++;
      param += 10;
    }
    else if (strcmp(argv[i], "-b") == 0 && i + 1 < argc)
    {
      mpz_set_str(b, argv[i + 1], 10);
      i++;
      param += 100;
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
    else if (strcmp(argv[i], "-p") == 0)
    {
      p = atoi(argv[i + 1]);
      i++;
    }
  }
  if (param != 1111)
  {
    fprintf(stderr, "Uso -C/-D -m [tamano del espacio de texto cifrado] -a [coeficiente multiplicativo de la funcion afin] -b [termino constante de la funcion afin] -i [fichero de entrada] -o [fichero de salida] -p [(opcional) numero a permutar]\n");
    ret = -1;
  }
  if (euclides(a, m) != 1)
  {
    fprintf(stderr, "No existe inverso multiplicativo\n");
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
        func = afin_enc;
      else
        func = afin_dec;

      fgets(let, TAM, entrada);
      for (i = 0; i < TAM - 1; i++)
      {
          num = char_to_number(let[i]);
          if (num >= 0){
            num = func(num, a, b, m);
            code[idx] = number_to_char(num);
            idx++;
          }
      }
      code[idx] = '\0';

      permute(p, code, encode);

      fprintf(salida, "%s\n", code);
    }
  }

  clear(entrada, salida, code, let, a, b, m, param);

  return ret;
}

int euclides(mpz_t a, mpz_t b)
{
  int res;
  mpz_t result, r0, r1, r2;

  mpz_init_set(r0, a);
  mpz_init_set(r1, b);
  mpz_init(r2);

  if (mpz_cmp_ui(a, 0) == 0 && mpz_cmp_ui(b, 0) == 0)
  {
    mpz_clear(r0);
    mpz_clear(r1);
    mpz_clear(r2);
    return -1;
  }

  if (mpz_cmp(b, a) > 0)
  {
    mpz_set(r0, b);
    mpz_set(r1, a);
  }

  mpz_mod(r2, r0, r1);
  while (mpz_cmp_ui(r2, 0) != 0)
  {
    mpz_set(r0, r1);
    mpz_set(r1, r2);
    mpz_mod(r2, r0, r1);
  }
  res = mpz_get_si(r1);

  mpz_clear(r0);
  mpz_clear(r1);
  mpz_clear(r2);

  return res;
}

void euclides_ext(mpz_t r0, mpz_t r1, mpz_t mod, mpz_t a_inv)
{
  mpz_t u0, u1, u_i;
  mpz_t v0, v1, v_i;

  mpz_t q;                             // quoziente
  mpz_t local_r0, local_r1, local_mod; // r0, r1 e modulo locali per non sporcare il main
  mpz_t temp;                          // variabile temporanea per suddivisione compiti

  mpz_init(q);
  mpz_init(temp);
  mpz_init(v_i);
  mpz_init(u_i);

  mpz_init_set_ui(u0, 1);
  mpz_init_set_ui(u1, 0);
  mpz_init_set_ui(v0, 0);
  mpz_init_set_ui(v1, 1);
  mpz_init_set(local_r0, r0);
  mpz_init_set(local_r1, r1);
  mpz_init_set(local_mod, mod);

  // riadatto euclide (versione esteso) (r0 = m, r1 = a) quindi v_0 = a^-1 mod m
  while (mpz_cmp_ui(local_r1, 0) != 0)
  {
    mpz_mod(local_mod, local_r0, local_r1); // local_mod = local_r0 % local_r1;
    mpz_tdiv_q(q, local_r0, local_r1);      //         q = local_r0 / local_r1;
    mpz_set(local_r0, local_r1);            //  local_r0 = local_r1;
    mpz_set(local_r1, local_mod);           //  local_r1 = local_mod;

    // calcoli su u0, u1, u_i
    mpz_mul(temp, q, u1);
    mpz_sub(temp, u0, temp);
    mpz_set(u_i, temp); // u_i = u0 - q * u1;

    mpz_set(u0, u1);  // u0 = u1;
    mpz_set(u1, u_i); // u1 = u_i;

    // calcoli su v0, v1, v_i
    mpz_mul(temp, q, v1);
    mpz_sub(temp, v0, temp);
    mpz_set(v_i, temp); // v_i = v0 - q * v1;

    mpz_set(v0, v1);  // v0 = v1;
    mpz_set(v1, v_i); // v1 = v_i;
  }

  if (mpz_cmp_ui(v0, 0) < 0)
  {
    mpz_add(temp, v0, mod);
    mpz_set(v0, temp); // v0 += mod;
  }

  mpz_set(a_inv, v0); // in questo modo passo v0 alla variabile a_inv e posso usarla nel main

  mpz_clear(u0);
  mpz_clear(u1);
  mpz_clear(u_i);

  mpz_clear(v0);
  mpz_clear(v1);
  mpz_clear(v_i);

  mpz_clear(local_mod);
  mpz_clear(local_r0);
  mpz_clear(local_r1);
  mpz_clear(q);
  mpz_clear(temp);
}

int afin_enc(int num, mpz_t a, mpz_t b, mpz_t m)
{
  mpz_t x, res;
  int resultado;
  mpz_init_set_si(x, num);
  mpz_init(res);

  mpz_mul(res, x, a);
  mpz_add(res, res, b);
  mpz_mod(res, res, m);

  resultado = mpz_get_si(res);

  mpz_clear(x);
  mpz_clear(res);

  return resultado;
}

int afin_dec(int num, mpz_t a, mpz_t b, mpz_t m)
{
  mpz_t a_inv, x, res;
  int resultado;
  mpz_init(a_inv);
  mpz_init_set_si(x, num);
  mpz_init(res);

  euclides_ext(m, a, m, a_inv);

  mpz_sub(res, x, b);
  mpz_mul(res, res, a_inv);
  mpz_mod(res, res, m);

  resultado = mpz_get_si(res);

  mpz_clear(a_inv);
  mpz_clear(x);
  mpz_clear(res);

  return resultado;
}
void permute(int n, char *s, int encode) {
    int i, j;
    int len = strlen(s);
    int start, end, block_size;
    char temp;
    
    if (n <= 1 || !s || len == 0)
        return;

    for (i = 0; i * n < len; i++) {
        start = i * n;
        end = (i + 1) * n < len ? (i + 1) * n : len;
        block_size = end - start;

        if (block_size <= 1) 
          continue;

        if (encode == 0) {
            temp = s[start];
            j;
            for (j = start; j < end - 1; j++) {
                s[j] = s[j + 1];
            }
            s[end - 1] = temp;
        } else {
            temp = s[end - 1];
            j;
            for (j = end - 1; j > start; j--) {
                s[j] = s[j - 1];
            }
            s[start] = temp;
        }
    }
}

void clear(FILE *entrada, FILE *salida, char *code, char *let, mpz_t a, mpz_t b, mpz_t m, int param) {
  if (param % 10 == 1)
    mpz_clear(a);

  if (param % 100 >= 10)
    mpz_clear(b);
    
  if (param % 1000 >= 100)
    mpz_clear(m);

  if(entrada && entrada!=stdin)
    fclose(entrada);

  if(salida && salida!=stdout)
    fclose(salida);

  if(code)
    free(code);

  if(let)
    free(let);
}