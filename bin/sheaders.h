#ifndef SHEADERS_H
#define SHEADERS_H

#ifdef __i386__
    typedef unsigned char      uint8_t;
    typedef unsigned short     uint16_t;
    typedef unsigned int       uint32_t;
    typedef unsigned long long uint64_t;

    typedef signed char        int8_t;
    typedef signed short       int16_t;
    typedef signed int         int32_t;
    typedef signed long long   int64_t;

    typedef uint32_t           size_t;
    typedef int32_t            ssize_t;
    typedef uint32_t           uintptr_t;
    typedef int32_t            intptr_t;

#elif defined(__x86_64__)
    typedef unsigned char      uint8_t;
    typedef unsigned short     uint16_t;
    typedef unsigned int       uint32_t;
    typedef unsigned long      uint64_t;

    typedef signed char        int8_t;
    typedef signed short       int16_t;
    typedef signed int         int32_t;
    typedef signed long        int64_t;

    typedef uint64_t           size_t;
    typedef int64_t            ssize_t;
    typedef uint64_t           uintptr_t;
    typedef int64_t            intptr_t;

#else
    #error "Unsupported architecture"
#endif

#ifndef NULL
    #define NULL ((void*)0)
#endif

typedef enum { false, true } bool;

#define UINT32_MAX 0xFFFFFFFFU
#define INT32_MAX  0x7FFFFFFF
#define INT32_MIN  (-INT32_MAX - 1)

#ifdef __GNUC__
    #define NORETURN __attribute__((noreturn))
#else
    #define NORETURN
#endif

#endif
