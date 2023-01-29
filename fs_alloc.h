// Fixed size allocator in the single header format.

#ifndef FS_ALLOC_H
#define FS_ALLOC_H

void *fs_alloc();
void fs_free(void *ptr);

#ifdef FS_ALLOC_DEBUG
void fs_debug();
#endif // FS_ALLOC_DEBUG

#ifdef FS_ALLOC_IMPLEMENTATION
#include <assert.h>
#include <stddef.h>

// Define these yourself to override defaults
#ifndef FS_CHUNK_SIZE
#define FS_CHUNK_SIZE 64
#endif // FS_CHUNK_SIZE

#ifndef FS_MAX_CHUNKS
#define FS_MAX_CHUNKS 1024
#endif // FS_MAX_CHUNKS

unsigned char fs_heap[FS_CHUNK_SIZE * FS_MAX_CHUNKS];

// Used chunks are marked with bit 1, free chunks are marked with bit 0.
size_t fs_alloced_chunks[(FS_MAX_CHUNKS + (sizeof(size_t) * 8) - 1) /
                         (sizeof(size_t) * 8)] = {0};

// Make sure that we can keep track of all chunks
// * 8 to go bytes -> bits
static_assert(sizeof(fs_alloced_chunks) * 8 >= FS_MAX_CHUNKS,
              "fs_alloced_chunks is too small");

inline unsigned char is_allocated(size_t chunk_idx) {
  return fs_alloced_chunks[chunk_idx / (sizeof(size_t) * 8)] &
         (1ul << (chunk_idx % (sizeof(size_t) * 8)));
}

// Allocate a chunk of memory
// Returned memory is NOT zero initialized
// Returns NULL if no memory is available
void *fs_alloc() {
  for (int i = 0; i < FS_MAX_CHUNKS; i++) {
    if (!is_allocated(i)) {
      fs_alloced_chunks[i / (sizeof(size_t) * 8)] |=
          (1ul << (i % (sizeof(size_t) * 8)));
      return &fs_heap[i * FS_CHUNK_SIZE];
    }
  }
  // out of memory
  return NULL;
}

// Free a chunk of memory
void fs_free(void *ptr) {
  for (int i = 0; i < FS_MAX_CHUNKS; i++) {
    if (ptr == &fs_heap[i * FS_CHUNK_SIZE]) {
      fs_alloced_chunks[i / (sizeof(size_t) * 8)] &=
          ~(1ul << (i % (sizeof(size_t) * 8)));
      return;
    }
  }
  __builtin_unreachable();
}

#ifdef FS_ALLOC_DEBUG
// Print out the state of the heap
// Disabled by default to compile without stdlib as requires `printf`
void fs_debug() {
  for (int i = 0; i < FS_MAX_CHUNKS; i++) {
    if (is_allocated(i)) {
      printf("chunk %d is allocated\n", i);
    }
  }
}
#endif // FS_ALLOC_DEBUG

#endif // FS_ALLOC_IMPLEMENTATION
#endif // FS_ALLOC_H
