#include "lince/core/core.h"
#include "lince/core/logger.h"
#include "lince/utils/memory.h"
#include "lince/utils/fileio.h"


string_t LinceReadFile(string_t path){
	LINCE_INFO("Reading file '%s'", path.str);
	
	FILE* handle = fopen(path.str, "r");
	if(!handle){
		LINCE_ERROR("Failed to open file '%s'", path);
		return (string_t){0};
	}

	/* Get file length */
	fseek(handle, 0, SEEK_END);
	size_t size = ftell(handle);
	fseek(handle, 0, SEEK_SET);

	if(size == 0){
		LINCE_WARN("File is empty '%s'", path);
		fclose(handle);
		return (string_t){0};
	}

	string_t contents = string_from_len(size);
	fread(contents.str, size, 1, handle); // load file data into buffer
	fclose(handle);

	return contents;
}

