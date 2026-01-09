#include <lince/platform/windows/windows_core.h>
#include <lince/utils/fileio.h>

size_t LinceFetchExecutablePath(char* buf, size_t max_size){

	size_t n_bytes = 0;
	memset(buf, 0, max_size);

	/// TODO: check return code
	n_bytes = (size_t)GetModuleFileNameA(NULL, buf, (DWORD)max_size);

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

	// Extend path length limit from 260 to 32767 by appending \\?\ to path.
	// Only works when using GetFileAttributesW, which takes a wchar string.
	// Also requires all slashes to be backwards, e.g. "\\"
	// https://learn.microsoft.com/en-us/windows/win32/fileio/maximum-file-path-limitation
	/*
	string_t long_path = string_from_fmt("\\\\?\\%.*s", (int)path.len, path.str);
	for(size_t i = 0; i != long_path.len; ++i){
		char* p = long_path.str + i;
		if(*p == '/') *p = '\\';
	}
	DWORD attrib = GetFileAttributesW(long_path.str);
	string_free(&long_path);
	*/
	
	DWORD attrib = GetFileAttributesA(path.str);
	is_file = (attrib != INVALID_FILE_ATTRIBUTES) && !(attrib & FILE_ATTRIBUTE_DIRECTORY);
	return is_file;
}



LinceBool LinceIsDir(string_t path){
	LinceBool is_dir = LinceFalse;

	// Extend path length limit from 260 to 32767 by appending \\?\ to path.
	// Only works when using GetFileAttributesW, which takes a wchar string.
	// Also requires all slashes to be backwards, e.g. "\\"
	// https://learn.microsoft.com/en-us/windows/win32/fileio/maximum-file-path-limitation
	/*
	string_t long_path = string_from_fmt("\\\\?\\%.*s", (int)path.len, path.str);
	for(size_t i = 0; i != long_path.len; ++i){
		char* p = long_path.str + i;
		if(*p == '/') *p = '\\';
	}
	*/
	
	DWORD attrib = GetFileAttributesA(path.str);
	is_dir = attrib != INVALID_FILE_ATTRIBUTES && (attrib & FILE_ATTRIBUTE_DIRECTORY);
	return is_dir;
}


LinceBool LinceMakeDir(string_t path){
    return (_mkdir(path.str) == 0);
}