/**
 * The BasilC Interpreter
 * Copyright (C) Shawn Anastasio 2016
 * Licensed under the GNU GPL v3
 */
/**
 * This file contains code that defines BasilC commands related to program
 * control flow, such as if(), endif(), goto(), label(), and end()
 */

#include <stdint.h>
#include <stdlib.h>

#include <main.h>
#include <cmd.h>

// Handle execution of BasilC-end()
void basilc_end_callback(stack_node_t *node) {
    // Exit program
    exit(0);
}
