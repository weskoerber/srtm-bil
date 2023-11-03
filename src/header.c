#include "srtm/header.h"

#include "slice.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *resolution_str[] = {
  [b1] = "1-bit",
  [b4] = "4-bit",
  [b8] = "8-bit",
  [b16] = "16-bit",
  [b32] = "32-bit",
};

const char *byteorder_str[] = {
  [little_endian] = "little_endian",
  [big_endian] = "big_endian",
};

const char *layout_str[] = {
  [bil] = "bil",
  [bip] = "bip",
  [bsq] = "bsq",
};

const char *pixeltype_str[] = {
  [signedint] = "signedint",
  [unsignedint] = "unsignedint",
};

typedef struct {
  const char *const key;
  bool (*const parse)(const slice *, header *const);
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

uint32_t header_parse_uint32(const slice *data);
int32_t header_parse_int32(const slice *data);
double header_parse_double(const slice *data);

bool header_parse_nrows(const slice *data, header *header);
bool header_parse_ncols(const slice *data, header *header);
bool header_parse_nbands(const slice *data, header *header);
bool header_parse_nbits(const slice *data, header *header);
bool header_parse_byteorder(const slice *data, header *header);
bool header_parse_layout(const slice *data, header *header);
bool header_parse_skipbytes(const slice *data, header *header);
bool header_parse_ulxmap(const slice *data, header *header);
bool header_parse_ulymap(const slice *data, header *header);
bool header_parse_xdim(const slice *data, header *header);
bool header_parse_ydim(const slice *data, header *header);
bool header_parse_bandrowbytes(const slice *data, header *header);
bool header_parse_totalrowbytes(const slice *data, header *header);
bool header_parse_bandgapbytes(const slice *data, header *header);
bool header_parse_pixeltype(const slice *data, header *header);
bool header_parse_nodata(const slice *data, header *header);

const header_proc_map header_proc_map_[] = {
  {header_fields_.nrows,         header_parse_nrows        },
  {header_fields_.ncols,         header_parse_ncols        },
  {header_fields_.nbands,        header_parse_nbands       },
  {header_fields_.nbits,         header_parse_nbits        },
  {header_fields_.byteorder,     header_parse_byteorder    },
  {header_fields_.layout,        header_parse_layout       },
  {header_fields_.skipbytes,     header_parse_skipbytes    },
  {header_fields_.ulxmap,        header_parse_ulxmap       },
  {header_fields_.ulymap,        header_parse_ulymap       },
  {header_fields_.xdim,          header_parse_xdim         },
  {header_fields_.ydim,          header_parse_ydim         },
  {header_fields_.bandrowbytes,  header_parse_bandrowbytes },
  {header_fields_.totalrowbytes, header_parse_totalrowbytes},
  {header_fields_.bandgapbytes,  header_parse_bandgapbytes },
  {header_fields_.pixeltype,     header_parse_pixeltype    },
  {header_fields_.nodata,        header_parse_nodata       },
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

  slice_toupper(&key);
  slice_toupper(&value);

  return new_header;
}

uint32_t header_parse_uint32(const slice *data) {
  return strtoul((const char *)data->start, NULL, 10);
}

int32_t header_parse_int32(const slice *data) {
  return strtol((const char *)data->start, NULL, 10);
}

double header_parse_double(const slice *data) {
  return strtod((const char *)data->start, NULL);
}

bool header_parse_nrows(const slice *data, header *header) {
  header->nrows = header_parse_uint32(data);

  return true;
}

bool header_parse_ncols(const slice *data, header *header) {
  header->ncols = header_parse_uint32(data);

  return true;
}

bool header_parse_nbands(const slice *data, header *header) {
  header->nbands = header_parse_uint32(data);

  return true;
}

bool header_parse_nbits(const slice *data, header *header) {
  bool status = false;

  if (slice_cmp_str(data, "1")) {
    header->nbits = b1;
    status = true;
  } else if (slice_cmp_str(data, "4")) {
    header->nbits = b4;
    status = true;
  } else if (slice_cmp_str(data, "8")) {
    header->nbits = b8;
    status = true;
  } else if (slice_cmp_str(data, "16")) {
    header->nbits = b16;
    status = true;
  } else if (slice_cmp_str(data, "32")) {
    header->nbits = b32;
    status = true;
  }

  return status;
}

bool header_parse_byteorder(const slice *data, header *header) {
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

bool header_parse_layout(const slice *data, header *header) {
  bool status = false;

  if (slice_cmp_str(data, "BIL")) {
    header->layout = bil;
    status = true;
  } else if (slice_cmp_str(data, "BIP")) {
    header->layout = bip;
    status = true;
  } else if (slice_cmp_str(data, "BSQ")) {
    header->layout = bsq;
    status = true;
  }

  return status;
}

bool header_parse_skipbytes(const slice *data, header *header) {
  header->skipbytes = header_parse_uint32(data);

  return true;
}

bool header_parse_ulxmap(const slice *data, header *header) {
  header->ulxmap = header_parse_double(data);

  return true;
}

bool header_parse_ulymap(const slice *data, header *header) {
  header->ulymap = header_parse_double(data);

  return true;
}

bool header_parse_xdim(const slice *data, header *header) {
  header->xdim = header_parse_double(data);

  return true;
}

bool header_parse_ydim(const slice *data, header *header) {
  header->ydim = header_parse_double(data);

  return true;
}

bool header_parse_bandrowbytes(const slice *data, header *header) {
  header->bandrowbytes = header_parse_uint32(data);

  return true;
}

bool header_parse_totalrowbytes(const slice *data, header *header) {
  header->totalrowbytes = header_parse_uint32(data);

  return true;
}

bool header_parse_bandgapbytes(const slice *data, header *header) {
  header->bandgapbytes = header_parse_uint32(data);

  return true;
}

bool header_parse_pixeltype(const slice *data, header *header) {
  bool status = false;

  if (slice_cmp_str(data, "SIGNEDINT")) {
    header->pixeltype = signedint;
    status = true;
  } else if (slice_cmp_str(data, "UNSIGNEDINT")) {
    header->pixeltype = unsignedint;
    status = true;
  }

  return status;
}

bool header_parse_nodata(const slice *data, header *header) {
  header->nodata = header_parse_int32(data);

  return true;
}
