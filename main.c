/**
 * The BasilC Interpreter
 * Copyright (C) Shawn Anastasio 2016
 * Licensed under the GNU GPL v3
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "main.h"

// Comments: BasilC#// (comment)
// Loops: Basilc-loop(times)
// Print: BasilC-say()
// Set Printing Color: BasilC-tint(red)
// Label/Goto: BasilC-label(name), BasilC-goto(name)
// Sleep: BasilC-naptime(sec)
// System: BasilC-yolo(command)
// If: BasilC-if(condition)
// Endif: BasilC-endif()

stack_node_t *root;
stack_node_t *current_stack;

variable_stack_node_t *root_var;
// Must always point to latest empty node
variable_stack_node_t *current_var_stack;

bool in_block;

int32_t main(int32_t argc, char **argv) {
    // Verify arguments
    if (argc != 2) {
        printf("Usage: %s <script.basilc>\n", argv[0]);
        return 1;
    }

    // Set global variables
    in_block = false;

    // Create initial stack
    root = (stack_node_t *) malloc(sizeof(stack_node_t));
    current_stack = root;
    stack_node_initialize(root);

    // Create initial variable stack
    root_var = (variable_stack_node_t *) malloc(sizeof(variable_stack_node_t));
    current_var_stack = root_var;

    // Open script file
    FILE *fp;
    fp = fopen(argv[1], "r");
    if (fp == NULL) {
        perror("Error");
        return 1;
    }
    int32_t lines = 0;
    int32_t chars = 0;

    // Count lines and chars
    char cur_char = 0;
    while (!feof(fp)) {
        cur_char = fgetc(fp);
        if (cur_char == '\n') ++lines;
        chars++;
    }

    // Create Buffer with correct size
    char *buffer = 0;

    // Fill buffer
    if (fp) {
        fseek(fp, 0, SEEK_SET);
        buffer = malloc(chars+1);
        if (buffer) {
            fread (buffer, 1, chars, fp);
        }
        fclose(fp);
        buffer[chars] = '\0';
    }

    // DEBUG
    fputs("BasilC Interpreter v1.0\n\n", stderr);

    // Begin parsing
    cur_char = 0;

    int32_t line_len = 0;
    int32_t pos = 0;
    for (int32_t i=0; i<lines; i++) {
        // Get line length
        while (buffer[pos+line_len] && buffer[pos+line_len] != '\n') {
            ++line_len;
        }

        // Put line in temp buffer
        ++line_len;
        char temp[line_len];
        strncpy(temp, buffer+pos, line_len);

        // Parse line
        parse_line(temp, line_len, i+1);

        // Update position and reset line_len
        pos += line_len;
        line_len = 0;

    }

    // Cleanup and run final parsing checks
    parse_cleanup();

    // Execute stack
    stack_execute();
}

// Intialize an empty stack node
void stack_node_initialize(stack_node_t *s) {
    s->command = NULL;
    s->execute = true;
    for (int32_t i=0; i<STACK_PARAMETER_MAX_AMOUNT; i++) {
        for (int32_t z=0; z<STACK_PARAMETER_MAX_LENGTH; z++) {
            s->parameters[i][z] = 0;
        }
    }
    s->next = NULL;
}

// Parse line of code
void parse_line(char *line, int32_t line_len, int32_t linenum) {
    // Remove trailing \n
    if (line[--line_len] == '\n') line[line_len] = '\0';

    // Skip empty string
    if (line_len == 0) return;

    //printf("PARSE LINE: %s\n", line);
    //printf("line_len: %d\n", line_len);

    //ignore shebang lines
    if (line[0]=='#') return;

    // Parse line of code
    if (line_len <= 6) {
        // All BasilC code is prefixed with "BasilC"; line length must be > 6
        goto parse_fail;
    } if (strncmp(line, "BasilC", 6) != 0) {
        goto parse_fail;
    }

    // Check comment
    if (line_len >= 9 && strncmp(line, "BasilC#//", 9) == 0) {
        return;
    }

    // Check say()
    if (line_len >= 11 && strncmp(line, "BasilC-say(", 11) == 0) {
        // Make sure arguments are closed
        if (line[line_len-1] != ')') {
            goto parse_fail;
        }

        // Add to stack
        current_stack->command = "say";
        // If parameter is specified, add it
        if (line_len > 11+1) {
            strncpy(current_stack->parameters[0], line+11, line_len-11-1);
        }

        goto advance_stack;
    }

    /*
     * Check tint()
	 * The tint() function changes the text color to one of 8 ANSI terminal
	 * colors (TODO: add Bright versions of these colors)
	 */
    if (line_len >= 12 && strncmp(line, "BasilC-tint(", 12) == 0) {
        // Make sure arguments are closed
        if (line[line_len-1] != ')') {
            goto parse_fail;
        }

        // Add to stack
        current_stack->command = "tint";
        // If parameter is specified, add it
        if (line_len > 12+1) {
            strncpy(current_stack->parameters[0], line+12, line_len-12-1);
        }

        goto advance_stack;
    }

    // Check label()
    if (line_len >= 13 && strncmp(line, "BasilC-label(", 13) == 0) {
        // Make sure arguments are closed
        if (line[line_len-1] != ')') {
            goto parse_fail;
        }

        // Add to stack
        current_stack->command = "label";
        // If parameter is specified, add it
        if (line_len > 13+1) {
            strncpy(current_stack->parameters[0], line+13, line_len-13-1);
        } else {
            goto parse_fail;
        }

        goto advance_stack;
    }

    // Check goto()
    if (line_len >= 12 && strncmp(line, "BasilC-goto(", 12) == 0) {
        // Make sure arguments are closed
        if (line[line_len-1] != ')') {
            goto parse_fail;
        }

        // Add to stack
        current_stack->command = "goto";
        // If parameter is specified, add it
        if (line_len > 12+1) {
            strncpy(current_stack->parameters[0], line+12, line_len-12-1);
        } else {
            goto parse_fail;
        }

        goto advance_stack;
    }

    // Check yolo()
    if (line_len >= 12 && strncmp(line, "BasilC-yolo(", 12) == 0) {
        // Make sure arguments are closed
        if (line[line_len-1] != ')') {
            goto parse_fail;
        }

        // Add to stack
        current_stack->command = "yolo";
        // If parameter is specified, add it
        if (line_len > 12+1) {
            strncpy(current_stack->parameters[0], line+12, line_len-12-1);
        } else {
            goto parse_fail;
        }

        goto advance_stack;
    }

    // Check naptime()
    if (line_len >= 15 && strncmp(line, "BasilC-naptime(", 15) == 0) {
        // Make sure arguments are closed
        if (line[line_len-1] != ')') {
            goto parse_fail;
        }

        // Add to stack
        current_stack->command = "naptime";
        // If parameter is specified, add it
        if (line_len > 15+1) {
            strncpy(current_stack->parameters[0], line+15, line_len-15-1);
        } else {
            goto parse_fail;
        }

        goto advance_stack;
    }

    // Check if()
    if (line_len >= 10 && strncmp(line, "BasilC-if(", 10) == 0) {
        // Make sure arguments are closed
        if (line[line_len-1] != ')') {
            goto parse_fail;
        }

        // Add to stack
        current_stack->command = "if";
        // Store condition in parameters
        if (line_len > 10+1) {
            strncpy(current_stack->parameters[0], line+10, line_len-10-1);
            // Mark start of code block
            in_block = true;
        } else {
            goto parse_fail;
        }

        goto advance_stack;
    }

    // Check endif()
    if (line_len == 14 && strncmp(line, "BasilC-endif()", 14) == 0) {
        if (in_block) {
            in_block = false;
            current_stack->command = "endif";
        }
        else goto parse_fail;

        goto advance_stack;
    }

    // Check define()
    if (line_len >= 14 && strncmp(line, "BasilC-define(", 14) == 0) {
        if (line[line_len-1] != ')') {
            goto parse_fail;
        }

        // Add to stack
        current_stack->command = "define";
        // If size and parameters are correct
        if (line_len > 10+1 && str_index_of(line, ",") > -1) {
            int32_t comma = str_index_of(line, ",");
            //printf("line_len: %d\ncomma: %d\n", line_len, comma);
            strncpy(current_stack->parameters[0], line+14, comma-14);
            strncpy(current_stack->parameters[1], line+comma+2, line_len-comma-3);
            goto advance_stack;
        } else {
            goto parse_fail;
        }

        goto advance_stack;
    }

    // Check sayvar()
    if (line_len >= 14 && strncmp(line, "BasilC-sayvar(", 14) == 0) { // Make sure arguments are closed
        if (line[line_len-1] != ')') {
            goto parse_fail;
        }

        // Add to stack
        current_stack->command = "sayvar";
        // If parameter is specified, add it
        if (line_len > 15) {
            strncpy(current_stack->parameters[0], line+14, line_len-15);
        } else {
            goto parse_fail;
        }
        goto advance_stack;
    }

parse_fail:
    fprintf(stderr, "Invalid command\n");
    fprintf(stderr, "At line %d: %s\n", linenum, line);
    exit(1);
    return;

advance_stack:
    if (strncmp(current_stack->command, "if", 2) != 0)
        current_stack->execute = !in_block;
    current_stack->next = malloc(sizeof(stack_node_t));
    stack_node_initialize(current_stack->next);
    current_stack = current_stack->next;
    return;
}

void parse_cleanup() {
    // Check for unclosed if statement blocks
    if (in_block) {
        exit_with_error("Unclosed if statement!");
    }
}

void stack_execute() {
    stack_node_t *cur = root;
    while (cur != NULL) {
        // Handle commands
        if (cur->command == NULL) goto loop_next;

        // Handle nodes marked as "don't execute"
        if (cur->execute == false) goto loop_next;

        // say()
        if (strcmp(cur->command, "say") == 0) {
            char *temp = cur->parameters[0];
            // Try to parse variables in string
            char *parsed = parse_var_string(temp);
            if (parsed != NULL) {
                // If variables were parsed successfully
                printf("%s\n", parsed);
                free(parsed);
                goto loop_next;
            }
            printf("%s\n", temp);
            goto loop_next;
        }

        // tint()
        if (strcmp(cur->command, "tint") == 0) {
            char *temp = cur->parameters[0];
            /* prints ANSI escape code to allow the following BasilC-say
            statement to be in the corresponding color */
            if (strcmp(temp, "black") == 0)
                printf("\033[30m");
            else if (strcmp(temp, "red") == 0)
                printf("\033[31m");
            else if (strcmp(temp, "green") == 0)
                printf("\033[32m");
            else if (strcmp(temp, "yellow") == 0)
                printf("\033[33m");
            else if (strcmp(temp, "blue") == 0)
                printf("\033[34m");
            else if (strcmp(temp, "magenta") == 0)
                printf("\033[35m");
            else if (strcmp(temp, "cyan") == 0)
                printf("\033[36m");
            else if (strcmp(temp, "white") == 0)
                printf("\033[37m");
            /* if the color is not one of the available options, reset
            terminal to default color state */
            else
                printf("\033[0m");
            goto loop_next;
        }

        // goto()
        if (strcmp(cur->command, "goto") == 0) {
            char *temp = cur->parameters[0];
            stack_node_t *label = stack_search_label(temp);
            if (label != NULL) {
                cur = label;
                goto loop_skip;
            }
        }

        // yolo()
        if (strcmp(cur->command, "yolo") == 0) {
            char *temp = cur->parameters[0];
            system(temp);
            goto loop_next;
        }

        // naptime()
        if (strcmp(cur->command, "naptime") == 0) {
            char *temp = cur->parameters[0];
            sleep(atoi(temp));
            goto loop_next;
        }

        // if()
        if (strcmp(cur->command, "if") == 0) {
            char *temp = cur->parameters[0];
            bool cond = eval_conditional(temp);
            //printf("[debug] Conditional evaluated to: %d\n", (int8_t)cond);
            // If condition is true, mark all commands in block as execute
            if (cond)
                set_block_execute(cur, true);
            goto loop_next;
        }

        // define()
        if (strcmp(cur->command, "define") == 0) {
            char *var_name = cur->parameters[0];
            char *var_data = cur->parameters[1];

            // Add to variable stack
            strcpy(current_var_stack->name, var_name);
            strcpy(current_var_stack->data, var_data);

            // Advance variable stack
            current_var_stack->next = (variable_stack_node_t *)
                                      malloc(sizeof(variable_stack_node_t));
            current_var_stack = current_var_stack->next;

            goto loop_next;
        }

        // sayvar()
        if (strcmp(cur->command, "sayvar") == 0) {
            char *temp = get_data_for_var(cur->parameters[0]);
            if (temp != NULL) {
                printf("%s\n", temp);
                goto loop_next;
            }
            printf("null\n");
            goto loop_next;
        }

    loop_next:
        cur = cur->next;

    loop_skip:
        cur = cur; // Make C compiler happy
    }
}

/**
 * Search for label in stack
 * @param  label name of label
 * @return pointer to stack node with label, or NULL if label isn't found
 */
stack_node_t * stack_search_label(char *label) {
    stack_node_t *cur = root;
    while (cur != NULL) {
        if (cur->command == NULL) break;

        if (strcmp(cur->command, "label") == 0) {
            char *temp = cur->parameters[0];
            if (strcmp(temp, label) == 0) {
                return cur;
            }
        }
        cur = cur->next;
    }

    return NULL;
}

void set_block_execute(stack_node_t *cur, bool val) {
    while (cur != NULL) {
        if (cur->command == NULL) break;
        if (strncmp(cur->command, "endif", 5) == 0) break;

        cur->execute = val;
        cur = cur->next;
    }
}

bool eval_conditional(char *cond) {
    // Determine sign
    char sign = '\0';
    if (str_index_of(cond, "=") > -1) {
        sign = '=';
    } else if (str_index_of(cond, ">") > -1) {
        sign = '>';
    } else if (str_index_of(cond, "<") > -1) {
        sign = '<';
    } else {
        exit_with_error("Invalid conditional!");
    }

    // Determine parameters
    char param_1[strlen(cond)];
    char param_2[strlen(cond)];
    if (split_string_delimiter(param_1, cond, " ") <= -1) {
        exit_with_error("Invalid conditional!");
    } else if (split_string_delimiter_rev(param_2, cond, " ") <= -1) {
        exit_with_error("Invalid conditional!");
    }

    // Evaluate
    if (sign == '=') {
        if (atoi(param_1) == atoi(param_2)) return true;
        return false;
    } else if (sign == '>') {
        if (atoi(param_1) > atoi(param_2)) return true;
        return false;
    } else if (sign == '<') {
        if (atoi(param_1) < atoi(param_2)) return true;
        return false;
    } else {
        exit_with_error("Invalid conditional!");
    }
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

void exit_with_error(char *error) {
    fprintf(stderr, "[error] %s\n", error);
    exit(1);
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

int32_t get_char_occurances(char *str, char *c) {
    int32_t counter = 0;
    int32_t i;
    for (i=0; i<strlen(str); i++) {
        if (strncmp(str+i, c, strlen(c)) == 0) counter++;
    }
    return counter;
}

char * get_data_for_var(char *var_name) {
    variable_stack_node_t *cur = root_var;
    while (cur != NULL) {
        if (strcmp(cur->name, var_name) == 0) {
            return cur->name;
        }
        cur = cur->next;
    }
    return NULL;
}

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
 * Translates all variables prefixed with $ to "%s" to be parsed later on with
 * sprintf.
 */
void prepare_var_string(char *str, int32_t num_vars) {
    int32_t i;
    for (i=0; i<num_vars; i++) {
        int32_t cur_var_index = str_index_of(str, "$");
        int32_t cur_var_end_index = str_index_of_skip(str, " ", cur_var_index);
        if (cur_var_end_index <= -1) cur_var_end_index = strlen(str);
        int32_t cur_var_length = cur_var_end_index - cur_var_index;
        shift_string_left(str, cur_var_index, cur_var_length-2); // 2 for %s
        str[cur_var_index] = '%';
        str[cur_var_index+1] = 's';
    }
}

/**
 * Parses a string containing variables (prefixed with $) into its actual value
 * Returns a full string with variables parsed (must be freed after use)
 * Returns NULL if string contains nonexistent variables
 */
char * parse_var_string(char *str) {
    // Get number of variables
    int32_t num_vars = get_char_occurances(str, "$");

    // Determine size of buffer
    int32_t bufsize = strlen(str) + (MAX_DATA_SIZE * num_vars) - num_vars;
    char *buf = malloc(bufsize);

    // Array of var values in order
    char var_values[num_vars][MAX_DATA_SIZE];
    int32_t var_values_size = 0;

    int32_t i;
    for (i=0; i<num_vars; i++) {
        int32_t cur_var_index = str_index_of_n(str, "$", i);
        int32_t cur_var_end_index = str_index_of_skip(str, " ", cur_var_index);
        if (cur_var_end_index <= -1) cur_var_end_index = strlen(str);
        int32_t cur_var_length = cur_var_end_index - cur_var_index;
        char cur_var[MAX_DATA_SIZE];
        memset(cur_var, '\0', MAX_DATA_SIZE);
        strncpy(cur_var, str+cur_var_index+1, cur_var_length-1);
        // Get data for this var
        char* cur_var_data = get_data_for_var(cur_var);
        if (cur_var_data == NULL) return NULL;

        // Store in Array
        strcpy(var_values[var_values_size++], cur_var_data);
    }

    // Get sprintf format string
    char formatstr[strlen(str)+1];
    strcpy(formatstr, str);
    prepare_var_string(formatstr, num_vars);

    // Get final parsed string
    sprintf(buf, formatstr, var_values);
    return buf;
}
