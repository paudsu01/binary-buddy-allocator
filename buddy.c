#include "buddy.h"
#include <sys/mman.h>
#include <math.h>
#include "types.h"

#define MIN_ALLOCATION() round_log(sizeof(node))
// 2^20 = 1 MB ~ 1,048,576 Bytes maximum possible allocation
// This is also the total size of the memory we are handling for this implementation.
#define MAX_ALLOCATION 20

// Note: Every free list is a doubly linked list.
// index for free list of size `2^k` is given by (MAX_ALLOCATION - k)
// For example, the free list that holds chunks of 512KB would be in index (MAX_ALLOCATION - 19) i.e in this case index `1`.
// Note: we won't have 20 free lists. We will have `20 - round_log(sizeof(node))` many free lists.
node* free_lists[20] = {NULL};
bool init = false;

void* malloc(size_t size){
  // Once a block is free, we need at least the sizeof(node) to be able to store it in its freelist(if we cannot coalesce it with its buddy).
  if (size < sizeof(node)) return NULL;

  // Init total memory if not already using `mmap`
  if (!init){
    // Initially, we think of our free space as just one chunk of size 2^k, where k in this case = 20.
    node* node = mmap(NULL, (size_t) pow(2, MAX_ALLOCATION), PROT_READ|PROT_WRITE, MAP_ANON|MAP_PRIVATE, -1, 0);
    if (node == MAP_FAILED) return NULL;
    init = true;
  }

  // Find the free list for the given size.
  // More specifically, we need to first round up (size + sizeof(header))
  // to the nearest 2^i such that 2^i >= size. This is because we only give out blocks of fixed sizes.
  // (MAX_ALLOCATION - i) will give the index to the freelist for the requested size
  size_t power = round_log(size + sizeof(header));
  node* freelist = free_lists[MAX_ALLOCATION - power];


  // If the freelist is not empty, we have the memory chunk! Job done
  // We just need to make sure to update that the block is in use and return the appropriate pointer
  if (freelist != NULL){
    // Remove the node from the freelist
    node* selected_node = freelist;
    freelist = freelist->next;
    freelist->previous = NULL;
    return update_header_and_return_pointer(selected_node);

  } else{
    // We now need to find if there exists a bigger block that we can split. We do this recursively
    // We also need to make sure we put the unused split blocks in their corresponding free lists
    // @todo

  }



  // @todo
  return NULL;
}

void free(void* pointer){
  //@todo
}

/* Helper functions */

void* update_header_and_return_pointer(node* node){
    // reset next, previous and set it to in use
    node->next = NULL;
    node->previous = NULL;
    node->header.free = false;
    return (node + 1);
}