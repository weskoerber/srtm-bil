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

header *header_parse(const char *data);
bool header_parse_file(const char *filename, header *header);

extern const char *resolution_str[];
extern const char *byteorder_str[];
extern const char *layout_str[];
extern const char *pixeltype_str[];

extern const char header_str_nrows[];
extern const char header_str_ncols[];
extern const char header_str_nbands[];
extern const char header_str_nbits[];
extern const char header_str_byteorder[];
extern const char header_str_layout[];
extern const char header_str_skipbytes[];
extern const char header_str_ulxmap[];
extern const char header_str_ulymap[];
extern const char header_str_xdim[];
extern const char header_str_ydim[];
extern const char header_str_bandrowbytes[];
extern const char header_str_totalrowbytes[];
extern const char header_str_bandgapbytes[];
extern const char header_str_pixeltype[];
extern const char header_str_nodata[];
