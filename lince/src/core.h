#ifndef LINCE_CORE_H
#define LINCE_CORE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Platform */
#ifdef LINCE_WINDOWS
    // #include <windows.h>
#elif defined(LINCE_LINUX)
    // linux
#elif defined(LINCE_MACOS)
    // MacOS
#else
    // other
#endif

/* Debugging */
#ifdef LINCE_DEBUG
#   define LINCE_LOG(...) fprintf(stderr, __VA_ARGS__);
#   define LINCE_ASSERT(condition, msg) \
    if(!(condition)) { \
        do { \
            fprintf(stderr, "Error: '%s':%d in function '%s': %s (%s) \n", \
                            __FILE__, __LINE__, __func__, msg, #condition); \
            exit(-1); \
        } while(0); \
    }
#else
#   define LINCE_LOG
#   define LINCE_ASSERT
#endif

/* Constants & typedefs */
#define LINCE_STR_MAX 100
typedef enum LinceBool{ LinceFalse = 0, LinceTrue = 1 } LinceBool;


#endif // LINCE_CORE_H