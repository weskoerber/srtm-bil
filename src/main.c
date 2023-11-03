#include "srtm/header.h"
#include "srtm/srtm.h"

#include <stdio.h>
#include <stdlib.h>

int main() {
  header header;
  header_parse_file("data/n32_w116_1arc_v3.hdr", &header);

  fprintf(stderr, "nrows: %u\n", header.nrows);
  fprintf(stderr, "ncols: %u\n", header.ncols);
  fprintf(stderr, "nbands: %u\n", header.nbands);
  fprintf(stderr, "nbits: %s\n", resolution_str[header.nbits]);
  fprintf(stderr, "byteorder: %s\n", byteorder_str[header.byteorder]);
  fprintf(stderr, "layout: %s\n", layout_str[header.layout]);
  fprintf(stderr, "skipbytes: %u\n", header.skipbytes);
  fprintf(stderr, "ulxmap: %.18f\n", header.ulxmap);
  fprintf(stderr, "ulymap: %.18f\n", header.ulymap);
  fprintf(stderr, "xdim: %.18f\n", header.xdim);
  fprintf(stderr, "ydim: %.18f\n", header.ydim);
  fprintf(stderr, "bandrowbytes: %u\n", header.bandrowbytes);
  fprintf(stderr, "totalrowbytes: %u\n", header.totalrowbytes);
  fprintf(stderr, "bandgapbytes: %u\n", header.bandgapbytes);
  fprintf(stderr, "pixeltype: %s\n", pixeltype_str[header.pixeltype]);
  fprintf(stderr, "nodata: %d\n", header.nodata);
}
