#include "process.h"

#include <stdio.h>

#include "handlers.h"

void process_character(int ch, const Options *opts, ProcessContext *ctx) {
  // Обработка сжатия пустых строк
  handle_squeeze_blank(ch, opts, ctx);

  // Вывод с номерами строк
  handle_line_numbers(ch, opts, ctx);

  int special_handled = 0;
  special_handled = handle_nonprint(ch, opts, ctx);

  // Обработка табуляций (-T)
  if (!special_handled) {
    special_handled = handle_tabs(ch, opts, ctx);
  }

  // Вывод символа конца строки (-E, -e)
  handle_line_ends(ch, opts, ctx);

  if (!special_handled && !ctx->skip_output) {
    fputc(ch, ctx->output);
    if (ch == '\n') {
      fflush(ctx->output);
    }
  }

  if (ch == '\n') {
    ctx->new_line = 1;
  }
}

void process_file(FILE *file, const Options *opts, ProcessContext *ctx) {
  int ch;
  while ((ch = fgetc(file)) != EOF) {
    process_character(ch, opts, ctx);
  }
}

int process_files(int argc, char **argv, int start_index, const Options *opts,
                  FILE *output) {
  ProcessContext ctx = {.new_line = 1,
                        .line_num = 1,
                        .prev_blank = 0,
                        .output = output,
                        .skip_output = 0};

  int has_error = 0;

  for (int i = start_index; i < argc; i++) {
    FILE *file = fopen(argv[i], "rb");
    if (file == NULL) {
      fprintf(stderr, "Cannot open: %s\n", argv[i]);
      has_error = 1;  // Флаг ошибки
      continue;
    }

    process_file(file, opts, &ctx);

    fclose(file);
  }
  return has_error ? 1 : 0;
}
