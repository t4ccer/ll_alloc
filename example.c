#define FS_ALLOC_IMPLEMENTATION
#include "fs_alloc.h"

#include <stdio.h>
#include <string.h>

int main () {
  printf("Heap size: %lu\n", sizeof(heap));
  printf("Tracker size: %lu\n", sizeof(alloced_chunks));
  char *str1 = ll_alloc();
  char *str2 = ll_alloc();

  strncpy(str1, "Hello", CHUNK_SIZE - 1);
  strncpy(str2, "World!\n", CHUNK_SIZE - 1);

  printf("%s %s", str1, str2);

  ll_free(str1);
  ll_free(str2);
  return 0;
}
