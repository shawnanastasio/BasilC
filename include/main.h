#pragma once

#include <stdint.h>

#define STACK_PARAMETER_MAX_AMOUNT 5 // Max parameters a command can have
#define STACK_PARAMETER_MAX_LENGTH 100 // Max length of a parameter

#define MAX_DATA_SIZE 32

// General stack node contains command and pointer to parameter linked list
struct stack_node {
    char *command;
    bool execute;
    char parameters[STACK_PARAMETER_MAX_AMOUNT][STACK_PARAMETER_MAX_LENGTH];
    struct stack_node *next;
};

struct variable_stack_node {
    char name[MAX_DATA_SIZE];
    char data[MAX_DATA_SIZE];
    struct variable_stack_node *next;
};

typedef struct stack_node stack_node_t;
typedef struct variable_stack_node variable_stack_node_t;

void stack_node_initialize(struct stack_node *s);
void parse_line(char *line, int line_len, int linenum);
void stack_execute();
void parse_cleanup();
stack_node_t * stack_search_label(char *label);
variable_stack_node_t * var_stack_search_label(char *label);
void set_block_execute(stack_node_t *start, bool val);
bool eval_conditional(char *cond);
void exit_with_error(char *error);
char * get_data_for_var(char *var_name);
void prepare_var_string(char *str, int32_t num_vars);
char * parse_var_string(char *str);
void printANSIescape(char *code);
