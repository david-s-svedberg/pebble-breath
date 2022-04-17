#pragma once

#include <pebble.h>

void update_config_menu(Window* config_window);
void setup_settings_items(
    SimpleMenuItem* theme_item,
    SimpleMenuItem* short_time,
    SimpleMenuItem* long_time,
    SimpleMenuItem* auto_start,
    SimpleMenuItem* auto_kill,
    SimpleMenuLayer* settings_menu_layer,
    StatusBarLayer* status_bar);
char* get_current_theme();
char* get_current_short_time();
char* get_current_long_time();
char* get_current_auto_start();
char* get_current_auto_kill();
void handle_toggle_current_theme(int index, void* context);
void handle_tick_short_time(int index, void* context);
void handle_tick_long_time(int index, void* context);
void handle_toggle_auto_start(int index, void* context);
void handle_toggle_auto_kill(int index, void* context);