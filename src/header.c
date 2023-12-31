#include "srtm/header.h"

#include "slice.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *resolution_str[] = {
  [b1] = "1",
  [b4] = "4",
  [b8] = "8",
  [b16] = "16",
  [b32] = "32",
};

const char *byteorder_str[] = {
  [little_endian] = "I",
  [big_endian] = "M",
};

const char *layout_str[] = {
  [bil] = "BIL",
  [bip] = "BIP",
  [bsq] = "BSQ",
};

const char *pixeltype_str[] = {
  [signedint] = "SIGNEDINT",
  [unsignedint] = "UNSIGNEDINT",
};

const char header_str_nrows[] = "NROWS";
const char header_str_ncols[] = "NCOLS";
const char header_str_nbands[] = "NBANDS";
const char header_str_nbits[] = "NBITS";
const char header_str_byteorder[] = "BYTEORDER";
const char header_str_layout[] = "LAYOUT";
const char header_str_skipbytes[] = "SKIPBYTES";
const char header_str_ulxmap[] = "ULXMAP";
const char header_str_ulymap[] = "ULYMAP";
const char header_str_xdim[] = "XDIM";
const char header_str_ydim[] = "YDIM";
const char header_str_bandrowbytes[] = "BANDROWBYTES";
const char header_str_totalrowbytes[] = "TOTALROWBYTES";
const char header_str_bandgapbytes[] = "BANDGAPBYTES";
const char header_str_pixeltype[] = "PIXELTYPE";
const char header_str_nodata[] = "NODATA";

typedef struct {
  const char *key;
  bool (*parse)(const slice *, header *);
} header_proc_map;

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

char *trim_key(slice *data, char *str);
char *trim_value(slice *data, char *str);
void parse_line(slice *key, slice *value, char *line);

const header_proc_map header_proc_map_[] = {
  {header_str_nrows,         header_parse_nrows        },
  {header_str_ncols,         header_parse_ncols        },
  {header_str_nbands,        header_parse_nbands       },
  {header_str_nbits,         header_parse_nbits        },
  {header_str_byteorder,     header_parse_byteorder    },
  {header_str_layout,        header_parse_layout       },
  {header_str_skipbytes,     header_parse_skipbytes    },
  {header_str_ulxmap,        header_parse_ulxmap       },
  {header_str_ulymap,        header_parse_ulymap       },
  {header_str_xdim,          header_parse_xdim         },
  {header_str_ydim,          header_parse_ydim         },
  {header_str_bandrowbytes,  header_parse_bandrowbytes },
  {header_str_totalrowbytes, header_parse_totalrowbytes},
  {header_str_bandgapbytes,  header_parse_bandgapbytes },
  {header_str_pixeltype,     header_parse_pixeltype    },
  {header_str_nodata,        header_parse_nodata       },
};
const size_t num_header_procs =
  sizeof(header_proc_map_) / sizeof(header_proc_map_[0]);

bool header_parse_file(const char *filename, header *header) {
  char *real_filename = NULL;
  char *filename_cpy = strdup(filename);

  if (filename_cpy[0] == '~') {
    filename_cpy[0] = '/';
    char *home = getenv("HOME");
    if (home == NULL) {
      fprintf(stderr, "Unable to retrieve 'HOME' environment variable\n");
      return false;
    }
    real_filename = malloc(strlen(home) + strlen(filename_cpy) + 2);
    strncpy(real_filename, home, strlen(home));
    strncpy(real_filename + strlen(home), filename_cpy, strlen(filename_cpy));
  } else {
    real_filename = filename_cpy;
  }

  FILE *hdr = fopen(real_filename, "rb");
  if (!hdr) {
    printf("fopen: failed opening file: %s\n", real_filename);
    return false;
  }

  fseek(hdr, 0, SEEK_END);
  long size = ftell(hdr);
  char *buf = (char *)malloc(size);
  rewind(hdr); // fseek(hdr, 0, SEEK_SET);

  while (!feof(hdr)) {
    fread(buf, size, 1, hdr);
  }

  *header = *header_parse(buf);

  fclose(hdr);
  free(buf);

  return header != NULL;
}

header *header_parse(const char *data) {
  header *new_header = (header *)malloc(sizeof(header));
  char *tmp_data = (char *)malloc(strlen(data) + 1);
  memcpy(tmp_data, data, strlen(data));

  char *nl_tok = strtok(tmp_data, "\n"), *cur = nl_tok;
  slice key = {0}, value = {0};
  while (nl_tok != nullptr) {
    cur = nl_tok;

    parse_line(&key, &value, cur);

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

  if (slice_cmp_str(data, resolution_str[b1])) {
    header->nbits = b1;
    status = true;
  } else if (slice_cmp_str(data, resolution_str[b4])) {
    header->nbits = b4;
    status = true;
  } else if (slice_cmp_str(data, resolution_str[b8])) {
    header->nbits = b8;
    status = true;
  } else if (slice_cmp_str(data, resolution_str[b16])) {
    header->nbits = b16;
    status = true;
  } else if (slice_cmp_str(data, resolution_str[b32])) {
    header->nbits = b32;
    status = true;
  }

  return status;
}

bool header_parse_byteorder(const slice *data, header *header) {
  bool status = false;

  if (slice_cmp_str(data, byteorder_str[little_endian]) == 0) {
    header->byteorder = little_endian;
    status = true;
  } else if (slice_cmp_str(data, byteorder_str[big_endian]) == 0) {
    header->byteorder = big_endian;
    status = true;
  }

  return status;
}

bool header_parse_layout(const slice *data, header *header) {
  bool status = false;

  if (slice_cmp_str(data, layout_str[bil])) {
    header->layout = bil;
    status = true;
  } else if (slice_cmp_str(data, layout_str[bip])) {
    header->layout = bip;
    status = true;
  } else if (slice_cmp_str(data, layout_str[bsq])) {
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

  if (slice_cmp_str(data, pixeltype_str[signedint])) {
    header->pixeltype = signedint;
    status = true;
  } else if (slice_cmp_str(data, pixeltype_str[unsignedint])) {
    header->pixeltype = unsignedint;
    status = true;
  }

  return status;
}

bool header_parse_nodata(const slice *data, header *header) {
  header->nodata = header_parse_int32(data);

  return true;
}

char *trim_key(slice *data, char *str) {
  data->start = str;

  while (str) {
    if (!isspace(*str)) {
      str++;
      continue;
    }

    data->end = str;
    data->length = data->end - data->start;
    break;
  }

  return str;
}

char *trim_value(slice *data, char *str) {
  while (str) {
    if (isspace(*str)) {
      str++;
      continue;
    }

    data->start = str;
    break;
  }

  while (str) {
    if (*str && !isspace(*str)) {
      str++;
      continue;
    }

    data->end = str;
    data->length = data->end - data->start;
    break;
  }

  return str;
}

void parse_line(slice *key, slice *value, char *line) {
  line = trim_key(key, line);
  line = trim_value(value, line);
}
