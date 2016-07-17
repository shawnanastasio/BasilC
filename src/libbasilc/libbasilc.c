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
#include <stdio.h>

#include <libbasilc/controlflow.h>
#include <libbasilc/io.h>
#include <libbasilc/system.h>
#include <libbasilc/variable.h>

#include <cmd.h>

/**
 * Registers all libbasilc functions to the registered_cmd_stack.
 */
void libbasilc_register() {
    /* Register controlflow functions */
    register_cmd(&basilc_if);
    register_cmd(&basilc_endif);
    register_cmd(&basilc_label);
    register_cmd(&basilc_goto);
    register_cmd(&basilc_end);

    /* Register io functions */
    register_cmd(&basilc_say);
    register_cmd(&basilc_sayln);
    register_cmd(&basilc_tint);
    register_cmd(&basilc_tintbg);
    register_cmd(&basilc_ask);

    /* Register system functions */
    register_cmd(&basilc_yolo);
    register_cmd(&basilc_naptime);

    /* Register variable functions */
    register_cmd(&basilc_define);
}

void __debug_print_cmd_stack() {
    registered_cmd_stack_t *cur = root_cmd;
    while (cur->next != NULL) {
        printf("cmd: %s\n", cur->name);
        cur = cur->next;
    }
}
