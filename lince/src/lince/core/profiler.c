#include "core/profiler.h"

#include <GLFW/glfw3.h>

static LinceProfiler PROFILER = {0};

FILE* LinceGetProfiler(){
	return PROFILER.output_file;
}

void LinceOpenProfiler(const char* filename){
	if(!filename){
        fprintf(stderr, "[Warning] Profiler filename undefined. Profiling is disabled.");
        return;
    }
	PROFILER.output_file = fopen(filename, "w");
}

void LinceCloseProfiler(){
	if(!PROFILER.output_file) return;
	fclose(PROFILER.output_file);
    PROFILER.output_file = NULL;
}


