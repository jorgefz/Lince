/** @file core.h

## Platform and setup macros
| Name          | Description                               |
| ------------- | ----------------------------------------- |
| LINCE_WINDOWS | Defined on Windows                        |
| LINCE_LINUX   | Defined on GNU/Linux                      |
| LINCE_MACOS   | Defined on MacOS                          |
| LINCE_VERSION | String with current engine version        |
| LINCE_DIR     | Directory where output files are saved    |

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
| Name                 | Description                          |
| -------------------- | ------------------------------------ |
| LINCE_NAME_MAX       | Maximum size for short names         |
| LINCE_TEXT_MAX       | Maximum size for longer strings      |
| LINCE_PATH_MAX       | Maximum size for a path or directory |

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
#define LINCE_VERSION "0.8.0" ///< Current Lince version

#define LINCE_GL_VERSION_MAJOR 4 ///< Minimum major OpenGL version supported
#define LINCE_GL_VERSION_MINOR 5 ///< Minimum minor OpenGL version supported

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
    #define LINCE_DIR ""  ///< Directory where output files are saved
#endif

/* Misc */
#define LINCE_UNUSED(x) (void)(x)  ///< Gets rid of unused variable compiler warnings

/* Profiling */
#if !defined(LINCE_DEBUG) && defined(LINCE_PROFILE)
    #undef LINCE_PROFILE
#endif

/* Logging */

/** Logs a string message starting with `[INFO]`.
 * Can be a formatted string followed by an argument list.
 * Requires including "logger.h" header to work!
*/
#define LINCE_INFO(msg, ...)  LinceLoggerInfo(msg, ##__VA_ARGS__)

/** Logs a string message starting with `[WARNING]`.
 * Can be a formatted string followed by an argument list.
 * Requires including "logger.h" header to work!
*/
#define LINCE_WARN(msg, ...)  LinceLoggerWarn(msg, ##__VA_ARGS__)

/** Logs a string message starting with `[ERROR]`.
 * Can be a formatted string followed by an argument list.
 * Requires including "logger.h" header to work!
*/
#define LINCE_ERROR(msg, ...) LinceLoggerError(msg, ##__VA_ARGS__)


#ifdef LINCE_DEBUG
    /** @brief Logs an error message if a condition is not satisfied.
    *       If LINCE_DEBUG is set, the program also exits.
    *   @param condition Expression to verify.
    *   @param msg  Error message to log. Can be a formatted string followed by an argument list.
    *   @param ... Optional argument list for formatted string
    */
    #define LINCE_ASSERT(condition, msg, ...) \
        if(!(condition)) do { \
            LINCE_ERROR(msg, ##__VA_ARGS__); \
            LINCE_ERROR("at %s:%d in function '%s' ('%s' failed)", \
                __FILE__, __LINE__, __func__, #condition); \
            exit(-1); \
        } while(0)
#else
    #define LINCE_ASSERT(condition, msg, ...) \
        if(!(condition)) do { \
            LINCE_ERROR(msg, ##__VA_ARGS__); \
            LINCE_ERROR("at %s:%d in function '%s' ('%s' failed)", \
                __FILE__, __LINE__, __func__, #condition); \
        } while(0)
#endif


/** @brief Quits the program if a given pointer is NULL. Used to check allocations.
*   @param ptr The pointer to check
*   @param size Size in bytes of the memory which failed to be allocated.
*/
#define LINCE_ASSERT_ALLOC(ptr, size) LINCE_ASSERT(ptr, "Failed to allocate %ld bytes", (long int)(size))

/* Constants & typedefs */
#define LINCE_NAME_MAX  64  ///< Used for short names (e.g. shader uniforms)
#define LINCE_TITLE_MAX 256 ///< Limit to the number of characters in the title
#define LINCE_TEXT_MAX 1024 ///< Used for longer string (e.g. descriptions)
#define LINCE_PATH_MAX 1024 ///< Max number of characters in a path

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