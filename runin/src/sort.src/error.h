#ifndef _error_h_
#define _error_h_
#include <stdarg.h>
#include <errno.h>

/* Fatal error unrelated to system call
 * Print message and terminate */

void
err_quit(const char *fmt, ...);

/* Print message and return to caller
 * Caller specifies "errnoflag" and "level" */

static void
err_doit(int errnoflag, int level, const char *fmt, va_list ap);

/* Fatal error related to system call
 * Print message and terminate */

void
err_sys(const char *fmt, ...);

#endif
