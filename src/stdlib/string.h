#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

void  *memset(void *s, int c, size_t n);
void  *memcpy(void *restrict dest, const void *restrict src, size_t n);
size_t strlen(const char *s);
size_t strnlen(const char *s, size_t maxlen);
