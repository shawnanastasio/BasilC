#pragma once

#include <stdint.h>

#define STACK_PARAMETER_MAX_AMOUNT 5 // Max parameters a command can have
#define STACK_PARAMETER_MAX_LENGTH 100 // Max length of a parameter

// General stack node contains command and pointer to parameter linked list
struct stack_node {
    char *command;
    bool execute;
    char parameters[STACK_PARAMETER_MAX_AMOUNT][STACK_PARAMETER_MAX_LENGTH];
    struct stack_node *next;
};

typedef struct stack_node stack_node_t;

void stack_node_initialize(struct stack_node *s);
void parse_line(char *line, int line_len, int linenum);
void stack_execute();
stack_node_t * stack_search_label(char *label);
void set_block_execute(stack_node_t *start, bool val);
bool eval_conditional(char *cond);
int str_index_of(char *str, char c);
void exit_with_error(char *error);
int32_t split_string_delimiter(char *buf, char *str, char delim);
int32_t split_string_delimiter_rev(char *buf, char *str, char delim);
void parse_cleanup();
