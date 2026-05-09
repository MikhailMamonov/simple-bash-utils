#include "options.h"

#include <getopt.h>
#include <stdio.h>

void init_options(Options *opts) {
  opts->number = 0;
  opts->number_nonblank = 0;
  opts->squeeze_blank = 0;
  opts->show_ends = 0;
  opts->show_nonprinting = 0;
  opts->show_tabs = 0;
}

void print_help(const char *prog) {
  printf("Usage: %s [OPTIONS] [FILES...] [> OUTPUT] [>> OUTPUT]\n", prog);
  printf("Concatenate files to standard output or to a file.\n\n");
  printf("Options:\n");
  printf("  -h, --help     Show this help\n");
  printf(
      "  -v, --show-nonprinting   Displays non-printing characters using ^ "
      "and "
      "M- notation (except for Line Feeds and Tabs).\n");
  printf("  -b, --number-nonblank   Number non-empty lines; overrides -n.\n");
  printf(
      "  -E, --show-ends  Displays a $ character at the end of every "
      "line.\n");
  printf("  -n, --number  Number all output lines.\n\n");
  printf(
      "  -s, --squeeze-blank Suppress repeated empty lines (replaces "
      "multiple "
      "with one).\n");
  printf("  -t, --show-tabs     Show tabs\n");
  printf("Examples:\n");
  printf("  %s file1.txt file2.txt              # Output to terminal\n", prog);
  printf("  %s file1.txt file2.txt > out.txt    # Save to out.txt\n", prog);
  printf("  %s file3.txt >> out.txt             # Append to out.txt\n", prog);
  printf(
      "  %s -v file1.txt > all.txt               # Displays non-printing "
      "characters using ^ and M to all.txt\n",
      prog);
}

ParseResult parse_options(int argc, char **argv, Options *opts) {
  static struct option const long_options[] = {
      {"help", no_argument, 0, 'h'},
      {"number-nonblank", no_argument, 0, 'b'},
      {"show-ends", no_argument, 0, 'E'},
      {"show-ends", no_argument, 0, 'e'},
      {"number", no_argument, 0, 'n'},
      {"squeeze-blank", no_argument, 0, 's'},
      {"show-nonprinting", no_argument, 0, 'v'},
      {"show-tabs", no_argument, 0, 'T'},
      {0, 0, 0, 0}};

  int opt;
  while ((opt = getopt_long(argc, argv, "+hbeEnstuvT", long_options, 0)) !=
         -1) {
    switch (opt) {
      case 'h':
        print_help(argv[0]);
        return PARSE_HELP;
      case 'b':
        opts->number = 1;
        opts->number_nonblank = 1;
        break;
      case 'E':
        opts->show_ends = 1;
        break;
      case 'e':  // <-- ДОБАВИТЬ ЭТОТ case
        opts->show_ends = 1;
        opts->show_nonprinting = 1;
        break;
      case 'n':
        opts->number = 1;
        break;
      case 's':
        opts->squeeze_blank = 1;
        break;
      case 'T':
        opts->show_tabs = 1;
        break;
      case 't':
        opts->show_tabs = 1;
        opts->show_nonprinting = 1;
        break;
      case 'v':
        opts->show_nonprinting = 1;
        break;
      case '?':
        fprintf(stderr, "Unknown option. Use -h for help.\n");
        return 1;
      default:
        fprintf(stderr, "Error parsing options\n");
        return 1;
    }
  }
  return 0;
}
