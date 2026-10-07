#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#include <ctype.h>

#include <gmp.h>

#include "utils.h"

int char_to_number(int c) {
    if (isupper(c)) return c - 'A';
    if (islower(c)) return c - 'a';
    return -1;
}

char number_to_char(int num) {
    if (num >= 0 && num <= 25) return num + 'a';
    return '\0';
}

int mcd(int a, int b)
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