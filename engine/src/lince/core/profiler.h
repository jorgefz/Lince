#ifndef LINCE_PROFILER_H
#define LINCE_PROFILER_H

//#include "core/core.h"

/* Returns number of milliseconds the application has been active */
double LinceGetTimeMillisec(void);

#ifdef LINCE_PROFILE
	/* Starts profiling block and timer */
	#define LINCE_PROFILER_START(ms_counter) \
		double ms_counter = LinceGetTimeMillisec()
	/* Ends profiling block and saves benchmark to file */
	#define LINCE_PROFILER_END(ms_counter, file) \
		if(file) fprintf(file, "\"%s\": %.14g\n", \
			__FUNCTION__, LinceGetTimeMillisec()-ms_counter)
#else
	#define LINCE_PROFILER_START(ms_counter)
	#define LINCE_PROFILER_END(file, ms_counter)
#endif

#endif /* LINCE_PROFILER_H */