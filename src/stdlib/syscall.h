#include <sys/stat.h>

void *_sbrk(int incr);

int _write(int file, char *ptr, int len);

int _read(int file, char *ptr, int len);

int _close(int file);

int _lseek(int file, int ptr, int dir);

int _fstat(int file, struct stat *st);

int _isatty(int file);

int _kill(int pid, int sig);

int _getpid(void);

void _exit(int status);
