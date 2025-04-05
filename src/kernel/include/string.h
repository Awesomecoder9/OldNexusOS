#pragma once
#ifndef STRING_H
#define STRING_H

#include "stdint.h"

void strcpy(char* src, char* dest);
char* strcat(char* src, char* dest);
char* strchr(char* str, char c);
bool strcmp(char* str1, char* str2);
char* strrev(char* str);
size_t strlen(char* src);

#endif