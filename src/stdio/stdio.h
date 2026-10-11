#pragma once
#include "stdlib.h"
#include "string.h"
#include <stdarg.h>
// #include "usart_public.h"

typedef struct
{
	int fd;
} FILE;

#ifndef STDIN_FILENO
#	define STDIN_FILENO 0
#endif
#ifndef STDOUT_FILENO
#	define STDOUT_FILENO 1
#endif
#ifndef STDERROR_FILENO
#	define STDERROR_FILENO 2
#endif

#ifdef USE_LIBC
#	error "If using some libc, then the FILE *definition will cause issues"
#endif

extern FILE *stdin;
extern FILE *stdout;
extern FILE *stderr;

int putchar(int c);
int puts(const char *s);
int fputs(const char *restrict s, FILE *restrict stream);

void perror(const char *s);

int vprintf(const char *restrict format, va_list args) __attribute__((format(printf, 1, 0)));
int printf(const char *restrict format, ...) __attribute__((format(printf, 1, 2)));

int vfprintf(FILE *stream, const char *restrict format, va_list args) __attribute__((format(printf, 2, 0)));
int fprintf(FILE *restrict stream, const char *restrict format, ...) __attribute__((format(printf, 2, 3)));

int vsprintf(char *restrict str, const char *restrict format, va_list args) __attribute__((format(printf, 2, 0)));
int sprintf(char *restrict str, const char *restrict format, ...) __attribute__((format(printf, 2, 3)));

int vsnprintf(char *restrict str, size_t size, const char *restrict format, va_list args) __attribute__((format(printf, 3, 0)));
int snprintf(char *restrict str, size_t size, const char *restrict format, ...) __attribute__((format(printf, 3, 4)));
