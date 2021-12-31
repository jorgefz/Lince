#ifndef LINCE_CORE_H
#define LINCE_CORE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef LINCE_WINDOWS
    #include <windows.h>
#else
    //unix
#endif

#define LINCE_STR_MAX 100

// Bool
typedef unsigned int lince_bool;
#define lince_false (lince_bool)0
#define lince_true (lince_bool)(!lince_false)

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