#include "string.h"

#ifndef USE_LIBC

void *memset(void *s, int c, size_t n)
{
	unsigned char *p = s;
	for (size_t i = 0; i < n; i++)
	{
		p[i] = (unsigned char)c;
	}

	return s;
}
void *memcpy(void *restrict dest, const void *restrict src, size_t n)
{
	unsigned char		*d = dest;
	const unsigned char *s = src;

	for (size_t i = 0; i < n; i++)
	{
		d[i] = s[i];
	}

	return dest;
}
size_t strlen(const char *s)
{
	for (size_t i = 0;; i++)
	{
		if (s[i] == '\0')
		{
			return i;
		}
	}
}
size_t strnlen(const char *s, size_t maxlen)
{

	for (size_t i = 0; i < maxlen; i++)
	{
		if (s[i] == '\0')
		{
			return i;
		}
	}
	return maxlen;
}

char *strcpy(char *restrict dest, const char *restrict src)
{
	memcpy(dest, src, strlen(src) + 1);
	return dest;
}
char *strncpy(char *restrict dest, const char *restrict src, size_t dsize)
{

	size_t len = strnlen(src, dsize);
	memcpy(dest, src, len);
	if (len < dsize)
	{

		memset(dest + len, '\0', dsize - len);
	}
	return dest;
}

/*
Return the number of byte written, including the null terminator;
Custom function
*/
size_t strwrite(char *restrict dest, const char *restrict src)
{
	size_t len = strlen(src);
	memcpy(dest, src, len + 1);
	return len + 1;
}

#endif
