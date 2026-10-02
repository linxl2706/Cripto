#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#include <ctype.h>

#include <gmp.h>

/*#include "gmp.h"*/

#define TAM 4096

int *char_to_number(char *c)
{
  int i;
  int *num = malloc(strlen(c) * sizeof(int));
  for (i = 0; i < strlen(c); i++)
  {
    if (c[i] >= 'A' && c[i] <= 'Z')
      num[i] = c[i] - 'A';
    else if (c[i] >= 'a' && c[i] <= 'z')
      num[i] = c[i] - 'a';
  }
  return num;
}

int *number_to_char(int *num)
{
  int i;
  char *c = malloc(strlen(num) * sizeof(char));
  for (i = 0; i < strlen(num); i++)
  {
    if (num[i] >= 0 && num[i] <= 25)
      c[i] = num[i] + 'A';
    else if (num[i] >= 26 && num[i] <= 51)
      c[i] = num[i] - 26 + 'a';
    else
      c[i] = ' ';
  }
  return c;
}

int euclides(mpz_t a, mpz_t b)
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

  int encode;

  char *code = NULL;

  int ch, idx = 0;

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
  }
  else
  {
    if (!(code = malloc(TAM * sizeof(char))))
    {
      fprintf(stderr, "Error al reservar memoria\n");
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
      }
      else
      {
        while ((ch = fgetc(entrada)) != EOF && idx < TAM - 1)
        {
          code[idx++] = number_to_char(euclides_extendido(char_to_number(ch), a, b, m));
        }
        code[idx] = '\0';
      }

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

  return (0);
}
