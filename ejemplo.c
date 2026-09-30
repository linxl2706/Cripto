#include <stdio.h>

#include <stdlib.h>

#include <gmp.h>

/*#include "gmp.h"*/

int euclides(int a, int b) {
  int r0 = a, r1 = b, q, r2;
  if (a==0 && b==0) {
    return -1;
  }
  if(b > a) {
    r0 = b;
    r1 = a;
  }
  r2 = r0 % r1;
  while(r2 != 0) {
    r0 = r1;
    r1 = r2;
    r2 = r0 % r1;
  }
  return r1;
}

int* euclide

int euclides_extendido(int a, int b) {
  int r0 = a, r1 = b, q, r2;
  if (a==0 && b==0) {
    return -1;
  }
  if(b > a) {
    r0 = b;
    r1 = a;
  }
  r2 = r0 % r1;
  while(r2 != 0) {
    r0 = r1;
    r1 = r2;
    r2 = r0 % r1;
  }
  return r1;
}



/* PROGRAMA PRINCIPAL */

int main (int argc,char *argv[]) {

  mpz_t a,b,m;

  int modo;

  FILE *entrada,*salida;


  mpz_init (a);

  mpz_init (b);

  mpz_init (m);



  mpz_set_str (a,"123452345234523452352352345112341234213",10);

  mpz_set_str (b,"234562344341234123421341234441234213421",10);



  mpz_add    (m,a,b);



  gmp_printf ("El resultado de la suma es %Zd\n", m);



  mpz_clear (a);

  mpz_clear (b);

  mpz_clear (m);

  int s = euclides(8*4*23*34*2, 18*342*2*34*4);
  printf("%d\n",s);

  return(0);

}

