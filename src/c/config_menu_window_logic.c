#include "config_menu_window_logic.h"

#include <pebble.h>

#include "persistance.h"
#include "icons.h"

#define MAX_QUAD_BRUSH_TIME (60)
#define MIN_QUAD_BRUSH_TIME (10)

static SimpleMenuItem* m_theme_item;
static SimpleMenuItem* m_short_time;
static SimpleMenuItem* m_long_time;
static SimpleMenuItem* m_auto_start;
static SimpleMenuItem* m_auto_kill;

static SimpleMenuLayer* m_settings_menu_layer;
static StatusBarLayer* m_status_bar;

static void update_theme_menu_item();
static void update_short_menu_item();
static void update_long_menu_item();
static void update_auto_start_menu_item();
static void update_auto_kill_menu_item();

static void refresh_ui()
{
    layer_mark_dirty(simple_menu_layer_get_layer(m_settings_menu_layer));
}

char* get_current_theme()
{
    static char theme_buffer[6];

    char* theme = is_dark_theme() ? "Dark" : "Light";
    strncpy(theme_buffer, theme, 6);

    return theme_buffer;
}

char* get_current_short_time()
{
    static char short_time_buffer[3];

    snprintf(short_time_buffer, 3, "%d", get_short_quad_time());

    return short_time_buffer;
}

char* get_current_long_time()
{
    static char long_time_buffer[3];

    snprintf(long_time_buffer, 3, "%d", get_long_quad_time());

    return long_time_buffer;
}

char* get_current_auto_start()
{
    static char auto_start_buffer[5];

    strcpy(auto_start_buffer, use_auto_start() ? "True" : "False");

    return auto_start_buffer;
}

char* get_current_auto_kill()
{
    static char auto_kill_buffer[5];

    strcpy(auto_kill_buffer, use_auto_kill() ? "True" : "False");

    return auto_kill_buffer;
}

void handle_toggle_current_theme(int index, void* context)
{
    toggle_theme();
    update_theme_menu_item();
    refresh_ui();
}

static uint8_t get_next_time_tick(uint8_t current)
{
    uint8_t new_time = current + 5;
    if(new_time > MAX_QUAD_BRUSH_TIME)
    {
        new_time = MIN_QUAD_BRUSH_TIME;
    }
    return new_time;
}

void handle_tick_short_time(int index, void* context)
{
    uint8_t new_time = get_next_time_tick(get_short_quad_time());
    set_short_quad_time(new_time);
    update_short_menu_item();
    refresh_ui();
}

void handle_tick_long_time(int index, void* context)
{
    uint8_t new_time = get_next_time_tick(get_long_quad_time());
    set_long_quad_time(new_time);
    update_long_menu_item();
    refresh_ui();
}

void handle_toggle_auto_start(int index, void* context)
{
    toggle_auto_start();
    update_auto_start_menu_item();
    refresh_ui();
}

void handle_toggle_auto_kill(int index, void* context)
{
    toggle_auto_kill();
    update_auto_kill_menu_item();
    refresh_ui();
}

static void update_theme_menu_item()
{
    m_theme_item->subtitle = get_current_theme();
}

static void update_short_menu_item()
{
    m_short_time->subtitle = get_current_short_time();
}

static void update_long_menu_item()
{
    m_long_time->subtitle = get_current_long_time();
}

static void update_auto_start_menu_item()
{
    m_auto_start->subtitle = get_current_auto_start();
}

static void update_auto_kill_menu_item()
{
    m_auto_kill->subtitle = get_current_auto_kill();
}

void update_config_menu(Window* config_window)
{
    window_set_background_color(config_window, get_background_color());
    status_bar_layer_set_colors(m_status_bar, get_background_color(), get_foreground_color());
    update_theme_menu_item();
    update_short_menu_item();
    update_long_menu_item();
    update_auto_start_menu_item();
    update_auto_kill_menu_item();
    refresh_ui();
}

void setup_settings_items(
    SimpleMenuItem* theme_item,
    SimpleMenuItem* short_time,
    SimpleMenuItem* long_time,
    SimpleMenuItem* auto_start,
    SimpleMenuItem* auto_kill,
    SimpleMenuLayer* settings_menu_layer,
    StatusBarLayer* status_bar)
{
    m_theme_item = theme_item;
    m_short_time = short_time;
    m_long_time = long_time;
    m_auto_start = auto_start;
    m_auto_kill = auto_kill;
    m_settings_menu_layer = settings_menu_layer;
    m_status_bar = status_bar;
}