#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

void  *memset(void *s, int c, size_t n);
void  *memcpy(void *restrict dest, const void *restrict src, size_t n);
char  *strcpy(char *restrict dest, const char *restrict src);
char  *strncpy(char *restrict dest, const char *restrict src, size_t dsize);
size_t strlen(const char *s);
size_t strnlen(const char *s, size_t maxlen);

size_t strwrite(char *restrict dest, const char *restrict src);
