#ifndef GREP_H
#define GREP_H
#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#define SET_BINARY_MODE(stream) _setmode(_fileno(stream), _O_BINARY)
#else
#define SET_BINARY_MODE(stream) ((void)0)
#endif
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum { PARSE_OK = 0, PARSE_ERROR = 1, PARSE_HELP = 2 } ParseResult;

typedef struct {
  char **patterns;         // Массив шаблонов
  int pattern_count;       // Количество шаблонов
  int pattern_capacity;    // Вместимость массива
  char *pattern_file;      // Имя файла с шаблонами
  int regexp;              // -e
  int ignore_case;         // Флаг -i
  int invert_match;        // флаг -v
  int count;               // -c
  int files_with_matches;  // l
  int line_number;         //-n
  int no_filename;         // -h
  int no_messages;         // -s
  int pattern_from_file;   // флаг -f
  int only_matching;       // -o
  int multiple_files;
} Options;

typedef struct {
  regex_t *regexes;
  int regex_count;
  int has_empty_pattern;
} CompiledPatterns;

typedef struct {
  int line_length;
  int match_count;
  int line_num;
  char *filename;
  int has_error;
  CompiledPatterns *compiled;
} ProcessContext;

#endif
