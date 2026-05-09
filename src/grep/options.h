#include "grep.h"

void init_options(Options *opts);
ParseResult add_pattern(Options *opts, const char *pattern);
void free_pattern_options(Options *opts);
ParseResult load_patterns_from_file(Options *opts, const char *pattern_file);
ParseResult parse_options(int argc, char **argv, Options *opts);
void print_help(const char *program_name);
