#pragma once

#include <stddef.h>

typedef struct {
  void *start, *end;
  size_t length;
} slice;

char *slice_to_string(const slice *slice);

long slice_cmp(const slice *s1, const slice *s2);
long slice_cmp_str(const slice *s, const char *str);

slice *slice_toupper(slice *s);
slice *slice_tolower(slice *s);
