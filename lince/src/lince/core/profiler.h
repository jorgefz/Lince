#ifndef LINCE_PROFILER_H
#define LINCE_PROFILER_H

#include <stdio.h>
#include "lince/core/core.h"

/* Returns the file used by the profiler to save benchmarking stats */
FILE* LinceGetProfiler();

/*
Provides a file to which stats are written.
If the profiler file is already set, it does nothing.
If the passed file is NULL, the profiler file is closed and set to NULL. 
*/
void LinceSetProfiler(FILE* file);

/* Returns number of milliseconds the application has been active */
double LinceGetTimeMillisec(void);


#ifdef LINCE_PROFILE

	/* Starts profiling block and timer */
	#define LINCE_PROFILER_START(ms_counter) \
		double ms_counter = LinceGetTimeMillisec()
	
	/* Ends profiling block and saves benchmark to file */
	#define LINCE_PROFILER_END(ms_counter) \
		if(LinceGetProfiler()) \
			fprintf(LinceGetProfiler(), "\"%s\": %.14g\n", \
			__FUNCTION__, LinceGetTimeMillisec()-ms_counter)
	
#else
	#define LINCE_PROFILER_START(ms_counter)
	#define LINCE_PROFILER_END(file, ms_counter)
#endif

#endif /* LINCE_PROFILER_H */