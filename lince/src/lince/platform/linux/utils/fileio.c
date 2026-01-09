#include <lince/platform/linux/linux_core.h>
#include <lince/utils/fileio.h>

size_t LinceFetchExecutablePath(char* buf, size_t max_size){

    size_t n_bytes = 0;
	memset(buf, 0, max_size);

	ssize_t retval = readlink("/proc/self/exe", buf, max_size);
	if(retval == -1){
		return 0;
	}
    n_bytes = (size_t)retval;

	// Strip filename from full path
	char* end = buf + n_bytes;
	while(*end != '/' && *end != '\\' && end != buf){
		end--;
	}
	if(end == buf){
		return 0;
	}

	end++; // keep last slash
	*(end+1) = '\0';
	return (size_t)(end - buf);
}


LinceBool LinceIsFile(string_t path){
	LinceBool is_file = LinceFalse;
	struct stat path_stat;
    stat(path.str, &path_stat);
    return (LinceBool)S_ISREG(path_stat.st_mode);
}


LinceBool LinceIsDir(string_t path){
	LinceBool is_dir = LinceFalse;
	struct stat path_stat;
    if(stat(path.str, &path_stat) != 0){
		return LinceFalse;
	}
	return (path_stat.st_mode & S_IFDIR) != 0;
}


LinceBool LinceMakeDir(string_t path){
    struct stat path_stat;
    if (stat(path.str, &path_stat) == -1) {
        int status = mkdir(path.str, 0700);
        return (LinceBool)(status == 0);
    }
    return LinceFalse;
}