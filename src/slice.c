#include "slice.h"

#include <stdlib.h>
#include <string.h>

char *slice_to_string(const slice *const slice) {
  char *str = (char *)malloc(slice->length + 1);

  memcpy(str, slice->start, slice->length);

  str[slice->length] = 0;

  return str;
}
