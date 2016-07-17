#pragma once

#include <stdbool.h>

#include <main.h>
#include <cmd.h>

// Definition for BasilC-say()
bool basilc_say_callback(stack_node_t **node);
cmd_declaration_t basilc_say = {
    .name = "say",
    .num_args = -1,
    .handle_cmd = basilc_say_callback,
};

// Definition for BasilC-sayln()
bool basilc_sayln_callback(stack_node_t **node);
cmd_declaration_t basilc_sayln = {
    .name = "sayln",
    .num_args = -1,
    .handle_cmd = basilc_sayln_callback,
};

// Definition for BasilC-tint()
bool basilc_tint_callback(stack_node_t **node);
cmd_declaration_t basilc_tint = {
    .name = "tint",
    .num_args = 1,
    .handle_cmd = basilc_tint_callback,
};

// Definition for BasilC-tintbg()
bool basilc_tintbg_callback(stack_node_t **node);
cmd_declaration_t basilc_tintbg = {
    .name = "tintbg",
    .num_args = 1,
    .handle_cmd = basilc_tintbg_callback,
};

void basilc_handle_tint(char code, char *temp);

// Definition for BasilC-ask()
bool basilc_ask_callback(stack_node_t **node);
cmd_declaration_t basilc_ask = {
    .name = "ask",
    .num_args = 2,
    .handle_cmd = basilc_ask_callback,
};
