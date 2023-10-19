#include "srtm/header.h"

#include "slice.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  const char *const key;
  bool (*const parse)(header *const);
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

bool header_parse_nrows(header *header);

const header_proc_map header_proc_map_[] = {
  {header_fields_.nrows, header_parse_nrows},
};

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
      if (!isspace(*cur)) {
        cur++;
        continue;
      }

      value.end = cur;
      value.length = value.end - value.start;
      break;
    }

    char *str_key = slice_to_string(&key);
    char *str_value = slice_to_string(&value);

    printf("%s: %s\n", str_key, str_value);

    free(str_key);
    free(str_value);

    nl_tok = strtok(NULL, "\n");
  }

  return new_header;
}

bool header_parse_nrows(header *header) {
}
