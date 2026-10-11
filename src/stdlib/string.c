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

#endif
