#ifndef STRING_H
#define STRING_H
#include <stdbool.h>

int strlen(const char *str);
bool is_digit(char c);
int to_numeric_digit(char c);
int strnlen(const char *str, int max);
char *strcpy(char *dest, const char *src);
char tolower(char c);
int istrncmp(const char *s1, const char *s2, int n);
int strncmp(const char *str1, const char *str2, int n);
int strnlen_terminator(const char *str, int max, char terminator);
int cmpstring(const char *str1, const char *str2);
int number_to_digit(char *n);
char *digit_to_number(int n);

#endif