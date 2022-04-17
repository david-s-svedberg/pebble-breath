#include "config_menu_window.h"

#include <pebble.h>

#include "config_menu_window_logic.h"
#include "persistance.h"

static Window *config_window;

static StatusBarLayer* status_bar;

static SimpleMenuLayer* settings_menu_layer;

static SimpleMenuItem m_settings_items[5];

static SimpleMenuItem m_theme_item;
static SimpleMenuItem m_short_time;
static SimpleMenuItem m_long_time;
static SimpleMenuItem m_auto_start;
static SimpleMenuItem m_auto_kill;

static SimpleMenuSection m_settings_section;

static SimpleMenuSection m_menu[1];

static GColor8 m_background_color;
static GColor8 m_foreground_color;

static void setup_settings_menu_layer(Layer *window_layer, GRect bounds)
{
    m_settings_section.items = m_settings_items;
    m_settings_section.title = "Settings";
    m_settings_section.num_items = 5;

    m_theme_item.title = "Switch Theme";
    m_theme_item.subtitle = get_current_theme();
    m_theme_item.callback = handle_toggle_current_theme;
    m_settings_items[0] = m_theme_item;

    m_short_time.title = "Short time";
    m_short_time.subtitle = get_current_short_time();
    m_short_time.callback = handle_tick_short_time;
    m_settings_items[1] = m_short_time;

    m_long_time.title = "Long time";
    m_long_time.subtitle = get_current_long_time();
    m_long_time.callback = handle_tick_long_time;
    m_settings_items[2] = m_long_time;

    m_auto_start.title = "Auto start";
    m_auto_start.subtitle = get_current_auto_start();
    m_auto_start.callback = handle_toggle_auto_start;
    m_settings_items[3] = m_auto_start;

    m_auto_kill.title = "Auto kill";
    m_auto_kill.subtitle = get_current_auto_kill();
    m_auto_kill.callback = handle_toggle_auto_kill;
    m_settings_items[4] = m_auto_kill;

    m_menu[0] = m_settings_section;

    settings_menu_layer = simple_menu_layer_create(
        GRect(0, STATUS_BAR_LAYER_HEIGHT, bounds.size.w, bounds.size.h - STATUS_BAR_LAYER_HEIGHT),
        config_window,
        m_menu, ARRAY_LENGTH(m_menu), NULL);

    layer_add_child(window_layer, simple_menu_layer_get_layer(settings_menu_layer));
}

static void setup_status_bar(Layer *window_layer, GRect bounds)
{
    status_bar = status_bar_layer_create();

    status_bar_layer_set_colors(status_bar, m_background_color, m_foreground_color);
    status_bar_layer_set_separator_mode(status_bar, StatusBarLayerSeparatorModeDotted);

    layer_add_child(window_layer, status_bar_layer_get_layer(status_bar));
}

static void load_config_menu_window(Window *config_window)
{
    window_set_background_color(config_window, m_background_color);
    Layer *config_window_layer = window_get_root_layer(config_window);
    GRect config_window_bounds = layer_get_bounds(config_window_layer);

    setup_settings_menu_layer(config_window_layer, config_window_bounds);
    setup_status_bar(config_window_layer, config_window_bounds);

    setup_settings_items(&m_theme_item, &m_short_time, &m_long_time, &m_auto_start, &m_auto_kill, settings_menu_layer, status_bar);
}

static void unload_config_menu_window(Window *window)
{
    simple_menu_layer_destroy(settings_menu_layer);
    status_bar_layer_destroy(status_bar);
}

void setup_config_menu_window()
{
    config_window = window_create();

    window_set_window_handlers(config_window, (WindowHandlers) {
        .load = load_config_menu_window,
        .unload = unload_config_menu_window,
        .appear = update_config_menu
    });

    window_stack_push(config_window, true);
}

void tear_down_config_menu_window()
{
    window_destroy(config_window);
}