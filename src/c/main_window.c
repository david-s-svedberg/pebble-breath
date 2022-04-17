#include "main_window.h"

#include <pebble.h>

#include "main_window_logic.h"
#include "icons.h"
#include "persistance.h"

static Window *main_window;

static StatusBarLayer* status_bar;

static Layer* main_layer;

static ActionBarLayer* action_bar;

static void main_window_click_config_provider(void* context)
{
    window_single_click_subscribe(BUTTON_ID_UP, toggle_exercise);
    window_single_click_subscribe(BUTTON_ID_SELECT, toggle_running);
    window_single_click_subscribe(BUTTON_ID_DOWN, goto_config_window);
}

static void setup_main_window_action_bar_layer(Layer *window_layer, GRect bounds)
{
    action_bar = action_bar_layer_create();
    action_bar_layer_set_background_color(action_bar, get_foreground_color());
    action_bar_layer_add_to_window(action_bar, main_window);
    action_bar_layer_set_click_config_provider(action_bar, main_window_click_config_provider);

    action_bar_layer_set_icon_animated(action_bar, BUTTON_ID_UP, get_swap_icon(), true);
    action_bar_layer_set_icon_animated(action_bar, BUTTON_ID_SELECT, get_play_icon(), true);
    action_bar_layer_set_icon_animated(action_bar, BUTTON_ID_DOWN, get_config_icon(), true);
}


static void setup_main_layer(Layer *window_layer, GRect bounds)
{
    main_layer = layer_create(GRect(bounds.origin.x, STATUS_BAR_LAYER_HEIGHT, bounds.size.w - ACTION_BAR_WIDTH, bounds.size.h - STATUS_BAR_LAYER_HEIGHT));
    layer_set_update_proc(main_layer, update_main_layer);
    layer_add_child(window_layer, main_layer);

}

static void setup_status_bar(Layer *window_layer, GRect bounds)
{
    status_bar = status_bar_layer_create();

    status_bar_layer_set_colors(status_bar, get_background_color(), get_foreground_color());
    status_bar_layer_set_separator_mode(status_bar, StatusBarLayerSeparatorModeDotted);

    layer_add_child(window_layer, status_bar_layer_get_layer(status_bar));
}

static void load_main_window(Window *window)
{
    window_set_background_color(window, get_background_color());
    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(window_layer);

    setup_main_layer(window_layer, bounds);
    setup_status_bar(window_layer, bounds);
    setup_main_window_action_bar_layer(window_layer, bounds);

    setup_layers(
        main_layer,
        action_bar,
        status_bar,
        main_window);

    reset_breathing();

    if(use_auto_start())
    {
        start_breathing();
    }
}

static void unload_main_window(Window *window)
{
    action_bar_layer_remove_from_window(action_bar);
    action_bar_layer_destroy(action_bar);
    status_bar_layer_destroy(status_bar);
    layer_destroy(main_layer);
}

void setup_main_window(GColor8 background_color, GColor8 foreground_color)
{
    main_window = window_create();

    window_set_window_handlers(main_window, (WindowHandlers) {
        .load = load_main_window,
        .unload = unload_main_window,
        .appear = update_main_window
    });

    window_stack_push(main_window, true);
}

void tear_down_main_window()
{
    window_destroy(main_window);
}