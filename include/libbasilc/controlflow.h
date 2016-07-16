#pragma once

#include <stdbool.h>

#include <main.h>
#include <cmd.h>

// Definition for BasilC-if()
bool basilc_if_callback(stack_node_t **node);
bool basilc_if_special_parse();
cmd_declaration_t basilc_if = {
    .name = "if",
    .num_args = 1,
    .handle_cmd = basilc_if_callback,
    .special_parse = basilc_if_special_parse,
};

// Definition of BasilC-endif()
bool basilc_endif_special_parse();
cmd_declaration_t basilc_endif = {
    .name = "endif",
    .num_args = 0,
    .special_parse = basilc_endif_special_parse,
};

// Definition of BasilC-label()
bool basilc_label_callback(stack_node_t **node);
cmd_declaration_t basilc_label = {
    .name = "label",
    .num_args = 1,
};

// Definition of BasilC-goto()
bool basilc_goto_callback(stack_node_t **node);
cmd_declaration_t basilc_goto = {
    .name = "goto",
    .num_args = 1,
    .handle_cmd = basilc_goto_callback,
};

// Definition for BasilC-end()
bool basilc_end_callback(stack_node_t **node);
cmd_declaration_t basilc_end = {
    .name = "end",
    .num_args = 0,
    .handle_cmd = basilc_end_callback,
};
