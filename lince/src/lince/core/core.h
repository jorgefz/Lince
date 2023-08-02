/** @file core.h

## Platform and setup macros
| Name          | Description                               |
| ------------- | ----------------------------------------- |
| LINCE_WINDOWS | Defined on Windows                        |
| LINCE_LINUX   | Defined on GNU/Linux                      |
| LINCE_MACOS   | Defined on MacOS                          |
| LINCE_VERSION | String with current engine version        |
| LINCE_DIR     | Directory where 'lince' folder is located |

## Debugging
| Name                      | Description                                        |
| ------------------------- | -------------------------------------------------- |
| LINCE_UNUSED(x)           | Supresses compiler warnings about unused variables |
| LINCE_DEBUG               | Defined if compiled on debug configuration         |
| LINCE_INFO(fmt, ...)      | Prints debug message - allows formatted string     |
| LINCE_WARN(fmt, ...)      | Prints debug warning - allows formatted string     |
| LINCE_ERROR(fmt, ...)     | Prints debug error - allows formatted string       |
| LINCE_ASSERT(x, fmt, ...) | Prints a message and exits if the condition fails  |
| LINCE_ASSERT_ALLOC(p, sz) | Exist if the given pointer is NULL                 |
| LINCE_PROFILE             | Enables profiling in debug mode                    |

## Engine constants
| Name                 | Description                     |
| -------------------- | --------------------------------|
| LINCE_NAME_MAX       | Maximum size for short names    |
| LINCE_NAME_MAX       | Maximum size for longer strings |

## Allocations
Customise memory management.
Note that these do not apply to the containers (e.g. array, hashmap, list).
| Name          | Description                 |
| --------------| ----------------------------|
| LINCE_MALLOC  | Custom version of `malloc`  |
| LINCE_CALLOC  | Custom version of `calloc`  |
| LINCE_REALLOC | Custom version of `realloc` |
| LINCE_FREE    | Custom version of `free`    |

*/


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
#define LINCE_VERSION "0.7.0" ///< Current Lince version

/* Platform */
#ifdef LINCE_WINDOWS
    // #include <windows.h>
#elif defined(LINCE_LINUX)
    // #include <unistd.h>
#elif defined(LINCE_MACOS)
    #error "MacOS not yet supported"
#else
    #error "Platform not supported"
#endif

/* Directory */
#ifndef LINCE_DIR
    #define LINCE_DIR ""  ///< Directory where 'lince' folder is located
#endif

/* Misc */
#define LINCE_UNUSED(x) (void)(x)  ///< Gets rid of unused variable compiler warnings

/* Profiling */
#if !defined(LINCE_DEBUG) && defined(LINCE_PROFILE)
    #undef LINCE_PROFILE
#endif

/* Logging */
#include "logger.h"
#define LINCE_INFO(...)  LinceLoggerInfo(__VA_ARGS__)  ///< Logs a message starting with `[INFO]`
#define LINCE_WARN(...)  LinceLoggerWarn(__VA_ARGS__)  ///< Logs a message starting with `[WARNING]`
#define LINCE_ERROR(...) LinceLoggerError(__VA_ARGS__) ///< Logs a message starting with `[ERROR]`

/** @brief Quits the program if a given condition fails.
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

/** @brief Quits the program if a given pointer is NULL. Used to check allocations.
*   @param ptr The pointer to check
*   @param size Size in bytes of the memory which failed to be allocated.
*/
#define LINCE_ASSERT_ALLOC(ptr, size) LINCE_ASSERT(ptr, " Failed to allocate %ld bytes", (long int)(size))

/* Constants & typedefs */
#define LINCE_NAME_MAX 100 ///< Used for short names (e.g. shader uniforms)
#define LINCE_TEXT_MAX 1000 ///< Used for longer string (e.g. descriptions)

/** @enum LinceBool
* Custom boolean type */
typedef enum LinceBool{ LinceFalse = 0, LinceTrue = 1 } LinceBool;

#ifndef LINCE_MALLOC
    #define LINCE_MALLOC malloc ///< Custom malloc
#endif

#ifndef LINCE_CALLOC
    #define LINCE_CALLOC calloc ///< Custom calloc
#endif

#ifndef LINCE_REALLOC
    #define LINCE_REALLOC realloc ///< Custom realloc
#endif

#ifndef LINCE_FREE
    #define LINCE_FREE free ///< Custom free
#endif

#endif // LINCE_CORE_H