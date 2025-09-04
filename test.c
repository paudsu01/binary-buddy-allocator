#include "buddy.h"
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <math.h>

typedef struct{
  int id;
  char* name;
  int year;
} student;

int main() {
    printf("Testing malloc...\n");

    // Test 1: basic allocation
    char* p1 = malloc(pow(2, 10));
    assert(p1 != NULL);
    strcpy(p1, "hello");
    assert(strcmp(p1, "hello") == 0);
    printf("Test 1 passed: basic allocation\n");

    // Test 2: allocation of multiple blocks
    char* p2 = malloc(20);
    char* p3 = malloc(5);
    assert(p2 != NULL && p3 != NULL);
    strcpy(p2, "memory allocator");
    strcpy(p3, "hi");
    assert(strcmp(p2, "memory allocator") == 0);
    assert(strcmp(p3, "hi") == 0);
    printf("Test 2 passed: multiple allocations\n");

    // Test 3: free and reuse
    free(p1);
    char* p4 = malloc(8);
    assert(p4 != NULL);
    strcpy(p4, "reuse");
    assert(strcmp(p4, "reuse") == 0);
    printf("Test 3 passed: reuse freed memory\n");


    // Test 5: large allocation
    size_t large_size = 1024;
    char* p6 = malloc(large_size);
    assert(p6 != NULL);
    for (size_t i = 0; i < large_size; i++) p6[i] = (char)(i % 256);
    for (size_t i = 0; i < large_size; i++) assert(p6[i] == (char)(i % 256));
    printf("Test 5 passed: large allocation\n");

    printf("All tests passed!\n");
    return 0;
}
