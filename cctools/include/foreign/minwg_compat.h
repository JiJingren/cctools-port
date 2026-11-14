#ifndef __CCTOOLS_MINGW_COMPAT_H_
#define __CCTOOLS_MINGW_COMPAT_H_

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include <process.h>
#include <direct.h>
#include <winsock2.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>
#ifdef interface
#undef interface
#endif
/* native Windows CRT defaults to text mode; Mach-O tools need binary I/O */
static void __attribute__((constructor)) cct_force_binary_fmode(void) { _fmode = _O_BINARY; }

#include <ws2tcpip.h>

/* mingw UCRT: the libdispatch os/generic_win_base.h redefines _mode_t as int
   while mingw sys/types.h has unsigned short.  Do not let our foreign
   sys/_types.h redefine Darwin pthread structs / mode_t on mingw at all:
   the mingw pthread.h + os headers already provide them. */
#define __CCT_SKIP_DARWIN_TYPES 1

typedef uint8_t  u_int8_t;
typedef uint16_t u_int16_t;
typedef uint32_t u_int32_t;
typedef uint64_t u_int64_t;
typedef unsigned char  u_char;
typedef unsigned short u_short;
typedef unsigned int   u_int;
typedef unsigned long  u_long;
typedef char *caddr_t;
typedef int uid_t;
typedef int gid_t;

#ifndef S_ISUID
#define S_ISUID 04000
#endif
#ifndef S_ISGID
#define S_ISGID 02000
#endif
#ifndef S_ISVTX
#define S_ISVTX 01000
#endif
#ifndef S_IFLNK
#define S_IFLNK 0120000
#endif
#ifndef S_IFSOCK
#define S_IFSOCK 0140000
#endif
#ifndef S_ISLNK
#define S_ISLNK(m) (((m) & S_IFMT) == S_IFLNK)
#endif
#ifndef S_ISREG
#define S_ISREG(m) (((m) & S_IFMT) == S_IFREG)
#endif
#ifndef S_ISDIR
#define S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)
#endif

#ifndef O_SYNC
#define O_SYNC 0
#endif
#ifndef O_NDELAY
#define O_NDELAY _O_NDELAY
#endif

#define lstat stat
static __inline int cct_getpagesize(void) { return 4096; }
#define getpagesize cct_getpagesize
#define localtime_r(t, r) (localtime_s((r), (t)) == 0 ? (r) : NULL)
typedef struct { char *buf; size_t len; size_t cap; int err; } cct_memstream_t;
static __inline FILE *cct_open_memstream(char **ptr, size_t *sizeloc)
{
	char *fname = _tempnam(NULL, "cctomm");
	FILE *f;
	if (!fname)
		return NULL;
	f = fopen(fname, "w+b");
	free(fname);
	if (!f)
		return NULL;
	if (ptr) *ptr = NULL;
	if (sizeloc) *sizeloc = 0;
	return f;
}
#define open_memstream cct_open_memstream
/* mkstemp: MSYS-style /tmp templates must be redirected to a real Windows
   temp directory for native binaries */
static __typeof__(&mkstemp) cct_real_mkstemp = mkstemp;
static __inline char *cct_tmp_fix(const char *tmpl, char *out, size_t outsz)
{
	char base[1024];
	size_t i, j = 0;
	const char *rest;
	if (strncmp(tmpl, "/tmp/", 5) == 0)
		rest = tmpl + 5;
	else if (strcmp(tmpl, "/tmp") == 0)
		rest = "";
	else
		return NULL;
	if (GetTempPathA((DWORD)sizeof(base), base) == 0)
		return NULL;
	for (i = 0; base[i] && j + 1 < outsz; i++)
		out[j++] = base[i];
	if (j > 0 && out[j - 1] != '\\' && out[j - 1] != '/' && j + 1 < outsz)
		out[j++] = '\\';
	for (i = 0; rest[i] && j + 1 < outsz; i++)
		out[j++] = rest[i];
	out[j] = 0;
	return out;
}
static __inline int cct_mkstemp(char *tmpl)
{
	char buf[1024];
	char *p = cct_tmp_fix(tmpl, buf, sizeof(buf));
	if (p != NULL)
		strcpy(tmpl, p);
	return cct_real_mkstemp(tmpl);
}
#define mkstemp cct_mkstemp

#define bzero(p, n) memset((p), 0, (n))
#define bcopy(s, d, n) memmove((d), (s), (n))

static __inline int cct_asprintf(char **out, const char *fmt, ...)
{
	va_list ap;
	int n;
	char *buf;

	va_start(ap, fmt);
	n = _vscprintf(fmt, ap);
	va_end(ap);
	if (n < 0)
		return -1;
	buf = (char *)malloc((size_t)n + 1);
	if (buf == NULL)
		return -1;
	va_start(ap, fmt);
	vsnprintf(buf, (size_t)n + 1, fmt, ap);
	va_end(ap);
	*out = buf;
	return n;
}

static __inline int cct_vasprintf(char **out, const char *fmt, va_list ap)
{
	va_list ap2;
	int n;
	char *buf;

	va_copy(ap2, ap);
	n = _vscprintf(fmt, ap2);
	va_end(ap2);
	if (n < 0)
		return -1;
	buf = (char *)malloc((size_t)n + 1);
	if (buf == NULL)
		return -1;
	vsnprintf(buf, (size_t)n + 1, fmt, ap);
	*out = buf;
	return n;
}

#define asprintf cct_asprintf
#define vasprintf cct_vasprintf

static __inline char *cct_realpath(const char *path, char *resolved)
{
	return _fullpath(resolved, path, _MAX_PATH);
}

#define realpath cct_realpath

static __inline ssize_t cct_readlink(const char *path, char *buf, size_t bufsiz)
{
	DWORD n;
	(void)path;
	if (bufsiz == 0)
		return -1;
	n = GetModuleFileNameA(NULL, buf, (DWORD)bufsiz);
	if (n == 0 || n >= (DWORD)bufsiz)
		return -1;
	return (ssize_t)n;
}

#define readlink cct_readlink

static __inline uid_t cct_getuid(void) { return 0; }
static __inline gid_t cct_getgid(void) { return 0; }
static __inline uid_t cct_geteuid(void) { return 0; }
static __inline gid_t cct_getegid(void) { return 0; }
#define getuid cct_getuid
#define getgid cct_getgid
#define geteuid cct_geteuid
#define getegid cct_getegid

#ifndef NAME_MAX
#define NAME_MAX 255
#endif
#ifndef PATH_MAX
#define PATH_MAX 260
#endif

#ifndef MAXPATHLEN
#define MAXPATHLEN PATH_MAX
#endif
#ifndef MAXNAMLEN
#define MAXNAMLEN NAME_MAX
#endif
#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif
#ifndef MAX
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef S_IRUSR
#define S_IRUSR 00400
#endif
#ifndef S_IWUSR
#define S_IWUSR 00200
#endif
#ifndef S_IXUSR
#define S_IXUSR 00100
#endif
#ifndef S_IRGRP
#define S_IRGRP 00040
#endif
#ifndef S_IWGRP
#define S_IWGRP 00020
#endif
#ifndef S_IXGRP
#define S_IXGRP 00010
#endif
#ifndef S_IROTH
#define S_IROTH 00004
#endif
#ifndef S_IWOTH
#define S_IWOTH 00002
#endif
#ifndef S_IXOTH
#define S_IXOTH 00001
#endif
#ifndef S_ISSOCK
#define S_ISSOCK(m) 0
#endif

#ifndef LOCK_SH
#define LOCK_SH 1
#endif
#ifndef LOCK_EX
#define LOCK_EX 2
#endif
#ifndef LOCK_NB
#define LOCK_NB 4
#endif
#ifndef LOCK_UN
#define LOCK_UN 8
#endif
static __inline int cct_flock(int fd, int op) { (void)fd; (void)op; return 0; }
#define flock cct_flock

#ifndef ESTALE
#define ESTALE 116
#endif


static __inline void *cct_getpwnam(const char *name)
{
	(void)name;
	return NULL;
}
#define getpwnam cct_getpwnam

struct group;
static __inline struct group *cct_getgrnam(const char *name)
{
	(void)name;
	return NULL;
}
#define getgrnam cct_getgrnam

#ifdef ERROR
#undef ERROR
#endif
#ifdef WARNING
#undef WARNING
#endif
#ifdef FATAL
#undef FATAL
#endif

/* ---- process / filesystem stubs (no fork on Windows) ---- */
#ifndef SIGPIPE
#define SIGPIPE 13
#endif
#ifndef SIGCHLD
#define SIGCHLD 17
#endif
#define WIFEXITED(s)   1
#define WEXITSTATUS(s) ((s) & 0xff)
#define WIFSIGNALED(s) 0
#define WTERMSIG(s)    0

#define HAVE_OPENMEMSTREAM_RUNTIME 0

#ifndef O_FSYNC
#define O_FSYNC 0
#endif
#ifndef O_DSYNC
#define O_DSYNC 0
#endif
static __inline int cct_chown(const char *p, int u, int g) { (void)p; (void)u; (void)g; return 0; }
#define chown cct_chown

static __inline int cct_rename(const char *from, const char *to)
{
	if (MoveFileExA(from, to, MOVEFILE_REPLACE_EXISTING | MOVEFILE_COPY_ALLOWED))
		return 0;
	errno = EACCES;
	return -1;
}
#define rename cct_rename

typedef int sigset_t;
#ifndef SIG_BLOCK
#define SIG_BLOCK 0
#endif
#ifndef SIG_SETMASK
#define SIG_SETMASK 0
#endif
static __inline int cct_sigfillset(sigset_t *s) { if (s) *s = 0; return 0; }
static __inline int cct_sigprocmask(int how, const sigset_t *set, sigset_t *old) { (void)how; (void)set; if (old) *old = 0; return 0; }
#define sigfillset cct_sigfillset
#define sigprocmask cct_sigprocmask

static __inline int cct_fchdir(int fd) { (void)fd; return 0; }
#define fchdir cct_fchdir
static __inline int cct_fork(void) { return -1; }
static __inline int cct_wait(int *status) { if (status) *status = 0; return -1; }
static __inline int cct_waitpid(int pid, int *status, int options)
{ (void)pid; (void)options; if (status) *status = 0; return -1; }
#define fork cct_fork
#define wait cct_wait
#define waitpid cct_waitpid

#ifndef _SC_PAGESIZE
#define _SC_PAGESIZE 30
#endif
static __inline long cct_sysconf(int name) { (void)name; return 4096; }
#define sysconf cct_sysconf

static __inline int cct_pread(int fd, void *buf, unsigned int n, long long off)
{ (void)fd; (void)buf; (void)n; (void)off; return -1; }
#define pread cct_pread

static __inline int cct_utimes(const char *p, const void *t) { (void)p; (void)t; return 0; }
#define utimes cct_utimes

static __inline int cct_fchmod(int fd, int mode) { (void)fd; (void)mode; return 0; }
#define fchmod cct_fchmod

#ifndef S_ISCHR
#define S_ISCHR(m) 0
#endif
#ifndef S_ISBLK
#define S_ISBLK(m) 0
#endif
#ifndef S_ISFIFO
#define S_ISFIFO(m) 0
#endif


/* writable private file mappings: use a Win32 copy-on-write view (PAGE_WRITECOPY)
   so mapping and unmapping may happen from different translation units */
#include <sys/mman.h>
static __typeof__(&mmap) cct_real_mmap_fn = mmap;
static __typeof__(&munmap) cct_real_munmap_fn = munmap;
static __inline void *cct_mmap(void *addr, size_t len, int prot, int flags, int fd, long long off)
{
    HANDLE fh, sec;
    void *p;
    (void)addr;
    if (!(fd >= 0 && (prot & PROT_WRITE) && !(flags & MAP_SHARED)))
        return cct_real_mmap_fn(addr, len, prot, flags, fd, off);
    fh = (HANDLE)_get_osfhandle(fd);
    if (fh == INVALID_HANDLE_VALUE) { errno = EBADF; return (void *)-1; }
    sec = CreateFileMappingA(fh, NULL, PAGE_WRITECOPY, 0, 0, NULL);
    if (sec == NULL) { errno = EACCES; return (void *)-1; }
    p = MapViewOfFile(sec, FILE_MAP_COPY,
                      (DWORD)((unsigned long long)off >> 32),
                      (DWORD)((unsigned long long)off & 0xFFFFFFFFu), len);
    CloseHandle(sec);
    if (p == NULL) { errno = EACCES; return (void *)-1; }
    return p;
}
static __inline int cct_munmap(void *addr, size_t len)
{
    FlushViewOfFile(addr, 0);
    if (UnmapViewOfFile(addr))
        return 0;
    return cct_real_munmap_fn(addr, len);
}
#define mmap cct_mmap
#define munmap cct_munmap

/* 2-arg mkdir (posix) -> _mkdir */
static __inline int cct_mkdir2(const char *p, int mode) { (void)mode; return _mkdir(p); }
#define mkdir(p, m) cct_mkdir2((p), (m))


/* prevents modern-mingw pthread/mode conflicts with darwin headers */
#ifndef strndup
static __inline char *cct_strndup(const char *s, size_t n)
{
	size_t l = 0; char *r;
	while (l < n && s[l]) l++;
	r = (char *)malloc(l + 1);
	if (!r) return NULL;
	memcpy(r, s, l); r[l] = 0;
	return r;
}
#define strndup cct_strndup
#endif

#ifndef HAVE_PWRITE_STUB
#define HAVE_PWRITE_STUB
static __inline long cct_pwrite(int fd, const void *b, unsigned int n, long o) {
    long save = _lseek(fd, 0, 1); long r;
    if (_lseek(fd, o, 0) < 0) return -1;
    r = _write(fd, b, n);
    _lseek(fd, save, 0);
    return r;
}
#define pwrite cct_pwrite
#endif
#define __assert(cond, file, line) _assert((cond), (file), (line))

#endif
