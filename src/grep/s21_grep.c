#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

#include "grep.h"
#include "options.h"
#include "process.h"

int main(int argc, char **argv) {
  // Устанавливаем бинарный режим для stdout
  SET_BINARY_MODE(stdout);

  Options opts;

  init_options(&opts);

  CompiledPatterns compiled;

  ProcessContext ctx = {.match_count = 0,
                        .line_num = 0,
                        .has_error = 0,
                        .filename = NULL,
                        .line_length = 0,
                        .compiled = &compiled};
  int parse_result = parse_options(argc, argv, &opts);

  if (parse_result == PARSE_HELP) {
    print_help(argv[0]);
    return EXIT_SUCCESS;
  }
  if (parse_result != PARSE_OK) {
    fprintf(stderr, "Try '%s --help' for more information.\n", argv[0]);
    free_pattern_options(&opts);
    return 2;
  }

  if (compile_patterns(&opts, ctx.compiled) != 0) {
    free_pattern_options(&opts);
    return 2;
  }

  int result;

  if (optind >= argc) {
    process_file(&opts, &ctx);
    result = (ctx.match_count > 0) ? 0 : 1;
    if (ctx.has_error) result = 2;
  } else {
    int file_count = argc - optind;
    // printf("file_count :%d\n", file_count );
    opts.multiple_files = (file_count > 1);
    result = process_files(argc, argv, optind, &opts, &ctx);
  }

  free_pattern_options(&opts);
  free_compiled_patterns(ctx.compiled);

  return result;
}
