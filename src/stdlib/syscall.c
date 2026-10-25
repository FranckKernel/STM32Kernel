#include "usart.h"
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

extern char __heap_start;
extern char __stack_start;

static char *heap_end = &__heap_start;

#ifndef STDIN_FILENO
#	define STDIN_FILENO 0
#endif
#ifndef STDOUT_FILENO
#	define STDOUT_FILENO 1
#endif
#ifndef STDERROR_FILENO
#	define STDERROR_FILENO 2
#endif

void *_sbrk(int incr)
{
	char *prev_heap_end = heap_end;

	/* No bound check against stack here — add one if you care
	 * about heap/stack collision detection. */
	heap_end += incr;
	if (heap_end >= &__stack_start)
	{
		heap_end -= incr;
		return NULL;
	}

	return (void *)prev_heap_end;
}

// void *_sbrk_r(struct _reent *r, ptrdiff_t incr)
// {
// 	(void)r; // unused in a single‑threaded environment
// 	return _sbrk(incr);
// }

int _write(int file, char *ptr, int len)
{
	if (len < 0)
	{
		return -1;
	}

	switch (file)
	{
	case (STDOUT_FILENO):
	{
		return usart_write_n(usart2, ptr, len);
	}
	case (STDERROR_FILENO):
	{
		// could use usart1 for std err.
		// But anyway... We can imagine how
		// A file nbr can represent where to right
		int a = usart_write(usart2, "\x1b[31m"); // Red
		if (a != 5)
		{
			return a;
		}
		int b = usart_write_n(usart2, ptr, len);
		if (b != len)
		{
			return a + b;
		}
		int c = usart_write(usart2, "\x1b[0m"); // Reset color and newline
		return a + b + c;
	}
	default: return -1;
	}

	/* No console/UART wired up yet — pretend we wrote it all. */
}

// Reentrant version (used by printf)
// ssize_t _write_r(struct _reent *r, int fd, const void *buf, size_t count)
// {
// 	(void)r;
// 	return _write(fd, buf, count);
// }

int _read(int file, char *ptr, int len)
{
	(void)file;
	(void)ptr;
	(void)len;

	switch (file)
	{
	case (STDIN_FILENO):
	{
		// Maybe something to do with usart reciever. But it's currently
		// using an interrupt...
		break;
	}
	}

	return 0; /* EOF */
}

int _close(int file)
{
	(void)file;

	return -1;
}

int _lseek(int file, int ptr, int dir)
{
	(void)file;
	(void)ptr;
	(void)dir;

	return 0;
}

int _fstat(int file, struct stat *st)
{
	(void)file;

	st->st_mode = S_IFCHR;
	return 0;
}

int _isatty(int file)
{
	(void)file;

	return 1;
}

int _kill(int pid, int sig)
{
	(void)pid;
	(void)sig;

	// errno = EINVAL;
	return -1;
}

int _getpid(void)
{
	return 1;
}

void _exit(int status)
{
	(void)status;

	while (1)
	{
		/* Halt forever — nowhere for a bare-metal kernel to
		 * "return" to. Swap for a breakpoint/reset if you'd
		 * rather trap here in a debugger. */
	}
}
