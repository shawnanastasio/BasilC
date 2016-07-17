/**
 * The BasilC Interpreter
 * Copyright (C) Shawn Anastasio 2016
 * Licensed under the GNU GPL v3
 */
/**
 * This file contains code that defines BasilC commands related to input and
 * output operations, such as say(), tint(), and ask()
 */

 #include <stdint.h>
 #include <stdbool.h>
 #include <stdlib.h>
 #include <string.h>
 #include <stdio.h>
 #include <ctype.h>

 #include <main.h>
 #include <cmd.h>

// Handle execution of BasilC-say()
bool basilc_say_callback(stack_node_t **node) {
    char *parsed = parse_var_string((*node)->parameters[0]);
    if (parsed != NULL) {
        printf("%s", parsed);
        free(parsed);
    } else {
        printf("%s", (*node)->parameters[0]);
    }
    return true;
}

// Handle execution of BasilC-sayln()
bool basilc_sayln_callback(stack_node_t **node) {
   basilc_say_callback(node);
   printf("\n");
   return true;
}

//shared code used by tint() and tintbg()
void basilc_handle_tint(char code, char *temp){
  /* prints ANSI escape code to allow the following BasilC-say
  statement to be in the corresponding color */
  if (monochrome_mode)
      return;
  if (strcmp(temp, "black") == 0)
      printf("\033[%c0m", code);
  else if (strcmp(temp, "red") == 0)
      printf("\033[%c1m", code);
  else if (strcmp(temp, "green") == 0)
      printf("\033[%c2m", code);
  else if (strcmp(temp, "yellow") == 0)
      printf("\033[%c3m", code);
  else if (strcmp(temp, "blue") == 0)
      printf("\033[%c4m", code);
  else if (strcmp(temp, "magenta") == 0)
      printf("\033[%c5m", code);
  else if (strcmp(temp, "cyan") == 0)
      printf("\033[%c6m", code);
  else if (strcmp(temp, "white") == 0)
      printf("\033[%c7m", code);
  /* if the color is not one of the available options, reset
  terminal to default color state */
  else
      printANSIescape("\033[0m");
}

// Handle execution of BasilC-tint()
bool basilc_tint_callback(stack_node_t **node) {
    char *temp = (*node)->parameters[0];
    int32_t i;
    for (i=0; temp[i]; i++){
        temp[i] = tolower(temp[i]);
    }
    /* prints ANSI escape code to allow the following BasilC-say
    statement to be in the corresponding color */
    basilc_handle_tint('3', temp);
    return true;
}

// Handle execution of BasilC-tintbg()
bool basilc_tintbg_callback(stack_node_t **node) {
    char *temp = (*node)->parameters[0];
    int32_t i;
    for (i=0; temp[i]; i++){
        temp[i] = tolower(temp[i]);
    }
    basilc_handle_tint('4', temp);
    return true;
}

// Handle execution of BasilC-ask()
bool basilc_ask_callback(stack_node_t **node) {
    variable_stack_node_t *temp_var = var_stack_search_label((*node)->parameters[1]);
    if (temp_var != NULL) {
        printf("%s", (*node)->parameters[0]);
        fgets(temp_var->data, STACK_PARAMETER_MAX_LENGTH, stdin);
        temp_var->data[strlen(temp_var->data)-1] = '\0';
        return true;
    } else {
        printf("Variable %s has not been declared!\n", (*node)->parameters[1]);
        return false;
    }
}
