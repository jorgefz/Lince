#ifndef LINCE_CORE_H
#define LINCE_CORE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/* Version */
#ifdef LINCE_VERSION
    #undef LINCE_VERSION
#endif
#define LINCE_VERSION "0.7.0" //> Current Lince version


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

4) Directory

LINCE_DIR
    Directory from which executable is called.
    Necessary to load engine assets properly.

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

/* Directory */
#ifndef LINCE_DIR
    #define LINCE_DIR ""
#endif

/* Debugging */
#define LINCE_UNUSED(x) (void)(x)       //> Gets rid of unused variable compiler warnings
// #define LINCE_PROFILE

/* Logging */
#include "logger.h"
#define LINCE_INFO(...) LinceLoggerInfo(__VA_ARGS__)    //> Logs a message starting with `[INFO]`
#define LINCE_WARN(...) LinceLoggerWarn(__VA_ARGS__)    //> Logs a message starting with `[WARNING]`
#define LINCE_ERROR(...) LinceLoggerError(__VA_ARGS__)  //> Logs a message starting with `[ERROR]`

/** Quits the program if a given condition fails.
*   @param condition Expression that resolves to 0 (false) or not 0 (true).
*   @param msg  Formatted string printed if the condtion fails before quitting.
*               It can be followed by variadic arguments for the formatted string.
*/
#define LINCE_ASSERT(condition, msg, ...) \
    if(!(condition)) do { \
        LINCE_ERROR(msg, ##__VA_ARGS__); \
        LINCE_ERROR("at %s:%d in function '%s' ('%s' failed)", \
            __FILE__, __LINE__, __func__, #condition); \
        exit(-1); \
    } while(0) \

/** Quits the program if a given pointer is NULL. Used to check allocations.
*   @param ptr The pointer to check
*   @param size Size in bytes of the memory which failed to be allocated.
*/
#define LINCE_ASSERT_ALLOC(ptr, size) LINCE_ASSERT(ptr, " Failed to allocate %d bytes", (int)(size))


/* Constants & typedefs */
#define LINCE_NAME_MAX 100 /**> Used for shader variable names, etc*/
#define LINCE_STR_MAX 1000 /**> Used for longer buffers */


/** Custom boolean type
*/
typedef enum LinceBool{ LinceFalse = 0, LinceTrue = 1 } LinceBool;


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