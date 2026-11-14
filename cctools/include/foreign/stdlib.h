/*
 * Cygwin defines __STRICT_ANSI__ with -std=c++0x which
 * disables needed functions and -std=gnu++0x doesn't work
 * work due to weird errors in libstdc++.
 */

#if defined(__CYGWIN__) || defined(__MINGW32__)
#pragma push_macro("__STRICT_ANSI__")
#undef __STRICT_ANSI__
#endif /* __CYGWIN__ */

#include_next <stdlib.h>

#if defined(__CYGWIN__) || defined(__MINGW32__)
#pragma pop_macro("__STRICT_ANSI__")
#endif /* __CYGWIN__ */
