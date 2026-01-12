/** @file defs.h
 * `defs.h` contains type definitions for the library as well as useful macros
 *  and the definition of a memory allocation interface. 
 *  
 * +--------------+
 * |    Macros    |
 * +--------------+
 * 
 * +------------------------+----------------------------------------+
 * | Macro                  | Description                            |
 * +------------------------+----------------------------------------+
 * | DAST_VERSION_MAJOR     | Major DAST version (semver).           |
 * | DAST_VERSION_MINOR     | Minor DAST version (semver).           |
 * | DAST_64BIT             | DAST was compiled in 64-bit mode.      |
 * | DAST_32BIT             | DAST was compiled in 32-bit mode.      |
 * | DAST_NO_STDLIB         | Disables all standard library includes |
 * | DAST_DEFAULT_ALLOCATOR | Default allocator used by the library  |
 * +------------------------+----------------------------------------+
 * 
 */


#ifndef DAST_DEFS_H
#define DAST_DEFS_H

/* Version */
#define DAST_VERSION_MAJOR 1
#define DAST_VERSION_MINOR 1

/* Check architecture */

/* Windows */
#if defined(_WIN32) || defined(_WIN64)
    #if defined(_WIN64)
        #define DAST_64BIT
    #else
        #define  DAST_32BIT
    #endif
/* Linux - GCC */
#elif defined(__GNUC__) || defined(__clang__)
    #if defined(__x86_64__) || defined(__ppc64__)
        #define DAST_64BIT
    #else
        #define DAST_32BIT
    #endif
#endif


#ifdef DAST_NO_STDLIB

    /* Fixed types */
    #if defined(DAST_32BIT)
    
        typedef          char  dast_i8;
        typedef unsigned char  dast_u8;
        typedef          short dast_i16;
        typedef unsigned short dast_u16;
        typedef          int   dast_i32;
        typedef unsigned int   dast_u32;
        typedef unsigned long  dast_sz;

    #elif defined(DAST_64BIT)
    
        typedef          char  dast_i8;
        typedef unsigned char  dast_u8;
        typedef          short dast_i16;
        typedef unsigned short dast_u16;
        typedef          int   dast_i32;
        typedef unsigned int   dast_u32;
        typedef unsigned long long dast_sz;
    
    #endif

    /* 64-bit int types */
    #if defined(_MSC_VER)
        typedef          __int64 dast_i64;
        typedef unsigned __int64 dast_u64;
    #else
        typedef          long long dast_i64;
        typedef unsigned long long dast_u64;
    #endif

    /* Compile-time constants */
    #define dast_null ((void*)0)

#else

    #include <string.h> /* memcmp, memcpy, memmove */
    #include <stdint.h> /* sized types (e.g. uint32_t) */
    #include <stddef.h> /* size_t, NULL */
    #include <stdlib.h> /* malloc, realloc, free */
    #include <stdarg.h> /* va_arg, etc */
    #include <stdio.h>  /* vsnprintf */

    /* Fixed types */
    typedef int8_t   dast_i8;
    typedef uint8_t  dast_u8;
    typedef int16_t  dast_i16;
    typedef uint16_t dast_u16;
    typedef int32_t  dast_i32;
    typedef uint32_t dast_u32;
    typedef int64_t  dast_i64;
    typedef uint64_t dast_u64;
    typedef size_t   dast_sz;

    /* Compile-time constants */
    #define dast_null NULL

#endif

typedef dast_u32 dast_bool; /**< Boolean type */
#define dast_true  (dast_bool)1 /**< Boolean true */
#define dast_false (dast_bool)0 /**< Boolean false */

/* Hashing */
#ifdef DAST_HASH_64BIT
    typedef dast_u64 dast_hash_t; /**< 64-bit hash type */
#else
    typedef dast_u32 dast_hash_t; /**< 32-bit hash type */
#endif


#endif /* DAST_DEFS_H */

