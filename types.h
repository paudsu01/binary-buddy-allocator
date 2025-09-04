#ifndef _BUDDY_TYPES_
#define _BUDDY_TYPES_

#include <stddef.h>
#include <stdbool.h>

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
