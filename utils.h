#ifndef _BUDDY_UTILS_
#define _BUDDY_UTILS_

#include <stddef.h> // Required for size_t

// Round to the nearest 2^i such that 2^i <= value
size_t round_log(size_t value);

#endif