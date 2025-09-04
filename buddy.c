#include <sys/mman.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>

#include "buddy.h"
#include "types.h"
#include "utils.h"

#define MIN_ALLOCATION() round_log(sizeof(node))
// 2^20 = 1 MB ~ 1,048,576 Bytes maximum possible allocation
// This is also the total size of the memory we are handling for this implementation.
#define MAX_ALLOCATION 20
#define MIN(a,b) (size_t) a < (size_t) b ? (size_t) a : (size_t) b

// Note: Every free list is a doubly linked list.
// index for free list of size `2^k` is given by (MAX_ALLOCATION - k)
// For example, the free list that holds chunks of 512KB would be in index (MAX_ALLOCATION - 19) i.e in this case index `1`.
// Note: we won't have 20 free lists. We will have `20 - round_log(sizeof(node))` many free lists.
static node* free_lists[20] = {NULL};
static void* starting_address;
static bool init = false;

/* Helper Function prototypes */
void reset_node(node* buddy, int power);
void* update_header_and_return_pointer(node* node);
void add_chunk(node* buddy_one, int power);
void* request_chunk(int power);
void remove_node(node* buddy_two);
void coalesce(node* buddy_one, size_t power);

void* malloc(size_t size){
  if (size < 1) return NULL;
  // Once a block is free, we need at least the sizeof(node) to be able to
  // store it in its freelist(if we cannot coalesce it with its buddy).
  if ((size + sizeof(header)) < sizeof(node)) size = sizeof(node) - sizeof(header);

  // Find the free list for the given size.
  // More specifically, we need to first round up (size + sizeof(header))
  // to the nearest 2^i such that 2^i >= size. This is because we only give out blocks of fixed sizes.
  // (MAX_ALLOCATION - i) will give the index to the freelist for the requested size
  size_t power = round_log(size + sizeof(header));

  // Init total memory if not already using `mmap`
  if (!init){
    // Initially, we think of our free space as just one chunk of size 2^k, where k in this case = 20.
    node* node = mmap(NULL, (size_t) pow(2, MAX_ALLOCATION), PROT_READ|PROT_WRITE, MAP_ANON|MAP_PRIVATE, -1, 0);
    if (node == MAP_FAILED) return NULL;
    starting_address = (void*) node;
    free_lists[0] = node;
    reset_node(node, 20);
    init = true;
  }

  // Request for a chunk
  // This is a *recursive* function.
  node* chunk = request_chunk(power);

  // Set it to `in use` and return the necessary pointer
  return update_header_and_return_pointer(chunk);
}

void free(void* pointer){
  if (!pointer) return;

  // get access to the header
  header* header_info = ((header *)pointer - 1);
  if (!header_info) return;

  header_info->free = true;
  size_t power = header_info->size;

  // Recursive function
  coalesce((void*)header_info, power);
}

/* Helper functions */

void coalesce(node* buddy_one, size_t power){
  if (power == MAX_ALLOCATION){
    free_lists[0] = buddy_one;
    return;
  }

  // Find the buddy's block address
  size_t offset = ((char *)buddy_one - (char* )starting_address);
  // flip the power_th bit.
  size_t buddy_offset = offset ^ (1 << power);
  header* buddy_two = (header*) ((char*)starting_address + buddy_offset);

  // Check if buddy is free
  // If not, add `buddy_one` to its appropriate freelist
  if (!buddy_two->free){
    // reset node and header information
    reset_node((node*) buddy_one, power);
    add_chunk(buddy_one, power);
  } else{
    // Remove the other buddy from its free list since we want to combine these buddies together!
    remove_node((node*) buddy_two);

    // Combine the buddies together. Find the smaller address between them which should be the address of the parent
    size_t min_offset = MIN(offset, buddy_offset);
    node* parent = (node*) ((char*)starting_address + min_offset);
    reset_node(parent, power + 1);

    // Now, we want to perform recursive merging
    // We want to keep merging these buddies if we can!
    coalesce(parent, power+1);
  }
}

/*
  * recursive function:
  => We return if we have a chunk,
  otheriwse, we request the freelist with blocks of size 2^(power+1) for a chunk. We then split the chunk into two and return one of them
  * Base case: The current freelist has a chunk available. It will give us that chunk.
  * Recursive case: The current freelist doesn't have a chunk available. So, it is also going to request a bigger chunk first
              from the freelist with blocks of size 2^(power+1) (recursive) and then split the chunk it recieved
              and returning us one of them.
  Note: if `power` is K, the the freelist associated with this size will
  contains chunks of size 2^K.
 */
void* request_chunk(int power){
  node** freelist = &(free_lists[MAX_ALLOCATION - power]);
  // Base case: Current freelist has a chunk
  if (*freelist != NULL){
    node* selected_node = (*freelist);
    // Remove the chunk from the front
    *freelist = (*freelist)->next;
    if (*freelist != NULL) (*freelist)->previous = NULL;
    return selected_node;

  } else{
    // Base case
    if (power == MAX_ALLOCATION) return NULL;

    // Recursive case
    node* bigger_chunk = (node*) request_chunk(power+1);
    if (bigger_chunk == NULL) return NULL;

    // Split the chunk into two
    node* buddy_one = bigger_chunk;
    // Calculate the address of the buddy using addr XOR 2^k, where k is the size of the block
    node* buddy_two = (node*) (((char *)buddy_one)+ (size_t) pow(2,power));

    // Reset the node information 
    reset_node(buddy_one, power);
    reset_node(buddy_two, power);

    // add one of the buddies to the freelist
    add_chunk(buddy_two, power);
    return buddy_one;
  }
}


/* Insert chunk at the front for its appropriate freelist
 * The freelist is determined by the `power`. A power of `x` means a freelist that is
 * storing chunks of size 2^x
 */
void add_chunk(node* buddy, int power){
    if (buddy == NULL) return;

    node** freelist = &(free_lists[MAX_ALLOCATION - power]);
    if (*freelist == NULL){
      // Add one of the chunks to the freelist for its size
      *freelist = buddy;
    } else{
      buddy->next = *freelist;
      (*freelist)->previous = buddy;
      *freelist = buddy;
    }
}

// Remove a node from its freelist
void remove_node(node* n){
  node* prev = n->previous;

  if (prev == NULL){
    free_lists[MAX_ALLOCATION - n->header.size] = n->next;
  } else{
    prev->next = n->next;
  }

  if (n->next != NULL) n->next->previous = prev;
}

/* Reset the node information */
void reset_node(node* buddy, int power){
  if (buddy == NULL) return;

  buddy->next = NULL;
  buddy->previous = NULL;
  buddy->header.size = power;
  buddy->header.free = true;
}

void* update_header_and_return_pointer(node* node){
    if (node == NULL) return NULL;

    // reset next, previous and set it to in use
    node->next = NULL;
    node->previous = NULL;
    node->header.free = false;
    return ((header*) node) + 1;
}