#include "logger.h"
#include "stdarg.h"
#include "assert.h"

static FILE* _logfile = NULL;

#ifdef LINCE_DEBUG
    static int _use_default_stderr = 1;
#else
    static int _use_default_stderr = 0;
#endif

#define WRITE_LOGFILE(file, header, fmt)   \
    do {                                   \
        fprintf(file, "["header"] ");      \
        va_list args;                      \
        va_start(args, fmt);               \
        vfprintf(file, fmt, args);         \
        va_end(args);                      \
        fprintf(file, "\n");               \
    } while(0)                             \


int LinceOpenLogger(const char* filename){
    if(!filename) return 0;
    _logfile = fopen(filename, "w");
    if(!_logfile) return 0;
    return 1;
}

void LinceCloseLogger(){
#ifndef LINCE_DEBUG
    if(_logfile) fclose(_logfile);
    _logfile = NULL;
#endif
}

void LinceLoggerDefaultToStderr(int flag){
    _use_default_stderr = !!flag;
}

void LinceLoggerInfo(const char* fmt, ...){
    if(_logfile){
        WRITE_LOGFILE(_logfile, "INFO", fmt);
    } else if(_use_default_stderr == 1){
        WRITE_LOGFILE(stderr, "INFO", fmt);
    }
}

void LinceLoggerWarn(const char* fmt, ...){
    if(_logfile){
        WRITE_LOGFILE(_logfile, "WARNING", fmt);
    } else if(_use_default_stderr == 1){
        WRITE_LOGFILE(stderr, "WARNING", fmt);
    }
}

void LinceLoggerError(const char* fmt, ...){
    if(_logfile){
        WRITE_LOGFILE(_logfile, "ERROR", fmt);
    } else if(_use_default_stderr == 1){
        WRITE_LOGFILE(stderr, "ERROR", fmt);
    }
}
