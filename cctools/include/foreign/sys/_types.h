#ifndef _SYS__TYPES_H_
#define _SYS__TYPES_H_
#include <stdint.h>
#include <stddef.h>
#include <machine/types.h>
#include <sys/cdefs.h>
#define __PTHREAD_SIZE__ 1168
#define __PTHREAD_ATTR_SIZE__ 56
#define __PTHREAD_MUTEXATTR_SIZE__ 8
#define __PTHREAD_MUTEX_SIZE__ 56
#define __PTHREAD_CONDATTR_SIZE__ 8
#define __PTHREAD_COND_SIZE__ 40
#define __PTHREAD_ONCE_SIZE__ 8
#define __PTHREAD_RWLOCK_SIZE__ 192
#define __PTHREAD_RWLOCKATTR_SIZE__ 16
struct __darwin_pthread_handler_rec { void (*__routine)(void *); void *__arg; struct __darwin_pthread_handler_rec *__next; };
#ifdef __cplusplus
#define __DARWIN_NULL __null
#else
#define __DARWIN_NULL ((void *)0)
#endif
typedef int64_t  __darwin_blkcnt_t;
typedef int32_t  __darwin_blksize_t;
typedef int32_t  __darwin_dev_t;
typedef unsigned int __darwin_fsblkcnt_t;
typedef unsigned int __darwin_fsfilcnt_t;
typedef uint32_t __darwin_gid_t;
typedef uint32_t __darwin_id_t;
typedef uint64_t __darwin_ino64_t;
typedef uint64_t __darwin_ino_t;
typedef unsigned int __darwin_natural_t;
typedef unsigned int __darwin_mach_port_name_t;
typedef unsigned int __darwin_mach_port_t;
typedef unsigned short __darwin_mode_t;
typedef int64_t  __darwin_off_t;
typedef int32_t  __darwin_pid_t;
typedef void *__darwin_pthread_attr_t;
typedef void *__darwin_pthread_cond_t;
typedef void *__darwin_pthread_condattr_t;
typedef unsigned long __darwin_pthread_key_t;
typedef void *__darwin_pthread_mutex_t;
typedef void *__darwin_pthread_mutexattr_t;
typedef void *__darwin_pthread_once_t;
typedef void *__darwin_pthread_rwlock_t;
typedef void *__darwin_pthread_rwlockattr_t;
typedef void *__darwin_pthread_t;
typedef uint32_t __darwin_sigset_t;
typedef int32_t  __darwin_suseconds_t;
typedef uint32_t __darwin_uid_t;
typedef uint32_t __darwin_useconds_t;
typedef unsigned char __darwin_uuid_t[16];
#endif
