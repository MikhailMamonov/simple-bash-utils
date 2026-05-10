#define _GNU_SOURCE
#include "options.h"

#include <getopt.h>
#include <stdio.h>
#include <string.h>

#define MAX_LINE 4096
#define INITIAL_PATTERN_CAPACITY 10

void init_options(Options *opts) {
  memset(opts, 0, sizeof(Options));

  opts->patterns = NULL;
  opts->pattern_count = 0;
  opts->pattern_capacity = 0;
  opts->pattern_file = NULL;
  opts->pattern_from_file = 0;
  opts->count = 0;
  opts->ignore_case = 0;
  opts->invert_match = 0;
  opts->files_with_matches = 0;
  opts->no_filename = 0;
  opts->no_messages = 0;
  opts->only_matching = 0;
  opts->multiple_files = 0;
}

void print_help(const char *program_name) {
  printf("Usage: %s [OPTIONS] PATTERN [FILE...]\n", program_name);
  printf("Options:\n");
  printf(
      "  -e PATTERN      Use PATTERN as a pattern (can be used multiple "
      "times)\n");
  printf("  -f FILE         Read patterns from FILE (one per line)\n");
  printf("  -c              Displays only the number of matching rows.\n");
  printf("  -l              Displays only file names that match.\n");
  printf("  -n              Shows line numbers.\n");
  printf("  -i              Ignore case distinctions\n");
  printf("  -o              Print only the matching parts\n");
  printf("  -h              Suppress filename prefix\n");
  printf("  -s              Suppress error messages\n");
  printf("  --help          Display this help\n");
}

void free_pattern_options(Options *opts) {
  if (opts->patterns) {
    for (int i = 0; i < opts->pattern_count; i++) {
      free(opts->patterns[i]);
      opts->patterns[i] = NULL;
    }
    free(opts->patterns);
    opts->patterns = NULL;
    opts->pattern_count = 0;
    opts->pattern_capacity = 0;
  }

  if (opts->pattern_file) {
    free(opts->pattern_file);
    opts->pattern_file = NULL;
  }
}

ParseResult add_pattern(Options *opts, const char *pattern) {
  if (!pattern) {
    return PARSE_ERROR;
  }

  if (opts->pattern_count >= opts->pattern_capacity) {
    int new_capacity = opts->pattern_capacity == 0 ? INITIAL_PATTERN_CAPACITY
                                                   : opts->pattern_capacity * 2;

    char **new_patterns =
        realloc(opts->patterns, new_capacity * sizeof(char *));
    if (!new_patterns) {
      return PARSE_ERROR;
    }

    opts->patterns = new_patterns;
    opts->pattern_capacity = new_capacity;
  }

  char *new_pattern = strdup(pattern);
  if (!new_pattern) {
    return PARSE_ERROR;
  }
  opts->patterns[opts->pattern_count] = new_pattern;

  opts->pattern_count++;
  return PARSE_OK;
}

ParseResult load_patterns_from_file(Options *opts, const char *pattern_file) {
  FILE *file = fopen(pattern_file, "r");
  if (!file) {
    return PARSE_ERROR;
  }

  int first_char = fgetc(file);
  if (first_char == EOF) {
    fclose(file);
    return PARSE_OK;
  }

  // Возвращаем первый символ обратно в поток
  ungetc(first_char, file);

  char line[MAX_LINE];
  int has_empty_line = 0;
  int has_non_empty_line = 0;
  int error_occurred = 0;

  while (fgets(line, sizeof(line), file) != NULL) {
    // Удаляем символ новой строки
    line[strcspn(line, "\n")] = '\0';

    if (strlen(line) > 0) {
      has_non_empty_line = 1;
      if (add_pattern(opts, line) == PARSE_ERROR) {
        error_occurred = 1;
        break;
      }
    } else {
      has_empty_line = 1;
    }
  }

  if (error_occurred) {
    fclose(file);
    // Очищаем добавленные паттерны
    for (int i = 0; i < opts->pattern_count; i++) {
      free(opts->patterns[i]);
    }
    free(opts->patterns);
    opts->patterns = NULL;
    opts->pattern_count = 0;
    opts->pattern_capacity = 0;
    return PARSE_ERROR;
  }

  // Если есть пустые строки, но нет непустых - добавляем один пустой паттерн
  if (!has_non_empty_line && has_empty_line) {
    add_pattern(opts, "");
  }

  fclose(file);
  return PARSE_OK;
}

ParseResult parse_options(int argc, char **argv, Options *opts) {
  static struct option const long_options[] = {
      {"regexp", required_argument, 0, 'e'},
      {"help", no_argument, 0, 'H'},
      {"count", no_argument, 0, 'c'},
      {"ignore-case", no_argument, 0, 'i'},
      {"invert-match", no_argument, 0, 'v'},
      {"line-number", no_argument, 0, 'n'},
      {"files-with-matches", no_argument, 0, 'l'},
      {"no-filename", no_argument, 0, 'h'},
      {"no-messages", no_argument, 0, 's'},
      {"file", required_argument, 0, 'f'},
      {"only-matching", no_argument, 0, 'o'},
      {0, 0, 0, 0}};

  int opt;
  while ((opt = getopt_long(argc, argv, "+e:f:Hcivnlhso", long_options,
                            NULL)) != -1) {
    switch (opt) {
      case 'e':
        if (add_pattern(opts, optarg) == PARSE_ERROR) {
          fprintf(stderr, "grep: failed to add pattern\n");
          return PARSE_ERROR;
        }
        break;
      case 'H':
        return PARSE_HELP;
      case 'c':
        opts->count = 1;
        break;
      case 'i':
        opts->ignore_case = 1;
        break;
      case 'v':
        opts->invert_match = 1;
        break;
      case 'n':
        opts->line_number = 1;
        break;
      case 'l':
        opts->files_with_matches = 1;
        break;
      case 'h':
        opts->no_filename = 1;
        break;
      case 's':
        opts->no_messages = 1;
        break;
      case 'f':
        opts->pattern_from_file = 1;
        opts->pattern_file = strdup(optarg);
        if (load_patterns_from_file(opts, optarg) == PARSE_ERROR) {
          fprintf(stderr, "grep: %s: No such file or directory\n", optarg);
          free(opts->pattern_file);
          return PARSE_ERROR;
        }
        break;
      case 'o':
        opts->only_matching = 1;
        break;
      case '?':
        fprintf(stderr, "Unknown option. Use -a for help.\n");
        return PARSE_ERROR;
      default:
        fprintf(stderr, "Error parsing options\n");
        return PARSE_ERROR;
    }
  }

  if (opts->pattern_count == 0 && !opts->pattern_from_file && optind < argc) {
    if (add_pattern(opts, argv[optind]) == PARSE_ERROR) {
      fprintf(stderr, "grep: failed to add pattern\n");
      return PARSE_ERROR;
    }
    optind++;
  }

  // printf("DEBUG parse_options: after getopt loop, optind = %d, argc = %d\n",
  // optind, argc);
  if (opts->pattern_count == 0 && !opts->pattern_from_file) {
    fprintf(stderr, "grep: No pattern specified\n");
    return PARSE_ERROR;
  }

  return PARSE_OK;
}
