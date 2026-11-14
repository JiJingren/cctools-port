#ifndef _CCT_PWD_H_
#define _CCT_PWD_H_
#include <sys/types.h>
#ifndef _STRUCT_PASSWD
#define _STRUCT_PASSWD
struct passwd { char *pw_name; char *pw_passwd; int pw_uid; int pw_gid; char *pw_gecos; char *pw_dir; char *pw_shell; };
#endif
#endif
