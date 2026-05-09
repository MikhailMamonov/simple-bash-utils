#include "cat.h"

void init_options(Options *opts);
ParseResult parse_options(int argc, char **argv, Options *opts);
void print_help(const char *program_name);