#define _POSIX_C_SOURCE 200809L
#define MAX_LINE 4096
#include "process.h"

#include <stdio.h>

void print_only_matching(const char *line, ProcessContext *ctx) {
  if (!ctx->compiled) {
    return;
  }
  // Если нет скомпилированных паттернов (только пустые) - ничего не выводим
  if (ctx->compiled->has_empty_pattern && ctx->compiled->regex_count == 0) {
    return;
  }

  const char *search_start = line;
  char *match_start = NULL;
  char *match_end = NULL;

  while (1) {
    match_start = NULL;
    match_end = NULL;
    int ret = matches_any_pattern(ctx->compiled, search_start, &match_start,
                                  &match_end);
    if (ret != 0) {
      break;
    }

    if (match_start == NULL || match_end == NULL) {
      break;
    }

    int match_len = match_end - match_start;
    if (match_len > 0) {
      printf("%.*s\n", match_len, match_start);
      search_start = match_end;
    } else {
      // Пустое совпадение - двигаемся вперёд
      if (*search_start == '\0') break;
      search_start++;
    }

    if (*search_start == '\0') break;
  }
}

void handle_match(const char *line, const Options *opts, ProcessContext *ctx) {
  if (opts->multiple_files && !opts->no_filename && ctx->filename) {
    printf("%s:", ctx->filename);
  }
  if (opts->line_number) {
    printf("%d:", ctx->line_num);
  }
  if (opts->only_matching) {
    print_only_matching(line, ctx);
  } else {
    printf("%s\n", line);
  }
}

void process_line(char *line, const Options *opts, ProcessContext *ctx) {
  ctx->line_num++;

  line[strcspn(line, "\n")] = '\0';
  ctx->line_length = strlen(line);

  int has_match = 0;
  if (ctx->compiled) {
    if (ctx->compiled->regex_count > 0) {
      has_match = (matches_any_pattern(ctx->compiled, line, NULL, NULL) == 0);
    } else if (ctx->compiled->has_empty_pattern) {
      has_match = 1;
    }
  }

  int should_output = opts->invert_match ? !has_match : has_match;

  if (should_output) {
    ctx->match_count++;
    if (!opts->files_with_matches && !opts->count) {
      handle_match(line, opts, ctx);
    }
  }
}

void process_file(const Options *opts, ProcessContext *ctx) {
  if (opts->pattern_count == 0) {
    return;
  }
  FILE *file = stdin;
  char line[MAX_LINE];
  int close_file = 0;
  if (ctx->filename) {
    file = fopen(ctx->filename, "r");
    if (file == NULL) {
      if (!opts->no_messages) {
        fprintf(stderr, "Cannot open: %s\n", ctx->filename);
      }
      ctx->has_error = 1;
      return;
    }
    close_file = 1;
  }

  while (fgets(line, sizeof(line), file) != NULL) {
    ctx->line_length = strlen(line);
    process_line(line, opts, ctx);
    if (opts->files_with_matches && ctx->match_count > 0) {
      if (ctx->filename) {
        printf("%s\n", ctx->filename);
      } else {
        printf("(standard input)\n");
      }
      break;
    }
  }

  // Проверка ошибок чтения
  if (ferror(file) && !ctx->has_error) {
    if (!opts->no_messages) {
      fprintf(stderr, "s21_grep: error reading %s\n",
              ctx->filename ? ctx->filename : "standard input");
    }
    ctx->has_error = 1;
  }

  if (!opts->files_with_matches && opts->count) {
    int print_filename = (opts->multiple_files && !opts->no_filename);
    if (print_filename) {
      printf("%s:%d\n", ctx->filename, ctx->match_count);
    } else {
      printf("%d\n", ctx->match_count);
    }
  }

  if (close_file) {
    fclose(file);
  }

  if (ctx->filename == NULL) {
    fflush(stdout);
  }
}

int process_files(int argc, char **argv, int start_index, const Options *opts,
                  ProcessContext *ctx) {
  int has_error = 0;
  int total_match_count = 0;

  if (opts->pattern_count == 0) {
    return EXIT_FAILURE;
  }

  for (int i = start_index; i < argc; i++) {
    ctx->match_count = 0;
    ctx->line_num = 0;
    ctx->has_error = 0, ctx->filename = argv[i];
    ctx->line_length = 0;
    process_file(opts, ctx);
    total_match_count += ctx->match_count;

    if (ctx->has_error) {  // предполагая, что в ProcessContext есть поле
                           // has_error
      has_error = 1;
    }
  }

  if (has_error) return 2;
  return (total_match_count > 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
