#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#include <ctype.h>

#include <gmp.h>

/*#include "gmp.h"*/

#define TAM 4096

int char_to_number(int c) {
    if (isupper(c)) return c - 'A';
    if (islower(c)) return c - 'a';
    return -1;
}

char number_to_char(int num) {
    if (num >= 0 && num <= 25) return num + 'a';
    return '\0';
}

int euclides(mpz_t a, mpz_t b)
{
  int res;
  mpz_t result, r0, r1, r2;

  mpz_init_set(r0, a);
  mpz_init_set(r1, b);
  mpz_init(r2);

  /*Comprobacion de valores*/
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
    mpz_set(r0, r1);     // r0 = r1
    mpz_set(r1, r2);     // r1 = r2
    mpz_mod(r2, r0, r1); // r2 = r0 % r1
  }
  res = mpz_get_si(r1);

  mpz_clear(r0);
  mpz_clear(r1);
  mpz_clear(r2);

  return res;
}

/*
int euclides_extendido(int a, int b)
{
  int r0 = a, r1 = b, q, r2;
  if (a == 0 && b == 0)
  {
    return -1;
  }
  if (b > a)
  {
    r0 = b;
    r1 = a;
  }
  r2 = r0 % r1;
  while (r2 != 0)
  {
    r0 = r1;
    r1 = r2;
    r2 = r0 % r1;
  }
  return r1;
}
  */

int afin(int num, mpz_t a, mpz_t b, mpz_t m)
{
  mpz_t x, res;
  mpz_init_set_si(x, num);
  mpz_init(res);

  mpz_mul(res, x, a);
  mpz_add(res, res, b);
  mpz_mod(res, res, m);

  int resultado = mpz_get_si(res);

  mpz_clear(x);
  mpz_clear(res);

  return resultado;
}

int main(int argc, char *argv[])
{

  mpz_t a, b, m;

  int modo;

  FILE *entrada = stdin, *salida = stdout;

  int encode = 0;

  char *code = NULL;

  int ch, idx = 0;

  int ret = 0;

  mpz_init(a);
  mpz_init(b);
  mpz_init(m);

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
      mpz_set_str(m, argv[i + 1], 10);
      i++;
    }
    else if (strcmp(argv[i], "-a") == 0 && i + 1 < argc)
    {
      mpz_set_str(a, argv[i + 1], 10);
      i++;
    }
    else if (strcmp(argv[i], "-b") == 0 && i + 1 < argc)
    {
      mpz_set_str(b, argv[i + 1], 10);
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

    else
    {
      if (!encode)
      {

        while ((ch = fgetc(entrada)) != EOF && idx < TAM - 1)
        {
          code[idx++] = number_to_char(afin(char_to_number(ch), a, b, m));
        }
        code[idx] = '\0';
      }/*
      else
      {
        while ((ch = fgetc(entrada)) != EOF && idx < TAM - 1)
        {
          code[idx++] = number_to_char(euclides_extendido(char_to_number(ch), a, b, m));
        }
        code[idx] = '\0';
      }
      */
      fprintf(salida, "%s\n", code);
      free(code);
    }
  }

  mpz_clear(a);

  mpz_clear(b);

  mpz_clear(m);

  if (entrada != stdin)
    fclose(entrada);

  if (salida != stdout)
    fclose(salida);

  return ret;
}
