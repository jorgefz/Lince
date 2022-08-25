#ifndef LINCE_CORE_H
#define LINCE_CORE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/*
--- Macros ---

1) Platform

LINCE_WINDOWS   - Windows
LINCE_LINUX     - Linux  
LINCE_MACOS     - MacOS  

2) Debug messages

LINCE_UNUSED( x )
    Removes compiler warning about unused variable

LINCE_LOGFILE
    File handle to which debug messages are printed.
    Set to 'stderr' by default.

LINCE_DEBUG
    Defined if the application was compiled in debug mode.
    If undefined, the macros below are set to nothing.

LINCE_INFO( message, ... )
    Prints message to console.

LINCE_ASSERT( condition, message, ... )
    If condition is met, prints message and exits.

LINCE_ASSERT_ALLOC( pointer, size )
    Used for checking that malloc (& family) functions sucessfully
    allocated requested memory.
    If 'pointer' is NULL, prints error message quoting
    number of bytes requested ('size'), and exits. 

3) Engine constants

LINCE_NAME_MAX
    Specifies maximum length of names, string identifiers, shader uniforms...
    Set to 100 by default.

LINCE_STR_MAX
    Maximum length of longer buffers, such as messages or text.

*/


/* --- Platform --- */
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
#define LINCE_UNUSED(x) (void)(x)

#ifndef LINCE_LOGFILE
#    define LINCE_LOGFILE stderr
#endif

#ifdef LINCE_DEBUG
#   define LINCE_INFO(...) do{ \
            fprintf(LINCE_LOGFILE, __VA_ARGS__); \
            fprintf(LINCE_LOGFILE,"\n"); \
        } while(0);
#   define LINCE_ASSERT(condition, msg, ...) \
    if(!(condition)) { \
        do { \
            fprintf(LINCE_LOGFILE, " --- Error: %s:%d in function '%s' ('%s' failed)\n",\
                            __FILE__, __LINE__, __func__, #condition); \
            fprintf(LINCE_LOGFILE, "\t"); \
            fprintf(LINCE_LOGFILE, msg, ##__VA_ARGS__); \
            fprintf(LINCE_LOGFILE, "\n"); \
            exit(-1); \
        } while(0); \
    }
#   define LINCE_ASSERT_ALLOC(ptr, size) LINCE_ASSERT(ptr, " Failed to allocate %d bytes", (int)(size))
#else
#   define LINCE_INFO
#   define LINCE_ASSERT
#   define LINCE_ASSERT_ALLOC
#endif

/* Constants & typedefs */
#define LINCE_NAME_MAX 100 /* used for shader variable names, etc*/
#define LINCE_STR_MAX 1000 /* used for longer buffers */
typedef enum LinceBool{ LinceFalse = 0, LinceTrue = 1 } LinceBool;


/* Loops & utils */

/*
Loops over the items in an array.
    item:   pointer to an element of the array. Must be declared before.
    array:  pointer to the beginning of memory where data is stored.
    len:    number of items in the array
*/
#define LinceForeach(item, array, len) for(item = array; item != array+len; ++item)


/* Memory */

#ifndef LINCE_MALLOC
#define LINCE_MALLOC malloc
#endif

#ifndef LINCE_CALLOC
#define LINCE_CALLOC calloc
#endif

#ifndef LINCE_REALLOC
#define LINCE_REALLOC realloc
#endif

#ifndef LINCE_FREE
#define LINCE_FREE free
#endif

#endif // LINCE_CORE_H