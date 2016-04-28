#pragma once

#define STACK_PARAMETER_MAX_AMOUNT 5 // Max parameters a command can have
#define STACK_PARAMETER_MAX_LENGTH 100 // Max length of a parameter

// General stack node contains command and pointer to parameter linked list
struct stack_node {
    char *command;
    char parameters[STACK_PARAMETER_MAX_AMOUNT][STACK_PARAMETER_MAX_LENGTH];
    struct stack_node *next;
};

typedef struct stack_node stack_node;

void stack_node_initialize(struct stack_node *s);
void parse_line(char *line, int line_len);
void stack_execute();
stack_node * stack_search_label(char *label);
