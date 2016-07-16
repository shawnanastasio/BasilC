#pragma once

#include <stdbool.h>

#include <main.h>
#include <cmd.h>

// Definition for BasilC-yolo()
bool basilc_yolo_callback(stack_node_t **node);
cmd_declaration_t basilc_yolo = {
    .name = "yolo",
    .num_args = -1,
    .handle_cmd = basilc_yolo_callback,
};

// Definition for BasilC-naptime()
bool basilc_naptime_callback(stack_node_t **node);
cmd_declaration_t basilc_naptime = {
    .name = "naptime",
    .num_args = 1,
    .handle_cmd = basilc_naptime_callback,
};
