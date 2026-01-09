
#if !defined(LINCE_LINUX)
    #error LINCE_LINUX not defined. You should only include this file if you are on Linux.
#endif


#include <sys/types.h>
#include <sys/stat.h>
#include <uuid/uuid.h>
#include <unistd.h>
