#ifndef HANDLERS_H
#define HANDLERS_H

#include "cat.h"

void handle_line_numbers(int ch, const Options *opts, ProcessContext *ctx);
int handle_squeeze_blank(int ch, const Options *opts, ProcessContext *ctx);
int handle_nonprint(int ch, const Options *opts, ProcessContext *ctx);
int handle_tabs(int ch, const Options *opts, ProcessContext *ctx);
void handle_line_ends(int ch, const Options *opts, ProcessContext *ctx);

#endif /* HANDLERS_H */