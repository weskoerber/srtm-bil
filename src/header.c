#include "srtm/header.h"

#include "slice.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  const char *const key;
  bool (*const parse)(slice *, header *const);
} header_proc_map;

typedef struct {
  const char *const nrows;
  const char *const ncols;
  const char *const nbands;
  const char *const nbits;
  const char *const byteorder;
  const char *const layout;
  const char *const skipbytes;
  const char *const ulxmap;
  const char *const ulymap;
  const char *const xdim;
  const char *const ydim;
  const char *const bandrowbytes;
  const char *const totalrowbytes;
  const char *const bandgapbytes;
  const char *const pixeltype;
  const char *const nodata;
} header_fields;

const header_fields header_fields_ = {
  .nrows = "NROWS",
  .ncols = "NCOLS",
  .nbands = "NBANDS",
  .nbits = "NBITS",
  .byteorder = "BYTEORDER",
  .layout = "LAYOUT",
  .skipbytes = "SKIPBYTES",
  .ulxmap = "ULXMAP",
  .ulymap = "ULYMAP",
  .xdim = "XDIM",
  .ydim = "YDIM",
  .bandrowbytes = "BANDROWBYTES",
  .totalrowbytes = "TOTALROWBYTES",
  .bandgapbytes = "BANDGAPBYTES",
  .pixeltype = "PIXELTYPE",
  .nodata = "NODATA",
};

const header *const header_parse(const char *data);

uint32_t header_parse_uint32(slice *data);
int32_t header_parse_int32(slice *data);
double header_parse_double(slice *data);

bool header_parse_byteorder(slice *data, header *header);
bool header_parse_nrows(slice *data, header *header);

const header_proc_map header_proc_map_[] = {
  {header_fields_.byteorder, header_parse_byteorder},
  {header_fields_.nrows,     header_parse_nrows    },
};
const size_t num_header_procs =
  sizeof(header_proc_map_) / sizeof(header_proc_map_[0]);

const header *const header_parse(const char *data) {
  header *new_header = (header *)malloc(sizeof(header));
  char *tmp_data = (char *)malloc(strlen(data) + 1);
  memcpy(tmp_data, data, strlen(data));

  char *nl_tok = strtok(tmp_data, "\n"), *cur = nl_tok;
  slice key = {0}, value = {0};
  while (nl_tok != nullptr) {
    cur = nl_tok;
    key.start = cur;
    while (cur) {
      if (!isspace(*cur)) {
        cur++;
        continue;
      }

      key.end = cur;
      key.length = key.end - key.start;
      break;
    }

    while (cur) {
      if (isspace(*cur)) {
        cur++;
        continue;
      }

      value.start = cur;
      break;
    }

    while (cur) {
      if (*cur && !isspace(*cur)) {
        cur++;
        continue;
      }

      value.end = cur;
      value.length = value.end - value.start;
      break;
    }

    for (size_t i = 0; i < num_header_procs; i++) {
      if (slice_cmp_str(&key, header_proc_map_[i].key) == 0) {
        header_proc_map_[i].parse(&value, new_header);
      }
    }

    nl_tok = strtok(NULL, "\n");
  }

  return new_header;
}

uint32_t header_parse_uint32(slice *data) {
  return strtoul((const char *)data->start, NULL, 10);
}

int32_t header_parse_int32(slice *data) {
  return strtol((const char *)data->start, NULL, 10);
}

double header_parse_double(slice *data) {
  return strtod((const char *)data->start, NULL);
}

bool header_parse_nrows(slice *data, header *header) {
  header->nrows = header_parse_uint32(data);

  fprintf(stderr, "Nrows: %u\n", header->nrows);

  return true;
}

bool header_parse_byteorder(slice *data, header *header) {
  bool status = false;

  if (slice_cmp_str(data, "I") == 0) {
    header->byteorder = little_endian;
    status = true;
  } else if (slice_cmp_str(data, "M") == 0) {
    header->byteorder = big_endian;
    status = true;
  }

  return status;
}
