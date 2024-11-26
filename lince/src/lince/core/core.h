/** @file core.h
 * 
 * Order of includes:
 *  1. Standard library headers
 *  2. defs.h, only macro definitions
 *  3. logger.h, needs macro definitions but nothing else
 *  4. allocator.h, needs macros and logger, but nothing else.
 * 
*/


#ifndef LINCE_CORE_H
#define LINCE_CORE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "lince/core/defs.h"
#include "lince/core/logger.h"
#include "lince/core/allocator.h"

#endif // LINCE_CORE_H