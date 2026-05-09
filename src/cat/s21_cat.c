#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

#include "cat.h"
#include "options.h"
#include "process.h"

int main(int argc, char **argv) {
  // Устанавливаем бинарный режим для stdout и stdin
  SET_BINARY_MODE(stdout);
  SET_BINARY_MODE(stdin);

  Options opts;
  init_options(&opts);

  int parse_result = parse_options(argc, argv, &opts);
  if (parse_result == PARSE_HELP) {
    return EXIT_SUCCESS;  // Help выведен, завершаемся
  }
  if (parse_result != 0) {
    return EXIT_FAILURE;  // Ошибка или help уже выведен
  }

  FILE *output = stdout;
  int has_error = 0;

  if (optind >= argc) {
    ProcessContext ctx = {
        .new_line = 1, .line_num = 1, .prev_blank = 0, .output = output};
    process_file(stdin, &opts, &ctx);
  } else {
    has_error = process_files(argc, argv, optind, &opts, output);
  }

  return has_error ? EXIT_FAILURE : EXIT_SUCCESS;
}
