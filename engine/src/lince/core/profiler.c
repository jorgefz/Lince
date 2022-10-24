#include "core/profiler.h"

#include <GLFW/glfw3.h>

static FILE* outfile = NULL;

FILE* LinceGetProfiler(){
	return outfile;
}

void LinceSetProfiler(FILE* file){
	if(file && !outfile){
		outfile = file;
	} else if(!file && outfile) {
		fclose(outfile);
		outfile = NULL;
	}
}

double LinceGetTimeMillisec(void){
	return (glfwGetTime() * 1000.0);
}

