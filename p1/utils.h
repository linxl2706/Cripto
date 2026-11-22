#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define TAM 65536

int char_to_number(int c);
char number_to_char(int num);   
int mcd(int a, int b);
int euclides_extendido(int m, int a);

#endif