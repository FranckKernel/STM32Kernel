#include <errno.h>
#include <reent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

extern char __heap_start;
extern char __stack_start;

static char *heap_end = &__heap_start;

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

void *_sbrk_r(struct _reent *r, ptrdiff_t incr)
{
	(void)r; // unused in a single‑threaded environment
	return _sbrk(incr);
}

int _write(int file, char *ptr, int len)
{
	(void)file;
	(void)ptr;

	/* No console/UART wired up yet — pretend we wrote it all. */
	return len;
}

// Reentrant version (used by printf)
ssize_t _write_r(struct _reent *r, int fd, const void *buf, size_t count)
{
	(void)r;
	return _write(fd, buf, count);
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
