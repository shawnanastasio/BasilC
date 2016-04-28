/**
 * The BasilC Interpreter
 * Copyright (C) Shawn Anastasio 2016
 * Licensed under the GNU GPL v3
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "main.h"

// Comments: BasilC#// (comment)
// Loops: Basilc-loop(times)
// Print: BasilC-say()

struct stack_node *root;
struct stack_node *current_stack;

int main(int argc, char **argv) {
    // Verify arguments
    if (argc != 2) {
        printf("Usage: %s <script.basilc>\n", argv[0]);
        return 1;
    }

    // Create initial stack
    root = (struct stack_node *) malloc(sizeof(struct stack_node));
    current_stack = root;
    stack_node_initialize(root);

    // Open script file
    FILE *fp;
    fp = fopen(argv[1], "r");
    if (fp == NULL) {
        perror("Error");
        return 1;
    }
    int lines = 0;
    int chars = 0;

    // Count lines and chars
    char cur_char = 0;
    while (!feof(fp)) {
        cur_char = fgetc(fp);
        if (cur_char == '\n') ++lines;
        chars++;
    }

    // Create Buffer with correct size
    char *buffer = 0;

    // Fill buffer
    if (fp) {
        fseek(fp, 0, SEEK_SET);
        buffer = malloc(chars+1);
        if (buffer) {
            fread (buffer, 1, chars, fp);
        }
        fclose(fp);
        buffer[chars] = '\0';
    }

    // DEBUG
    fputs("BasilC Interpreter v1.0\n\n", stderr);

    // Begin parsing
    cur_char = 0;

    int line_len = 0;
    int pos = 0;
    for (int i=0; i<lines; i++) {
        // Get line length
        while (buffer[pos+line_len] && buffer[pos+line_len] != '\n') {
            ++line_len;
        }

        // Put line in temp buffer
        ++line_len;
        char temp[line_len];
        strncpy(temp, buffer+pos, line_len);

        // Parse line
        parse_line(temp, line_len);

        // Update position and reset line_len
        pos += line_len;
        line_len = 0;

    }

    // Execute stack
    stack_execute();
}

// Intialize an empty stack node
void stack_node_initialize(struct stack_node *s) {
    s->command = NULL;
    for (int i=0; i<STACK_PARAMETER_MAX_AMOUNT; i++) {
        for (int z=0; z<STACK_PARAMETER_MAX_LENGTH; z++) {
            s->parameters[i][z] = 0;
        }
    }
    s->next = NULL;
}

// Parse line of code
void parse_line(char *line, int line_len) {
    // Remove trailing \n
    if (line[--line_len] == '\n') line[line_len] = '\0';

    // Skip empty string
    if (line_len == 0) return;

    //printf("PARSE LINE: %s\n", line);
    //printf("line_len: %d\n", line_len);

    // Parse line of code
    if (line_len <= 6) {
        // All BasilC code is prefixed with "BasilC"; line length must be > 6
        goto parse_fail;
    } if (strncmp(line, "BasilC", 6) != 0) {
        goto parse_fail;
    }

    // Check comment
    if (line_len >= 9 && strncmp(line, "BasilC#//", 9) == 0) {
        return;
    }

    // Check say()
    if (line_len >= 11 && strncmp(line, "BasilC-say(", 11) == 0) {
        // Make sure arguments are closed
        if (line[line_len-1] != ')') {
            goto parse_fail;
        }

        // Add to stack
        current_stack->command = "say";
        // If parameter is specified, add it
        if (line_len > 12) {
            strncpy(current_stack->parameters[0], line+11, line_len-11-1);
        }

        goto advance_stack;
    }

    return;
parse_fail:
    fprintf(stderr, "Invalid command: %s\n", line);
    return;

advance_stack:
    current_stack->next = malloc(sizeof(struct stack_node));
    stack_node_initialize(current_stack->next);
    current_stack = current_stack->next;
    return;
}

void stack_execute() {
    struct stack_node *cur = root;
    while (cur != NULL) {
        // Handle commands
        if (cur->command == NULL) goto loop_next;

        // say()
        if (strcmp(cur->command, "say") == 0) {
            char *temp = cur->parameters[0];
            printf("%s\n", temp);
            goto loop_next;
        }

    loop_next:
        cur = cur->next;
    }
}
