#include "logger.h"
#include "stdarg.h"
#include "assert.h"

static FILE* LOGFILE = NULL;

#ifdef LINCE_DEBUG
    #define CHECK_LOGFILE LOGFILE = stderr
#elif defined( LINCE_RELEASE )
    #define CHECK_LOGFILE if(!LOGFILE) return;            
#endif

#define WRITE_LOGFILE(header, fmt)          \
    do {                                    \
        fprintf(LOGFILE, "["header"] ");    \
        va_list args;                       \
        va_start(args, fmt);                \
        vfprintf(LOGFILE, fmt, args);        \
        va_end(args);                       \
        fprintf(LOGFILE, "\n");             \
    } while(0)                              \


int LinceOpenLogger(const char* filename){
#ifdef LINCE_DEBUG
    LOGFILE = stderr;
    (void)filename;
    return 1;
#elif defined( LINCE_RELEASE )
    if(!filename){
        return 0;
    }
    LOGFILE = fopen(filename, "w");
    if(!LOGFILE) return 0;
    return 1;    
#endif
}

void LinceLoggerInfo(const char* fmt, ...){
    CHECK_LOGFILE;
    WRITE_LOGFILE("INFO", fmt);
}

void LinceLoggerWarn(const char* fmt, ...){
    CHECK_LOGFILE;
    WRITE_LOGFILE("WARNING", fmt);
}

void LinceLoggerError(const char* fmt, ...){
    CHECK_LOGFILE;
    WRITE_LOGFILE("ERROR", fmt);
}

void LinceCloseLogger(){
#ifndef LINCE_DEBUG
    if(LOGFILE && LOGFILE != stderr) fclose(LOGFILE);
    LOGFILE = NULL;
#endif
}