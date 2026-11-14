#ifndef _CCT_ERR_H_
#define _CCT_ERR_H_
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
static __inline void cct_vwarn(const char *pre, const char *fmt, va_list ap)
{
    if (pre) fprintf(stderr, "%s: ", pre);
    vfprintf(stderr, fmt, ap);
    fputc('\n', stderr);
}
static __inline void cct_err(int e, const char *fmt, ...)
{
    va_list ap; va_start(ap, fmt); cct_vwarn(NULL, fmt, ap); va_end(ap);
    exit(e);
}
static __inline void cct_errx(int e, const char *fmt, ...)
{
    va_list ap; va_start(ap, fmt); cct_vwarn(NULL, fmt, ap); va_end(ap);
    exit(e);
}
static __inline void cct_warn(const char *fmt, ...)
{
    va_list ap; va_start(ap, fmt); cct_vwarn(NULL, fmt, ap); va_end(ap);
}
static __inline void cct_warnx(const char *fmt, ...)
{
    va_list ap; va_start(ap, fmt); cct_vwarn(NULL, fmt, ap); va_end(ap);
}
#define err cct_err
#define errx cct_errx
#define warn cct_warn
#define warnx cct_warnx
#endif
