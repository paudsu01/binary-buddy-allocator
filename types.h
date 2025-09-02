#ifndef _BUDDY_TYPES_
#define _BUDDY_TYPES_

#include <stddef.h>

// Any memory chunk returned to the user via `malloc` will contain this information as a header
typedef struct{
  size_t size;
  bool free;
} header;

// We will use struct inheritance to access `header` for any node
typedef struct __node{
  header header;
  struct __node * next;
  struct __node * previous;
} node;

#endif
