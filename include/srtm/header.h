#pragma once

#include <stddef.h>
#include <stdint.h>

typedef enum {
  b1 = 1,
  b4 = 4,
  b8 = 8,
  b16 = 16,
  b32 = 32,
} resolution;

typedef enum {
  little_endian,
  big_endian,
} byteorder;

typedef enum {
  bil,
  bip,
  bsq,
} layout;

typedef enum {
  signedint,
  unsignedint,
} pixeltype;

typedef struct {
  uint32_t nrows;
  uint32_t ncols;
  uint32_t nbands;
  resolution nbits;
  byteorder byteorder;
  layout layout;
  uint32_t skipbytes;
  double ulxmap;
  double ulymap;
  double xdim;
  double ydim;
  uint32_t bandrowbytes;
  uint32_t totalrowbytes;
  uint32_t bandgapbytes;
  pixeltype pixeltype;
  int32_t nodata;
} header;

const header *const header_parse(const char *data);
