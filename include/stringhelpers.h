#pragma once

#include <stdint.h>

void shift_string_left(char *str, int32_t start, int32_t n);
int32_t get_char_occurances(char *str, char *c);
int32_t split_string_delimiter(char *buf, char *str, char *delim);
int32_t split_string_delimiter_rev(char *buf, char *str, char *delim);
int32_t str_index_of(char *str, char *c);
int32_t str_index_of_n(char *str, char *c, int32_t n);
int32_t str_index_of_skip(char *str, char *c, int32_t skip);
