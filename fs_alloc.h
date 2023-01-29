#ifndef FS_ALLOC_H
#define FS_ALLOC_H

#define CHUNK_SIZE 64
#define MAX_CHUNKS 1024

void *ll_alloc ();
void ll_free (void *ptr);

#ifdef FS_ALLOC_IMPLEMENTATION
#include <stddef.h>

unsigned char heap[CHUNK_SIZE * MAX_CHUNKS] = {0};
unsigned char alloced_chunks[(MAX_CHUNKS + (sizeof(size_t) * 8) - 1) / (sizeof(size_t) * 8)] = {0};
size_t alloced_chunks_count = 0;

unsigned char is_allocated (size_t chunk_idx) {
  return alloced_chunks[chunk_idx/ (sizeof(size_t) * 8)] & (1ul << (chunk_idx % (sizeof(size_t) * 8)));
}

void *ll_alloc () {
  if (alloced_chunks_count >= MAX_CHUNKS) {
    return NULL;
  }

  for (int i = 0; i < MAX_CHUNKS; i++) {
    if (!is_allocated(i)) {
      alloced_chunks[i / (sizeof(size_t) * 8)] |= (1ul << (i % (sizeof(size_t) * 8)));
      alloced_chunks_count++;
      return &heap[i * CHUNK_SIZE];
    }
  }
  __builtin_unreachable();
}

void ll_free (void *ptr) {
  for (int i = 0; i < MAX_CHUNKS; i++) {
    if (ptr == &heap[i * CHUNK_SIZE]) {
      alloced_chunks[i / (sizeof(size_t) * 8)] &= ~(1ul << (i % (sizeof(size_t) * 8)));
      alloced_chunks_count--;
      return;
    }
  }
  __builtin_unreachable();
}
#endif
#endif
