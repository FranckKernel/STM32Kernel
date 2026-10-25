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
	char buffer[1000];
	int	 retb = vsnprintf(buffer, sizeof(buffer), format, args);
	if (retb < 0)
		return -1;

	// vsnprintf returns the number of characters that would have
	// been produced, excluding the terminating NUL.
	if ((size_t)retb >= sizeof(buffer))
	{
		// Output was truncated. Handle this properly.
		// A single fixed buffer cannot support arbitrary output.
		return -1;
	}

	int retp = fputs(buffer, stream);
	if (retp < 0)
		return -1;

	return retb;
}

// =================  Approaching hell but not quite right there
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

// ================= Need to implement vsnprintf and vsprintf bellow!

int vsprintf(char *restrict str, const char *restrict format, va_list args)
{
	return 0;
}

int vsnprintf(char *restrict str, size_t size, const char *restrict format, va_list args)
{

	return -1;
}
