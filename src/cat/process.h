#ifndef PROCESS_H
#define PROCESS_H

#include "cat.h"

int process_files(int argc, char **argv, int start_index, const Options *opts,
                  FILE *output);
void process_file(FILE *file, const Options *opts, ProcessContext *ctx);
void process_character(int ch, const Options *opts, ProcessContext *ctx);

#endif /* PROCESS_H */