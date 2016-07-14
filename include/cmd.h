#pragma once

#include <stdint.h>
#include <stdbool.h>

#include <main.h>

struct cmd_declaration {
    char *name;
    uint8_t num_args;
    void (*handle_cmd)(stack_node_t *);
};
typedef struct cmd_declaration cmd_declaration_t;

struct registered_cmd_stack {
    char *name;
    uint8_t num_args;
    void (*handle_cmd)(stack_node_t *);
    struct registered_cmd_stack *next;
};
typedef struct registered_cmd_stack registered_cmd_stack_t;

extern registered_cmd_stack_t *root_cmd;
extern registered_cmd_stack_t *current_cmd_stack;

void init_cmd_stack();
void register_cmd(cmd_declaration_t *dec);
registered_cmd_stack_t * cmd_stack_search_label(char *label);
bool parse_user_command(char *input, int32_t input_len);
