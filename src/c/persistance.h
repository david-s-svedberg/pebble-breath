#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "data.h"

GColor8 get_background_color();
GColor8 get_foreground_color();

bool use_long_time();
uint8_t get_long_quad_time();
uint8_t get_short_quad_time();
void toggle_quad_time();
void set_long_quad_time(uint8_t value);
void set_short_quad_time(uint8_t value);

bool is_dark_theme();
void toggle_theme();

bool has_any_data();
void save_data();

bool use_auto_start();
void toggle_auto_start();
bool use_auto_kill();
void toggle_auto_kill();

uint8_t get_current_quad_time();