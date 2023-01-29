#ifndef FS_ALLOC_H
#define FS_ALLOC_H

void *fs_alloc();
void fs_free(void *ptr);

#ifdef FS_ALLOC_DEBUG
void fs_debug();
#endif

#ifdef FS_ALLOC_IMPLEMENTATION
#include <assert.h>
#include <stddef.h>

#ifndef FS_CHUNK_SIZE
#define FS_CHUNK_SIZE 64
#endif

#ifndef FS_MAX_CHUNKS
#define FS_MAX_CHUNKS 1024
#endif

unsigned char fs_heap[FS_CHUNK_SIZE * FS_MAX_CHUNKS] = {0};
size_t fs_alloced_chunks[(FS_MAX_CHUNKS + (sizeof(size_t) * 8) - 1) /
                         (sizeof(size_t) * 8)] = {0};
size_t fs_alloced_chunks_count = 0;

static_assert(sizeof(fs_alloced_chunks) * 8 >= FS_MAX_CHUNKS,
              "fs_alloced_chunks is too small");

unsigned char is_allocated(size_t chunk_idx) {
  return fs_alloced_chunks[chunk_idx / (sizeof(size_t) * 8)] &
         (1ul << (chunk_idx % (sizeof(size_t) * 8)));
}

void *fs_alloc() {
  if (fs_alloced_chunks_count >= FS_MAX_CHUNKS) {
    return NULL;
  }

  for (int i = 0; i < FS_MAX_CHUNKS; i++) {
    if (!is_allocated(i)) {
      fs_alloced_chunks[i / (sizeof(size_t) * 8)] |=
          (1ul << (i % (sizeof(size_t) * 8)));
      fs_alloced_chunks_count++;
      return &fs_heap[i * FS_CHUNK_SIZE];
    }
  }
  __builtin_unreachable();
}

void fs_free(void *ptr) {
  for (int i = 0; i < FS_MAX_CHUNKS; i++) {
    if (ptr == &fs_heap[i * FS_CHUNK_SIZE]) {
      fs_alloced_chunks[i / (sizeof(size_t) * 8)] &=
          ~(1ul << (i % (sizeof(size_t) * 8)));
      fs_alloced_chunks_count--;
      return;
    }
  }
  __builtin_unreachable();
}

#ifdef FS_ALLOC_DEBUG
void fs_debug() {
  for (int i = 0; i < FS_MAX_CHUNKS; i++) {
    if (is_allocated(i)) {
      printf("chunk %d is allocated\n", i);
    }
  }
}
#endif

#endif
#endif
