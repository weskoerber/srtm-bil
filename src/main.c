#include "srtm/header.h"
#include "srtm/srtm.h"

#include <stdio.h>
#include <stdlib.h>

int main() {
  FILE *hdr = fopen("data/n32_w116_1arc_v3.hdr", "rb");
  if (!hdr) {
    printf("fopen fail\n");
    return 1;
  }

  fseek(hdr, 0, SEEK_END);
  long size = ftell(hdr);
  char *buf = (char *)malloc(size);
  rewind(hdr); // fseek(hdr, 0, SEEK_SET);

  while (!feof(hdr)) {
    fread(buf, size, 1, hdr);
  }

  header_parse(buf);

  fclose(hdr);
  free(buf);
}
