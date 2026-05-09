#ifndef CAT_H
#define CAT_H
#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#define SET_BINARY_MODE(stream) _setmode(_fileno(stream), _O_BINARY)
#else
#define SET_BINARY_MODE(stream) ((void)0)
#endif
#include <stdio.h>
#include <stdlib.h>

typedef enum { PARSE_OK = 0, PARSE_ERROR = 1, PARSE_HELP = 2 } ParseResult;

typedef struct {
  int number;
  int number_nonblank;
  int squeeze_blank;
  int show_ends;
  int show_nonprinting;
  int show_tabs;
} Options;

typedef struct {
  int new_line;
  int line_num;
  int prev_blank;
  int skip_output;
  FILE *output;
} ProcessContext;

#endif
