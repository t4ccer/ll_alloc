#ifndef LL_ALLOC_H
#define LL_ALLOC_H

#include <stddef.h>

void *ll_alloc(size_t size);
void ll_free(void *ptr);

#ifdef LL_ALLOC_DEBUG
void ll_debug();
#endif

#ifdef LL_ALLOC_IMPLEMENTATION

typedef struct ll_alloc_block {
  struct ll_alloc_block *next;
  size_t size;
  void *ptr;
} ll_alloc_block;

#define FS_CHUNK_SIZE sizeof(ll_alloc_block)

#define FS_ALLOC_IMPLEMENTATION
#include "fs_alloc.h"
#include <assert.h>

#ifndef LL_HEAP_SIZE
#define LL_HEAP_SIZE 640000
#endif

unsigned char ll_heap[LL_HEAP_SIZE];

ll_alloc_block *ll_alloc_root = NULL;

// TODO: Insert in the middle of the list
void *ll_alloc(size_t size) {
  void *ptr = ll_heap;

  if (ll_alloc_root == NULL) {
    ll_alloc_root = fs_alloc();
    ll_alloc_root->next = NULL;
    ll_alloc_root->size = size;
    ll_alloc_root->ptr = ptr;
    return ptr;
  } else {
    ll_alloc_block *curr_block = ll_alloc_root;
    while (1) {
      ptr += curr_block->size;

      if (curr_block->next == NULL) {
        curr_block->next = fs_alloc();
        curr_block->next->next = NULL;
        curr_block->next->size = size;
        curr_block->next->ptr = ptr;
        return ptr;
      }

      size_t space_between_blocks =
          curr_block->next->ptr - (curr_block->ptr + curr_block->size);
      if (space_between_blocks >= size) {
        ll_alloc_block *new_block = fs_alloc();
        new_block->next = curr_block->next;
        new_block->size = size;
        new_block->ptr = ptr;
        curr_block->next = new_block;
        return ptr;
      }

      curr_block = curr_block->next;
    }
  }
}

void ll_free(void *ptr) {
  ll_alloc_block *curr_block = ll_alloc_root;
  while (1) {
    if (curr_block->ptr == ptr) {
      assert(curr_block == ll_alloc_root);
      ll_alloc_block *next_block = curr_block->next;
      fs_free(ll_alloc_root);
      ll_alloc_root = next_block;
      return;
    } else if (curr_block->next->ptr == ptr) {
      fs_free(curr_block->next);
      curr_block->next = curr_block->next->next;
      return;
    }
    curr_block = curr_block->next;
  }
}

#ifdef LL_ALLOC_DEBUG
void ll_debug() {
  ll_alloc_block *curr_block = ll_alloc_root;
  size_t i = 0;
  while (curr_block != NULL) {
    printf("[ll_alloc_block: %ld (%p)] = {size = %ld, ptr = %p, next = %p}\n",
           i, curr_block, curr_block->size, curr_block->ptr, curr_block->next);
    i++;
    curr_block = curr_block->next;
  }
}
#endif

#endif
#endif
