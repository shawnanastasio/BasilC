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

char parse_error_msgs[][32] = {
    "Success",
    "Missing parenthesis",
    "Invalid command",
    "Incorrect arguments",
    "Failed special parse"
};

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
    current_cmd_stack->special_parse = dec->special_parse;

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
int32_t parse_user_command(char *input, int32_t input_len) {
    // Skip empty strings
    if (input_len == 0) return ERR_SUCCESS;

    // Skip shebang lines
    if (input[0] == '#') return ERR_SUCCESS;

    // Skip comments
    if (input_len >= 3 && strncmp(input, "#//", 3) == 0) {
        return ERR_SUCCESS;
    } else if (input_len >= 9 && strncmp(input, "BasilC#//", 9) == 0) {
        return ERR_SUCCESS;
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
        return ERR_PAREN;
    }

    // Extract command name
    if (has_prefix) {
        strncpy(cmd_name, input+7, paren-7);
        cmd_name[paren-7] = '\0';
    } else {
        strncpy(cmd_name, input, paren);
        cmd_name[paren] = '\0';
    }

    // Search for command in registered command stack
    registered_cmd_stack_t *res = cmd_stack_search_label(cmd_name);
    if (res == NULL) {
        return ERR_INVALID_CMD;
    }

    // Confirm number of given arguments with expected number
    int32_t num_args = get_char_occurances(input, ",");
    if (num_args != 0) {
        num_args++;
    } else if (input_len > paren && input[paren+1] != ')') {
        num_args++;
    }
    if (res->num_args >= 0 && num_args != res->num_args) {
        return ERR_ARGS;
    }

    // Extract arguments if applicable and add to general stack
    if (res->num_args > 1) {
        int32_t num_commas = get_char_occurances(input, ",");
        int32_t paren_end = str_index_of_skip(input, ")", paren);

        // Extract first argument and put into stack
        strncpy(current_stack->parameters[0], input+paren+1, str_index_of(input, ",")-paren-1);

        // Put in the rest
        int32_t i;
        for (i=0; i<num_commas; i++) {
            int32_t start = str_index_of_n(input, ",", i)+1;
            int32_t end = str_index_of_n(input, ",", i+1)-2;
            if (end == -1 - 2) {
                end = paren_end - 1;
            }
            strncpy(current_stack->parameters[i+1], input+start+1, end-start);
        }

        current_stack->command = res->name;

        goto advance_stack;
    } else if (res->num_args == 1 || res->num_args == -1) {
        // Only one argument provided, or -1 was specified which forces 1 arg
        int32_t paren_end = str_index_of_skip(input, ")", paren);
        strncpy(current_stack->parameters[0],
                input+paren+1, paren_end-paren-1);
        current_stack->command = res->name;

        goto advance_stack;
    } else {
        // No arguments, add to stack
        current_stack->command = res->name;
        goto advance_stack;
    }

    return false;
advance_stack:
    current_stack->execute = !in_block;
    // Handle special parsing commands
    if (res->special_parse != NULL && !(res->special_parse())) {
        return ERR_SPECIAL_PARSE;
    }
    current_stack->next = malloc(sizeof(stack_node_t));
    stack_node_initialize(current_stack->next);
    current_stack = current_stack->next;
    return ERR_SUCCESS;
}

/**
 * Execute a command from the general stack
 */
bool execute_command(stack_node_t **node) {
    bool result;

    // Handle empty commands
    if ((*node)->command == NULL) goto skip_node;

    // Handle nodes marked as "don't execute"
    if ((*node)->execute == false) goto skip_node;

    // Get registered_cmd_stack entry
    registered_cmd_stack_t *res = cmd_stack_search_label((*node)->command);
    if (res == NULL) return false;

    // Save stack node state before calling
    stack_node_t *temp = *node;

    // Skip functions without a handler command
    if (res->handle_cmd == NULL) {
        result = true;
        goto increment_stack_node;
    }

    // Call handler function
    result = res->handle_cmd(node);

increment_stack_node:
    // If stack wasn't modified by function, increment it to the next one
    if (result && *node == temp)
        *node = (*node)->next;

    return result;
skip_node:
    *node = (*node)->next;
    return true;
}
