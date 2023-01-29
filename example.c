#include <stdio.h>
#include <string.h>

#define LL_ALLOC_DEBUG
#define LL_ALLOC_IMPLEMENTATION
#include "ll_alloc.h"

int main() {
  char *str1 = ll_alloc(16);
  char *str2 = ll_alloc(32);
  char *str3 = ll_alloc(64);
  ll_debug();

  printf("----------------------------------------------------------------\n");

  ll_free(str2);
  ll_free(str1);
  ll_free(str3);
  ll_debug();
  return 0;
}
