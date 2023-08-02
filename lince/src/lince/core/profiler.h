/** @file profiler.h
* Usage:
* ```c
* void my_function(){
* 	LINCE_PROFILER_START(unique_timer_name);
* 	// expensive code
* 	LINCE_PROFILER_END(unique_timer_name);
* }
* ```
*/

#ifndef LINCE_PROFILER_H
#define LINCE_PROFILER_H

#include <stdio.h>
#include "lince/core/core.h"

/** @brief Returns the file used by the profiler to save benchmarking stats */
FILE* LinceGetProfiler();

/** @brief Opens the file to which profiling data is written.
If the filename is NULL, profiling will not start.
*/
void LinceOpenProfiler(const char* filename);

/** @brief Closes the file used for profiling */
void LinceCloseProfiler();

/** @brief Returns number of milliseconds the application has been active */
double LinceGetTimeMillisec(void);


#ifdef LINCE_PROFILE
	
	/// @brief Starts profiling block and timer
	/// @param ms_counter unique name for the time counter variable
	#define LINCE_PROFILER_START(ms_counter) \
		double ms_counter = LinceGetTimeMillisec() 
	
	/// @brief Ends profiling block and saves benchmark to file
	/// @param ms_counter unique name for the time counter variable
	#define LINCE_PROFILER_END(ms_counter) \
		if(LinceGetProfiler()) \
			fprintf(LinceGetProfiler(), "\"%s\": %.14g\n", \
			__FUNCTION__, LinceGetTimeMillisec()-ms_counter)
	
	
#else
	
	/// @brief Starts profiling block and timer
	/// @param ms_counter unique name for the time counter variable
	#define LINCE_PROFILER_START(ms_counter)

	/// @brief Ends profiling block and saves benchmark to file
	/// @param ms_counter unique name for the time counter variable
	#define LINCE_PROFILER_END(ms_counter)
#endif

#endif /* LINCE_PROFILER_H */