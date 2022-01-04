#ifndef LINCE_CORE_H
#define LINCE_CORE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef LINCE_WINDOWS
    // #include <windows.h>
#elif defined(LINCE_LINUX)
    // linux
#elif defined(LINCE_MACOS)
    // MacOS
#else
    // other
#endif

#define LINCE_STR_MAX 100

// Bool
typedef enum LinceBool{ LinceFalse = 0, LinceTrue = 1 } LinceBool;

// Assert
#define LINCE_ASSERT(condition, msg) \
    if(!(condition)) { \
        do { \
            fprintf(stderr, "Error: '%s':%d in function '%s': %s (%s) \n", \
                            __FILE__, __LINE__, __func__, msg, #condition); \
            exit(-1); \
        } while(0); \
    }

#endif // LINCE_CORE_H