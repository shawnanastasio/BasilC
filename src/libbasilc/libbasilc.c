/**
 * The BasilC Interpreter
 * Copyright (C) Shawn Anastasio 2016
 * Licensed under the GNU GPL v3
 */
/**
 * This file contains functions that register libbasilc commands to the
 * interpreter's internal registered_cmd_stack.
 */
#include <stdint.h>

#include <libbasilc/controlflow.h>

#include <cmd.h>

/**
 * Registers all libbasilc functions to the registered_cmd_stack.
 */
void libbasilc_register() {
    /* Register controlflow functions */
    register_cmd(&basilc_end);
}
