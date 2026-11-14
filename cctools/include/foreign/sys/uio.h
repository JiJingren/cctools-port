#ifndef _CCT_SYS_UIO_H_
#define _CCT_SYS_UIO_H_
#include <sys/types.h>
#ifndef _STRUCT_IOVEC
#define _STRUCT_IOVEC
struct iovec { void *iov_base; size_t iov_len; };
#endif
#endif
