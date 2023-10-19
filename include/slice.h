#pragma once

#include <stddef.h>

typedef struct {
  void *start, *end;
  size_t length;
} slice;

char *slice_to_string(const slice *const slice);
