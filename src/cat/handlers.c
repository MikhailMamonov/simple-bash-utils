
#include "handlers.h"

#include <stdio.h>
#include <stdlib.h>

void handle_line_ends(int ch, const Options *opts, ProcessContext *ctx) {
  if (ch == '\n' && opts->show_ends) {
    if (!opts->squeeze_blank || !ctx->skip_output) {
      fprintf(ctx->output, "$");
    }
  }
}

int handle_tabs(int ch, const Options *opts, ProcessContext *ctx) {
  if (opts->show_tabs && !opts->show_nonprinting && ch == '\t') {
    fprintf(ctx->output, "^I");
    return 1;
  }

  return 0;
}

int handle_nonprint(int ch, const Options *opts, ProcessContext *ctx) {
  if (opts->show_nonprinting) {
    if (ch == '\t') {
      if (opts->show_tabs) {
        fprintf(ctx->output, "^I");
        return 1;
      }
    } else if (ch == '\r') {
      fprintf(ctx->output, "^M");
      return 1;
    } else if (ch == '\n') {
      return 0;
    } else if (ch < 32) {
      fprintf(ctx->output, "^%c", ch + 64);
      return 1;
    } else if (ch == 127) {
      fprintf(ctx->output, "^?");
      return 1;
    } else if (ch > 127) {
      fputc(ch, ctx->output);  // В Linux просто выводим
      return 1;
    }
  }
  return 0;
}

int handle_squeeze_blank(int ch, const Options *opts, ProcessContext *ctx) {
  if (!opts->squeeze_blank) {
    ctx->skip_output = 0;
    return 0;
  }

  if (ch == '\n') {
    if (ctx->prev_blank == 0) {
      ctx->prev_blank = 0;  // Не увеличиваем!
      ctx->skip_output = 0;
      return 0;
    }
    // А если предыдущий символ был \n, то это пустая строка
    ctx->prev_blank++;
    if (ctx->prev_blank > 1) {
      ctx->skip_output = 1;
      return 0;
    }
    ctx->skip_output = 0;
    return 0;
  } else {
    ctx->prev_blank = 0;
    ctx->skip_output = 0;
    return 0;
  }
}

void handle_line_numbers(int ch, const Options *opts, ProcessContext *ctx) {
  if (ctx->new_line) {
    if (opts->number && !opts->number_nonblank) {
      fprintf(ctx->output, "%6d\t", ctx->line_num++);
      ctx->new_line = 0;
    } else if (opts->number_nonblank && ch != '\n') {
      fprintf(ctx->output, "%6d\t", ctx->line_num++);
      ctx->new_line = 0;
    } else {
      ctx->new_line = 0;
    }
  }
}
