#include "stdio.h"
#include "syscall.h"
#include <stdarg.h>

static FILE stdin_stream  = {.fd = 0};
static FILE stdout_stream = {.fd = 1};
static FILE stderr_stream = {.fd = 2};

FILE *stdin	 = &stdin_stream;
FILE *stdout = &stdout_stream;
FILE *stderr = &stderr_stream;

int write_stdout_n(char *ptr, int len)
{
	return _write(STDOUT_FILENO, ptr, len);
}

int puts(const char *ptr)
{
	// ignore warning, this is needed for newlib
	return _write(STDOUT_FILENO, ptr, strlen(ptr));
}

int putchar(int c)
{
	char ch = (char)c;
	// ignore warning, this is needed for newlib
	return _write(STDOUT_FILENO, &ch, 1);
}

int fputs(const char *ptr, FILE *restrict stream)
{
	return _write(stream->fd, ptr, strlen(ptr));
}

void perror(const char *s)
{
	fputs(s, stderr);
}

// ============================== Going into the printfs

int printf(const char *restrict format, ...)
{
	va_list args;
	va_start(args, format);

	int result = vfprintf(stdout, format, args);

	va_end(args);
	return result;
}

int vprintf(const char *restrict format, va_list args)
{
	return vfprintf(stdout, format, args);
}

int fprintf(FILE *restrict stream, const char *restrict format, ...)
{
	va_list args;
	va_start(args, format);

	int result = vfprintf(stream, format, args);

	va_end(args);
	return result;
}

int vfprintf(FILE *stream, const char *restrict format, va_list args)
{
	// Temporary. Must implement Vsnprintf
	char buffer[1000];
	int	 retb = vsnprintf(buffer, 1000, format, args);

	int retp = fputs(buffer, stream);

	return -1;
}

int sprintf(char *restrict str, const char *restrict format, ...)
{
	va_list args;
	va_start(args, format);

	int result = vsprintf(str, format, args);

	va_end(args);
	return result;
}
int snprintf(char *restrict str, size_t size, const char *restrict format, ...)
{
	va_list args;
	va_start(args, format);

	int result = vsnprintf(str, size, format, args);

	va_end(args);
	return result;
}
