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
#include <time.h>

#include <main.h>
#include <stringhelpers.h>
#include <cmd.h>
#include <libbasilc/libbasilc.h>

// Comments: BasilC#// (comment)
// Print: BasilC-say()
// Set Printing Color: BasilC-tint(red)
// Label/Goto: BasilC-label(name), BasilC-goto(name)
// Sleep: BasilC-naptime(sec)
// System: BasilC-yolo(command)
// If: BasilC-if(condition)
// Endif: BasilC-endif()
// Define variables: BasilC-define(var_name, var_data)
// End Program: BasilC-end()

stack_node_t *root;
stack_node_t *current_stack;

variable_stack_node_t *root_var;
variable_stack_node_t *current_var_stack;

bool in_block;
bool monochrome_mode;
bool hide_debugging;
bool show_timer;

int32_t main(int32_t argc, char **argv) {
    //start debug timer
    clock_t start_timer = clock();

    // Verify arguments
    if (argc < 2 || argc > 5) {
        printf("Usage: %s [-m] [-d] <script.basilc>\n", argv[0]);
        return 1;
    }

    // Set global variables
    in_block = false;
    monochrome_mode = false;
    hide_debugging = false;
    show_timer = false;

    // Initialize command stack
    init_cmd_stack();

    // Register libbasilc functions
    libbasilc_register();

    // Check parameters
    int32_t c;
    int32_t counter = 0;

    while ((c = find_option(argc, argv, "mdt", &counter)) != -1)
    switch (c) {
        case 'm':
            monochrome_mode = true; //don't output ANSI color codes
            break;
        case 'd':
            fclose(stderr); //don't show debugging and error info
            break;
        case 't':
            show_timer = true; //show elapse time
            break;
    }

    // Create initial stack
    root = (stack_node_t *) malloc(sizeof(stack_node_t));
    current_stack = root;
    stack_node_initialize(root);

    // Create initial variable stack
    root_var = (variable_stack_node_t *) malloc(sizeof(variable_stack_node_t));
    current_var_stack = root_var;

    // Open script file
    FILE *fp;
    fp = fopen(argv[argc-1], "r");
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
    int32_t i;
    for (i=0; i<lines; i++) {
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

    // Reset terminal colors
    printANSIescape("\033[0m");

    // Print program execution time
    clock_t end_timer = clock();
    double execution_time = (double)(end_timer - start_timer) / CLOCKS_PER_SEC;
    if (show_timer)
        printf("\nExecution Time: %f seconds\n", execution_time);

    return 0;
}

// Intialize an empty stack node
void stack_node_initialize(stack_node_t *s) {
    s->command = NULL;
    s->execute = true;
    int32_t i, z;
    for (i=0; i<STACK_PARAMETER_MAX_AMOUNT; i++) {
        for (z=0; z<STACK_PARAMETER_MAX_LENGTH; z++) {
            s->parameters[i][z] = 0;
        }
    }
    s->next = NULL;
}

// Parse line of code
void parse_line(char *line, int32_t line_len, int32_t linenum) {
    // Remove trailing \n
    if (line[--line_len] == '\n') line[line_len] = '\0';

    // Pass line to parser
    int32_t result = parse_user_command(line, line_len);
    if (result != ERR_SUCCESS) {
        printf("Error: %s\n", parse_error_msgs[result]);
    } else {
        return;
    }

parse_fail:
    fprintf(stderr, "At line %d: %s\n", linenum, line);
    exit(1);
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
    while (cur->next != NULL) {
        // Pass stack node to handler
        int32_t result = execute_command(&cur);
        if (result) {
            continue;
        } else {
            char error[80];
            sprintf(error, "Failed to execute command: %s", cur->command);
            exit_with_error(error);
        }
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

/**
 * Search for variable name in variable stack
 * @param  label name of variable
 * @return pointer to var stack node with name, or NULL if name isn't found
 */
variable_stack_node_t * var_stack_search_label(char *label) {
    variable_stack_node_t *cur = root_var;
    while (cur != NULL) {
        if (cur->name == NULL) break;

        if (strcmp(cur->name, label) == 0) {
            return cur;
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

void exit_with_error(char *error) {
    fprintf(stderr, "[error] %s\n", error);
    exit(1);
}

char * get_data_for_var(char *var_name) {
    variable_stack_node_t *cur = root_var;
    while (cur != NULL) {
        if (strcmp(cur->name, var_name) == 0) {
            return cur->data;
        }
        cur = cur->next;
    }
    return NULL;
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
    if (num_vars == 0) return NULL;

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

// wrapper around printf for ANSI escape codes
void printANSIescape(char *code){
    if (!monochrome_mode)
        printf("%s", code);
}
