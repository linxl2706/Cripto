#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#include <ctype.h>

#include <gmp.h>

#include "utils.h"

char convert(char c) {
    unsigned char uc = (unsigned char)c;

    // Using Latin-1 hex values to avoid multichar warnings and handle Spanish accents
    if (uc == 0xE1 || uc == 0xC1) return 'a'; // á, Á
    if (uc == 0xE9 || uc == 0xC9) return 'e'; // é, É
    if (uc == 0xED || uc == 0xCD) return 'i'; // í, Í
    if (uc == 0xF3 || uc == 0xD3) return 'o'; // ó, Ó
    if (uc == 0xFA || uc == 0xFC || uc == 0xDA) return 'u'; // ú, ü, Ú
    if (uc == 0xF1 || uc == 0xD1) return 'n'; // ñ, Ñ
    
    return c;
}

int char_to_number(int c) {
    char ch = convert(c);
    if (isupper(ch)) return c - 'A';
    if (islower(ch)) return c - 'a';
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

int euclides_extendido(int m, int a){
    int r0 = m;
    int r1 = a;
    int q;
    int mod;
    int v_i, v0 = 0, v1 = 1;
    int u_i, u0 = 1, u1 = 0;

     while (r1 != 0){
        mod = r0 % r1;
        q = r0 / r1;
        r0 = r1;
        r1 = mod;
        
        u_i = u0 - q * u1;
        v_i = v0 - q * v1;
        u0 = u1;
        u1 = u_i;
        v0 = v1;
        v1 = v_i;
    }

    if (v0 < 0) v0 += m;
    
    return v0;
}