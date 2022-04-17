#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <pebble.h>

#define CURRENT_DATA_VERSION (1)

typedef struct {
    GColor8 background_color;
    GColor8 foreground_color;
    uint8_t short_quad_time;
    uint8_t long_quad_time;
    bool current_is_long;
    uint8_t data_version;
    bool auto_start;
    bool auto_kill;
} Data;