#include "slice.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *slice_to_string(const slice *const slice) {
  char *str = (char *)malloc(slice->length + 1);

  memcpy(str, slice->start, slice->length);

  str[slice->length] = 0;

  return str;
}

long slice_cmp(const slice *s1, const slice *s2) {
  size_t i = 0;
  for (; *(char *)(s1->start + i) == *(char *)(s2->start + i) &&
         i < s1->length - 1;
       i++)
    ;

  return *(unsigned char *)(s1->start + i) - *(unsigned char *)(s2->start + i);
}

long slice_cmp_str(const slice *s, const char *str) {
  size_t i = 0;
  for (; *(char *)(s->start + i) == *(str + i) && i < s->length - 1; i++)
    ;

  return *(unsigned char *)(s->start + i) - *(unsigned char *)(str + i);
}

slice *slice_toupper(slice *s) {
  for (size_t i = 0; i < s->length; i++) {
    *(char *)s->start = toupper(*(char *)s->start);
  }

  return s;
}

slice *slice_tolower(slice *s) {
  for (size_t i = 0; i < s->length; i++) {
    *(char *)s->start = tolower(*(char *)s->start);
  }

  return s;
}
