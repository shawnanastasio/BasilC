#pragma once

#include <stdbool.h>

#include <main.h>
#include <cmd.h>

// Definition for BasilC-define()
bool basilc_define_callback(stack_node_t **node);
cmd_declaration_t basilc_define = {
    .name = "define",
    .num_args = 2,
    .handle_cmd = basilc_define_callback,
};
