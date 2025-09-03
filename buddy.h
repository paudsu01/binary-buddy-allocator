#ifndef _MEM_ALLOCATOR_
#define _MEM_ALLOCATOR_

#include <stddef.h>

void* malloc(size_t size);
void free(void* pointer);

#endif
