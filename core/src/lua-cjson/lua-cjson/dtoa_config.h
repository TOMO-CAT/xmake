#ifndef _DTOA_CONFIG_H
#define _DTOA_CONFIG_H

#include <stdio.h>
#include <stdlib.h>

#if defined(_MSC_VER)
typedef __int8 int8_t;
typedef __int16 int16_t;
typedef __int32 int32_t;
typedef __int64 int64_t;
typedef unsigned __int8 uint8_t;
typedef unsigned __int16 uint16_t;
typedef unsigned __int32 uint32_t;
typedef unsigned __int64 uint64_t;
#ifdef _WIN64
typedef __int64 intptr_t;
typedef unsigned __int64 uintptr_t;
#else
typedef __int32 intptr_t;
typedef unsigned __int32 uintptr_t;
#endif
#else
#   include <stdint.h>
#endif

/* Ensure dtoa.c does not USE_LOCALE. Lua CJSON must not use locale
 * aware conversion routines. */
#undef USE_LOCALE

/* dtoa.c should not touch errno, Lua CJSON does not use it, and it
 * may not be threadsafe */
#define NO_ERRNO

#define Long    int32_t
#define ULong   uint32_t
#define Llong   int64_t
#define ULLong  uint64_t

#ifdef IEEE_BIG_ENDIAN
#define IEEE_MC68k
#else
#define IEEE_8087
#endif

#define MALLOC(n)   xmalloc(n)

static void *xmalloc(size_t size)
{
    void *p;

    p = malloc(size);
    if (!p) {
        fprintf(stderr, "Out of memory");
        abort();
    }

    return p;
}

#ifdef MULTIPLE_THREADS

/* Enable locking to support multi-threaded applications */

#include <pthread.h>

static pthread_mutex_t private_dtoa_lock[2] = {
    PTHREAD_MUTEX_INITIALIZER,
    PTHREAD_MUTEX_INITIALIZER
};

#define ACQUIRE_DTOA_LOCK(n)    do {                                \
    int r = pthread_mutex_lock(&private_dtoa_lock[n]);              \
    if (r) {                                                        \
        fprintf(stderr, "pthread_mutex_lock failed with %d\n", r);  \
        abort();                                                    \
    }                                                               \
} while (0)

#define FREE_DTOA_LOCK(n)   do {                                    \
    int r = pthread_mutex_unlock(&private_dtoa_lock[n]);            \
    if (r) {                                                        \
        fprintf(stderr, "pthread_mutex_unlock failed with %d\n", r);\
        abort();                                                    \
    }                                                               \
} while (0)

#endif  /* MULTIPLE_THREADS */

#endif  /* _DTOA_CONFIG_H */

/* vi:ai et sw=4 ts=4:
 */
