#pragma once

#include <cmd.h>

// Definition for BasilC-end()
void basilc_end_callback(stack_node_t *node);
cmd_declaration_t basilc_end = {
    .name = "end",
    .num_args = 0,
    .handle_cmd = basilc_end_callback,
};
