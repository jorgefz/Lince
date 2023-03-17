#ifndef LINCE_LOGGER
#define LINCE_LOGGER
#include "stdio.h"

/*
Opens the logging file.
If it does not exist, it creates one. Otherwise, it creates it.
If LINCE_DEBUG is defined, no file is opened, and stderr is used instead.
*/
int LinceOpenLogger(const char* filename);

/* Reports a message, warning, or error to the log file. */
void LinceLoggerInfo(const char* fmt, ...);
void LinceLoggerWarn(const char* fmt, ...);
void LinceLoggerError(const char* fmt, ...);

/* Closes the file only if standard output is being used */
void LinceCloseLogger();

#endif /* LINCE_LOGGER */