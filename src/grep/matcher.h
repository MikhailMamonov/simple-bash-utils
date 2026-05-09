#ifndef MATCHER_H
#define MATCHER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "grep.h"

int compile_patterns(Options *opts, CompiledPatterns *compiled);
void free_compiled_patterns(CompiledPatterns *compiled);
int matches_any_pattern(CompiledPatterns *compiled, const char *search_start,
                        char **match_start, char **match_end);

#endif
