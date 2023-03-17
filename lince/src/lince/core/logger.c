#include "logger.h"
#include "stdarg.h"
#include "assert.h"

static FILE* LOGFILE = NULL;


#define CHECK_LOGFILE       \
    if(!LOGFILE) do{        \
        LOGFILE = stderr;   \
        LinceLoggerWarn("Log file is not open. Using stderr."); \
    } while(0)              \

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
#else
    if(!filename){
        fprintf(stderr, "[Error] Log file undefined");
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