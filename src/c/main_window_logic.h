#pragma once

#include <pebble.h>

void goto_config_window(ClickRecognizerRef recognizer, void* context);
void toggle_running(ClickRecognizerRef recognizer, void* context);
void toggle_exercise(ClickRecognizerRef recognizer, void* context);
void setup_layers(
    Layer* main_layer,
    ActionBarLayer* action_bar,
    StatusBarLayer* status_bar,
    Window* main_window);
void update_main_window(Window *window);
void start_breathing();
void reset_breathing();

void update_main_layer(struct Layer *layer, GContext *ctx);