#include "core/profiler.h"

#include <GLFW/glfw3.h>

static FILE* PROFILER = NULL;

FILE* LinceGetProfiler(){
	return PROFILER;
}

void LinceOpenProfiler(const char* filename){
	if(!filename){
        fprintf(stderr, "[Warning] Profiler filename undefined. Profiling is disabled.");
        return;
    }
	PROFILER = fopen(filename, "w");
}

void LinceCloseProfiler(){
	if(!PROFILER) return;
	fclose(PROFILER);
    PROFILER = NULL;
}

double LinceGetTimeMillisec(void){
	return (glfwGetTime() * 1000.0);
}

