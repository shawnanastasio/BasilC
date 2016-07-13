/**
 * The BasilC Interpreter
 * Copyright (C) Shawn Anastasio 2016
 * Licensed under the GNU GPL v3
 */
/**
 * This file contains a set of functions designed to make string manipulation
 * easier.
 */

#include <stdint.h>
#include <string.h>

#include <stringhelpers.h>

/**
 * Shift a string `n` times to the left starting at index `start`
 */
void shift_string_left(char *str, int32_t start, int32_t n) {
    int32_t i, j;
    // Shift entire string left by one space 'n' times
    for (i=0; i<n; i++) {
        // Shift entire string left by one space
        for (j=start; j<strlen(str); j++) {
             str[j] = str[j+1];
        }
    }
}

/**
 * Get the number of occurances of substring `c` in `str`
 */
int32_t get_char_occurances(char *str, char *c) {
    int32_t counter = 0;
    int32_t i;
    for (i=0; i<strlen(str); i++) {
        if (strncmp(str+i, c, strlen(c)) == 0) counter++;
    }
    return counter;
}

/**
 * Returns the all chars in a string up until `delim` into `buf`
 */
int32_t split_string_delimiter(char *buf, char *str, char *delim) {
    int32_t delim_pos = str_index_of(str, delim);
    if (delim_pos <= -1) return -1;
    strncpy(buf, str, delim_pos);
    return 0;
}

/**
 * Returns all chars in a string from the end up until `delim` into `buf`
 */
int32_t split_string_delimiter_rev(char *buf, char *str, char *delim) {
    int32_t last_delim_pos = -1;
    int32_t i;
    for (i=0; i<strlen(str); i++) {
        if (strncmp(str+i, delim, strlen(delim)) == 0) last_delim_pos = i;
    }
    if (last_delim_pos <= -1) return -1;

    for (i=0; i<strlen(str) - last_delim_pos - 1; i++) {
        buf[i] = str[strlen(str)-i-1];
    }
    return 0;
}

/**
 * Index of the first occurance of `c` in `str`
 */
int32_t str_index_of(char *str, char *c) {
    int32_t i;
    for (i=0; i<strlen(str); i++) {
        if (strncmp(str+i, c, strlen(c)) == 0) return i;
    }
    return -1;
}

/**
 * Index of the `n`th occurance of `c` in `str`
 */
int32_t str_index_of_n(char *str, char *c, int32_t n) {
    int32_t count = 0;
    int32_t i;
    for (i=0; i<strlen(str); i++) {
        if (strncmp(str+i, c, strlen(c)) == 0 && count++ == n) return i;
    }
    return -1;
}

/**
 * Index of the first occurance of 'c' in 'str' after 'skip'
 */
int32_t str_index_of_skip(char *str, char *c, int32_t skip) {
    int32_t i;
    for (i=skip; i<strlen(str); i++) {
        if (strncmp(str+i, c, strlen(c)) == 0) return i;
    }
    return -1;
}
