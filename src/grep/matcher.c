#include "matcher.h"

int compile_patterns(Options *opts, CompiledPatterns *compiled) {
  memset(compiled, 0, sizeof(CompiledPatterns));

  compiled->has_empty_pattern = 0;
  compiled->regex_count = 0;
  compiled->regexes = NULL;

  if (opts == NULL || opts->pattern_count == 0) {
    return 0;  // Успех, просто нет паттернов для компиляции
  }

  if (opts->patterns == NULL) {
    fprintf(stderr, "Error: No pattern to compile\n");
    return -1;
  }

  int non_empty_count = 0;
  for (int i = 0; i < opts->pattern_count; i++) {
    if (opts->patterns[i] != NULL && opts->patterns[i][0] != '\0') {
      non_empty_count++;
    } else {
      compiled->has_empty_pattern = 1;  // Обнаружен пустой паттерн
    }
  }

  // Если есть пустые паттерны, но нет непустых
  if (non_empty_count == 0) {
    // Только пустые паттерны - нечего компилировать
    compiled->regex_count = 0;
    compiled->regexes = NULL;
    return 0;
  }

  int cflags = 0;
  if (opts->ignore_case) {
    cflags |= REG_ICASE;
  }
  compiled->regexes = malloc(non_empty_count * sizeof(regex_t));
  if (compiled->regexes == NULL) {
    fprintf(stderr, "Error: Memory allocation failed\n");
    return -1;
  }

  compiled->regex_count = non_empty_count;

  int regex_index = 0;
  for (int i = 0; i < opts->pattern_count; i++) {
    // Пропускаем пустые паттерны
    if (opts->patterns[i] == NULL || opts->patterns[i][0] == '\0') {
      continue;
    }
    int err =
        regcomp(&compiled->regexes[regex_index], opts->patterns[i], cflags);
    if (err != 0) {
      char errbuf[256];
      regerror(err, &compiled->regexes[regex_index], errbuf, sizeof(errbuf));
      fprintf(stderr, "%s: %s:%d: Invalid regular expression\n",
              "/usr/bin/grep",  // имя программы (grep или ваша)
              opts->pattern_file,  // имя файла с паттернами
              i + 1);  // номер строки, где ошибка
      free_compiled_patterns(compiled);
      return 1;
    }
    regex_index++;
  }

  return 0;
}

void free_compiled_patterns(CompiledPatterns *compiled) {
  if (compiled->regexes) {
    for (int i = 0; i < compiled->regex_count; i++) {
      regfree(&compiled->regexes[i]);
    }
    free(compiled->regexes);
  }
}

int matches_any_pattern(CompiledPatterns *compiled, const char *search_start,
                        char **match_start, char **match_end) {
  regmatch_t pmatch[1];
  if (match_start) *match_start = NULL;
  if (match_end) *match_end = NULL;

  if (compiled == NULL) {
    return 1;
  }

  // Пустые паттерны обрабатываются отдельно
  if (compiled->has_empty_pattern) {
    if (match_start && match_end) {
      *match_start = (char *)search_start;
      *match_end = (char *)search_start;
    }
    return 0;
  }

  if (compiled->regex_count == 0) {
    return 1;
  }

  for (int i = 0; i < compiled->regex_count; i++) {
    if (regexec(&compiled->regexes[i], search_start, 1, pmatch, 0) == 0) {
      if (match_start && match_end) {
        *match_start = (char *)search_start + pmatch[0].rm_so;
        *match_end = (char *)search_start + pmatch[0].rm_eo;
      }
      // Возвращаем 0 даже для пустого совпадения
      return 0;
    }
  }

  return 1;
}
