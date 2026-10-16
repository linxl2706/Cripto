#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define TAM 4096

int char_to_number(int c);
char number_to_char(int num);   
int mcd(int a, int b);
int euclides_extendido(int a, int b);
void clear(FILE *entrada, FILE *salida, char *code, char *let);

#endif