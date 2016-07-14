/**
 * The BasilC Interpreter
 * Copyright (C) Shawn Anastasio 2016
 * Licensed under the GNU GPL v3
 */
 /**
  * This file contains code that defines interfaces for setting and declaring
  * language behavior.
  */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <main.h>
#include <cmd.h>
#include <stringhelpers.h>

registered_cmd_stack_t *root_cmd;
registered_cmd_stack_t *current_cmd_stack;

/**
 * Initalize the command stack
 */
void init_cmd_stack() {
    root_cmd = (registered_cmd_stack_t *) malloc(sizeof(registered_cmd_stack_t));
    current_cmd_stack = root_cmd;
}

/**
 * Register an interpreter-recognized command
 */
void register_cmd(cmd_declaration_t *dec) {
    // Copy provided data to internal command stack
    current_cmd_stack->name = dec->name;
    current_cmd_stack->num_args = dec->num_args;
    current_cmd_stack->handle_cmd = dec->handle_cmd;

    // Extend stack
    current_cmd_stack->next = (registered_cmd_stack_t *)
                              malloc(sizeof(registered_cmd_stack_t));
    current_cmd_stack = current_cmd_stack->next;
}

/**
 * Search for a command in the registered command stack
 * @param  label name of command
 * @return pointer to cmd stack node with name, or null if name isn't found
 */
registered_cmd_stack_t * cmd_stack_search_label(char *label) {
    registered_cmd_stack_t *cur = root_cmd;
    while (cur != NULL) {
        if (cur->name == NULL) break;

        if (strcmp(cur->name, label) == 0) {
            return cur;
        }
        cur = cur->next;
    }

    return NULL;
}

/**
 * Parse a user-inputted line and add to general stack if applicable
 */
bool parse_user_command(char *input, int32_t input_len) {
    // Skip empty strings
    if (input_len == 0) return true;

    // Skip shebang lines
    if (input[0] == '#') return true;

    // Skip comments
    if (input_len >= 3 && strncmp(input, "#//", 3) == 0) {
        return true;
    } else if (input_len >= 9 && strncmp(input, "BasilC#//", 9) == 0) {
        return true;;
    }

    // Determine whether command is prefixed with BasilC-
    bool has_prefix = false;
    if (strlen(input) > 6 && strncmp(input, "BasilC-", 7) == 0) {
        has_prefix = true;
    }

    // Search for command in registered command stack
    char cmd_name[strlen(input)];
    // Find first parenthesis
    int32_t paren = str_index_of(input, "(");
    if (paren == -1) {
        return false;
    }

    // Extract command name
    if (has_prefix) {
        strncpy(cmd_name, input+7, paren-7);
    } else {
        strncpy(cmd_name, input, paren);
    }

    // Search for command in registered command stack
    registered_cmd_stack_t *res = cmd_stack_search_label(cmd_name);
    if (res == NULL) {
        return false;
    }

    // Confirm number of given arguments with expected number
    int32_t num_args = get_char_occurances(input, ",");
    if (num_args != 0) num_args++;
    if (num_args != res->num_args) return false;

    // Extract arguments if applicable and add to general stack
    // TODO: Handle multiple arguments
    if (num_args == 1) {
        // Only one argument provided
        int32_t paren_end = str_index_of_skip(input, ")", paren);
        strncpy(current_stack->parameters[0],
                input+paren, paren_end-paren);
        current_stack->command = res->name;

        goto advance_stack;
    } else {
        // No arguments, add to stack
        current_stack->command = res->name;
        goto advance_stack;
    }

    return false;
advance_stack:
    if (strncmp(current_stack->command, "if", 2) != 0)
        current_stack->execute = !in_block;
    current_stack->next = malloc(sizeof(stack_node_t));
    stack_node_initialize(current_stack->next);
    current_stack = current_stack->next;
    return true;
}
