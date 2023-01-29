#define FS_ALLOC_IMPLEMENTATION
#include "fs_alloc.h"

#include <stdio.h>
#include <string.h>

int main () {
  printf("Heap size: %lu\n", sizeof(fs_heap));
  printf("Tracker size: %lu\n", sizeof(fs_alloced_chunks));
  char *str1 = fs_alloc();
  char *str2 = fs_alloc();

  strncpy(str1, "Hello", CHUNK_SIZE - 1);
  strncpy(str2, "World!\n", CHUNK_SIZE - 1);

  printf("%s %s", str1, str2);

  fs_free(str1);
  fs_free(str2);
  return 0;
}
