#ifndef LINCE_LOGGER
#define LINCE_LOGGER
#include "stdio.h"

/** @brief Opens the logging file.
* If it does not exist, it creates one.
* If LINCE_DEBUG is defined, no file is opened, and stderr is used instead.
* @param filename path and name for the log file
* @returns 1 if successful, 0 if not.
*/
int LinceOpenLogger(const char* filename);

/** @brief Closes the file only if standard output is not being used */
void LinceCloseLogger();

/** @brief Default to stderr when no log file is open */
void LinceLoggerDefaultToStderr(int flag);

/** @brief Reports a message to the log file. */
void LinceLoggerInfo(const char* fmt, ...);
/** @brief Reports a warning to the log file. */
void LinceLoggerWarn(const char* fmt, ...);
/** @brief Reports an error to the log file. */
void LinceLoggerError(const char* fmt, ...);


#endif /* LINCE_LOGGER */