/**
 * The BasilC Interpreter
 * Copyright (C) Shawn Anastasio 2016
 * Licensed under the GNU GPL v3
 */
/**
 * This file contains code that defines BasilC commands related to variable
 * storage and usage, such as define()
 */
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include <cmd.h>

// Handle execution of define()
bool basilc_define_callback(stack_node_t **node) {
    char *var_name = (*node)->parameters[0];
    char *var_data = (*node)->parameters[1];

    // Check if variable already exists
    variable_stack_node_t *var = var_stack_search_label(var_name);
    if (var != NULL) {
        // Redefine variable
        strcpy(var->data, var_data);
        return true;
    }

    // Add to variable stack
    strcpy(current_var_stack->name, var_name);
    strcpy(current_var_stack->data, var_data);

    // Advance variable stack
    current_var_stack->next = (variable_stack_node_t *)
                              malloc(sizeof(variable_stack_node_t));
    current_var_stack = current_var_stack->next;

    return true;
}
