#pragma once

#include <stdint.h>
#include <stdbool.h>

#include <main.h>

enum parse_error {
    ERR_SUCCESS,
    ERR_PAREN,
    ERR_INVALID_CMD,
    ERR_ARGS,
    ERR_SPECIAL_PARSE
};

extern char parse_error_msgs[][32];

struct cmd_declaration {
    char *name;
    int8_t num_args;
    bool (*handle_cmd)(stack_node_t **);
    bool (*special_parse)(void);
};
typedef struct cmd_declaration cmd_declaration_t;

struct registered_cmd_stack {
    char *name;
    int8_t num_args;
    bool (*handle_cmd)(stack_node_t **);
    bool (*special_parse)(void);
    struct registered_cmd_stack *next;
};
typedef struct registered_cmd_stack registered_cmd_stack_t;

extern registered_cmd_stack_t *root_cmd;
extern registered_cmd_stack_t *current_cmd_stack;

void init_cmd_stack();
void register_cmd(cmd_declaration_t *dec);
registered_cmd_stack_t * cmd_stack_search_label(char *label);
int32_t parse_user_command(char *input, int32_t input_len);
bool execute_command(stack_node_t **node);
void __debug_print_cmd_stack();
