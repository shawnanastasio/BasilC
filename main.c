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
    if (str_index_of(cond, '=') > -1) {
        sign = '=';
    } else if (str_index_of(cond, '>') > -1) {
        sign = '>';
    } else if (str_index_of(cond, '<') > -1) {
        sign = '<';
    } else {
        exit_with_error("Invalid conditional!");
    }

    // Determine parameters
    char param_1[strlen(cond)];
    char param_2[strlen(cond)];
    if (split_string_delimiter(param_1, cond, ' ') <= -1) {
        exit_with_error("Invalid conditional!");
    } else if (split_string_delimiter_rev(param_2, cond, ' ') <= -1) {
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

int32_t str_index_of(char *str, char c) {
    int32_t i = 0;
    for (i=0; i<strlen(str); i++) {
        if (str[i] == c) return i;
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
int32_t split_string_delimiter(char *buf, char *str, char delim) {
    int32_t delim_pos = str_index_of(str, delim);
    if (delim_pos <= -1) return -1;
    strncpy(buf, str, delim_pos);
    return 0;
}

int32_t split_string_delimiter_rev(char *buf, char *str, char delim) {
    int32_t last_delim_pos = -1;
    int32_t i;
    for (i=0; i<strlen(str); i++) {
        if (str[i] == delim) last_delim_pos = i;
    }
    if (last_delim_pos <= -1) return -1;

    for (i=0; i<strlen(str) - last_delim_pos - 1; i++) {
        buf[i] = str[strlen(str)-i-1];
    }
    return 0;
}
