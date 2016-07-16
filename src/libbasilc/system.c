/**
 * The BasilC Interpreter
 * Copyright (C) Shawn Anastasio 2016
 * Licensed under the GNU GPL v3
 */
/**
 * This file contains code that defines BasilC commands related to host system
 * operations such as yolo() and naptime().
 */

#include <stdlib.h>

#ifdef __unix__
#include <unistd.h>
#elif _WIN32
#include <windows.h>
#define sleep(x) (Sleep(x*1000))
#endif

#include <cmd.h>

// Handle execution of yolo()
bool basilc_yolo_callback(stack_node_t **node) {
    system((*node)->parameters[0]);
    return true;
}

// Handle execution of naptime()
bool basilc_naptime_callback(stack_node_t **node) {
    sleep(atoi((*node)->parameters[0]));
    return true;
}
