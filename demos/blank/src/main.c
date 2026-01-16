

#include <lince.h>

int main() {

	LinceAppSetConfigFile(string_scoped_lit("demos/blank/project.toml"));
	
	LinceRun();

	return 0;
}