/**
 * The BasilC Interpreter
 * Copyright (C) Shawn Anastasio 2016
 * Licensed under the GNU GPL v3
 */
/**
 * This file contains code that defines BasilC commands related to program
 * control flow, such as if(), endif(), label(), goto(), and end()
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include <main.h>
#include <cmd.h>

// Handle execution of BasilC-if()
bool basilc_if_callback(stack_node_t **node) {
    bool cond;

    // Try to parse variables in string
    char *parsed = parse_var_string((*node)->parameters[0]);
    if (parsed != NULL) {
        // If variables were parsed correctly
        cond = eval_conditional(parsed);
        free(parsed);
    } else {
        cond = eval_conditional((*node)->parameters[0]);
    }

    // If condition is true, mark all commands in block as execute
    if (cond) {
        set_block_execute(*node, true);
    }
    return true;
}
// Handle special parsing of BasilC-if()
bool basilc_if_special_parse() {
    // BasilC-if() requires in_block to be set to true
    // at time of parsing
    in_block = true;
    return true;
}

// Handle special parsing of BasilC-endif()
bool basilc_endif_special_parse() {
    if (!in_block) {
        return false;
    } else {
        in_block = false;
        return true;
    }
}

// Handle execution of BasilC-goto()
bool basilc_goto_callback(stack_node_t **node) {
    stack_node_t *label = stack_search_label((*node)->parameters[0]);
    if (label != NULL) {
        *node = label;
        return true;
    } else {
        return false;
    }
}

// Handle execution of BasilC-end()
bool basilc_end_callback(stack_node_t **node) {
    // Exit program
    exit(0);

    return false;
}
