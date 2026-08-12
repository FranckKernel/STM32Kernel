#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

extern char _end;

static char *heap_end = &_end;

void *_sbrk(int incr)
{
	return NULL;
	char *prev_heap_end = heap_end;

	/* No bound check against stack here — add one if you care
	 * about heap/stack collision detection. */
	heap_end += incr;

	return (void *)prev_heap_end;
}

int _write(int file, char *ptr, int len)
{
	(void)file;
	(void)ptr;

	/* No console/UART wired up yet — pretend we wrote it all. */
	return len;
}

int _read(int file, char *ptr, int len)
{
	(void)file;
	(void)ptr;
	(void)len;

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

	errno = EINVAL;
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
