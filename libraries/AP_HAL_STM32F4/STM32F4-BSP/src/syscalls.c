/*
 * syscalls.c
 *
 *  Created on: 2 Nov 2010
 *     Author: nanoage.co.uk
 *  Changed on: 2012-05-27
 *     Author: Christian Krueger
 *     Changes: Directed _write() output to __io_putchar()
 */
#include <errno.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <sys/unistd.h>
#include "stm32f4xx.h"
#include "stlinky.h"
#include <string.h>

#undef errno
extern int errno;

/*
   environ
   A pointer to a list of environment variables and their values.
   For a minimal environment, this empty list is adequate:
 */
char *__env[1] = { 0 };
char **environ = __env;

int _write(int file, char *ptr, int len);

volatile struct stlinky g_stlinky_term = {
		.magic = STLINKY_MAGIC,
		.bufsize = CONFIG_LIB_STLINKY_BSIZE,
		.txsize = 0,
		.rxsize = 0
};

#pragma weak __io_putchar /**< Defines following __io_putchar() as weak, it will only be used, if there is not other implementation (e.g. lcd-log) */

/**
 * @brief  Retargets the C library printf function to the USART.
 * @param  None
 * @retval None
 */
int __io_putchar(int ch)
{
		return ITM_SendChar(ch);
}



void _exit(int status)
{
		_write(1, "exit", 4);
		while (1)
		{
				;
		}
}

int _close(int file)
{
		return -1;
}
/*
   execve
   Transfer control to a new process. Minimal implementation (for a system without processes):
 */

int _execve(char *name, char **argv, char **env)
{
		errno = ENOMEM;
		return -1;
}
/*
   fork
   Create a new process. Minimal implementation (for a system without processes):
 */

int _fork()
{
		errno = EAGAIN;
		return -1;
}
/*
   fstat
   Status of an open file. For consistency with other minimal implementations in these examples,
   all files are regarded as character special devices.
   The `sys/stat.h' header file required is distributed in the `include' subdirectory for this C library.
 */
int _fstat(int file, struct stat *st)
{
		st->st_mode = S_IFCHR;
		return 0;
}

/*
   getpid
   Process-ID; this is sometimes used to generate strings unlikely to conflict with other processes. Minimal implementation, for a system without processes:
 */

int _getpid()
{
		return 1;
}

/*
   isatty
   Query whether output stream is a terminal. For consistency with the other minimal implementations,
 */
int _isatty(int file)
{


		switch (file)
		{
				case STDOUT_FILENO:
				case STDERR_FILENO:
				case STDIN_FILENO:
						return 1;
				default:
						//errno = ENOTTY;
						errno = EBADF;
						return 0;
		}
}

/*
   kill
   Send a signal. Minimal implementation:
 */
int _kill(int pid, int sig)
{
		errno = EINVAL;
		return (-1);
}

/*
   link
   Establish a new name for an existing file. Minimal implementation:
 */
int _link(char *old, char *new)
{
		errno = EMLINK;
		return -1;
}

/*
   lseek
   Set position in a file. Minimal implementation:
 */
int _lseek(int file, int ptr, int dir)
{
		return 0;
}


/*
   stat
   Status of a file (by name). Minimal implementation:
   int    _EXFUN(stat,( const char *__path, struct stat *__sbuf ));
 */
int _stat(const char *filepath, struct stat *st)
{
		st->st_mode = S_IFCHR;
		return 0;
}

/*
   times
   Timing information for current process. Minimal implementation:
 */
clock_t _times(struct tms *buf)
{
		return -1;
}

/*
   unlink
   Remove a file's directory entry. Minimal implementation:
 */
int _unlink(char *name)
{
		errno = ENOENT;
		return -1;
}

/*
   wait
   Wait for a child process. Minimal implementation:
 */
int _wait(int *status)
{
		errno = ECHILD;
		return -1;
}

int stlinky_tx(volatile struct stlinky* st, const char* buf, int siz)
{
		int sz = min_t(int, CONFIG_LIB_STLINKY_BSIZE, siz);
		while(st->txsize != 0);;; 
		memcpy((char*) st->txbuf, buf, sz);
		st->txsize = (unsigned char) sz;
		return sz;
}

/* TODO: We loose data here if we read less than avaliable */ 
int stlinky_rx(volatile struct stlinky* st, char* buf, int siz)
{
		int ret;
		while(st->rxsize == 0);;; 
		int sz = min_t(int, (int) st->rxsize, siz);
		memcpy(buf, (char*) st->rxbuf, sz);
		ret = (int) (st->rxsize);
		st->rxsize = 0;
		return ret;
}

int stlinky_avail(volatile struct stlinky* st)
{
		return st->rxsize;
}

void stlinky_wait_for_terminal(volatile struct stlinky* st)
{
		st->txbuf[0]='\n';
		st->txsize=1;
		while(st->txsize != 0);;; 
}

#ifdef CONFIG_LIB_STLINKY_NLIB

int _write(int file, char *ptr, int len) {
		return stlinky_tx(&g_stlinky_term, ptr, len);
}

int _read(int file, char *ptr, int len) {
		return stlinky_rx(&g_stlinky_term, ptr, len);
}

#else
/*
   write
   Write a character to a file. `libc' subroutines will use this system routine for output to all files, including stdout
   Returns -1 on error or number of bytes sent
 */
int _write(int file, char *ptr, int len)
{
		int n;

		switch (file)
		{
				case STDOUT_FILENO: /*stdout*/
						for (n = 0; n < len; n++)
						{
								__io_putchar((*ptr++));
						}
						break;
				case STDERR_FILENO: /* stderr */
						for (n = 0; n < len; n++)
						{
								__io_putchar((*ptr++));
						}
						break;
				default:
						errno = EBADF;
						return -1;
		}
		return len;
}

/*
   read
   Read a character to a file. `libc' subroutines will use this system routine for input from all files, including stdin
   Returns -1 on error or blocks until the number of characters have been read.
 */
int _read(int file, char *ptr, int len)
{
		int num = 0;
		switch (file)
		{
				case STDIN_FILENO:
						break;
				default:
						errno = EBADF;
						return -1;
		}
		return num;
}

#endif
