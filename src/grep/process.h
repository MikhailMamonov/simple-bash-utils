#ifndef PROCESS_H
#define PROCESS_H

#include "grep.h"
#include "matcher.h"

void print_only_matching(const char *line, ProcessContext *ctx);
void handle_match(const char *line, const Options *opts, ProcessContext *ctx);
int process_files(int argc, char **argv, int start_index, const Options *opts,
                  ProcessContext *ctx);
void process_file(const Options *opts, ProcessContext *ctx);
void process_line(char *line, const Options *opts, ProcessContext *ctx);

#endif /* PROCESS_H */
