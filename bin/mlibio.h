#ifndef MLIBIO_H
#define MLIBIO_H

#include "sheaders.h"

void clear(void);
void print(const char* text);
int printf(const char* fmt, ...);

void putchar(char c);
char getchar(void);
void puts(const char* s);

size_t strlen(const char* s);
int strcmp(const char* s1, const char* s2);
int strncmp(const char* s1, const char* s2, size_t n);
char* strcpy(char* dest, const char* src);
char* strncpy(char* dest, const char* src, size_t n);
char* strcat(char* dest, const char* src);
char* strchr(const char* s, int c);
char* strstr(const char* haystack, const char* needle);
char* strtok(char* str, const char* delim);

void* memset(void* s, int c, size_t n);
void* memcpy(void* dest, const void* src, size_t n);
void* memmove(void* dest, const void* src, size_t n);
int memcmp(const void* s1, const void* s2, size_t n);

void* malloc(size_t size);
void free(void* ptr);
void* calloc(size_t num, size_t size);
void* realloc(void* ptr, size_t new_size);

#endif