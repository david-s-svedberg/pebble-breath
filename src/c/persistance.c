#include "persistance.h"

#include <pebble.h>
#include <stdbool.h>
#include <gcolor_definitions.h>

static const uint32_t DATA_KEY = 659154;

static Data m_data;
static bool m_data_loaded = false;

static void seed_version_1_data(Data* data)
{
    data->background_color = GColorBlack;
    data->foreground_color = GColorWhite;
    data->current_is_long = true;
    data->long_quad_time = 35;
    data->short_quad_time = 15;
    data->auto_start = false;
    data->auto_kill = false;
}

static void seed_data()
{
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Seeding data");
    seed_version_1_data(&m_data);

    m_data.data_version = CURRENT_DATA_VERSION;
    save_data();
}

static bool data_version_is_current(Data* data)
{
    bool is_current = true;
    if(data->data_version < CURRENT_DATA_VERSION)
    {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "The data version:%d is lower than the current version:%d", data->data_version, CURRENT_DATA_VERSION);
        is_current = false;
    }
    return is_current;
}

static void migrate_data(Data* data)
{
    if(data->data_version < 1)
    {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Migrating to data version 1");

        seed_version_1_data(data);
        data->data_version = 1;
    }
}

static Data* get_data()
{
    if(!has_any_data())
    {
        seed_data();
    }
    if(!m_data_loaded)
    {
        persist_read_data(DATA_KEY, &m_data, sizeof(Data));
        m_data_loaded = true;
        if(!data_version_is_current(&m_data))
        {
            migrate_data(&m_data);
            save_data();
        }
    }
    return &m_data;
}

bool use_long_time()
{
    return get_data()->current_is_long;
}

uint8_t get_long_quad_time()
{
    return get_data()->long_quad_time;
}

uint8_t get_short_quad_time()
{
    return get_data()->short_quad_time;
}

void toggle_quad_time()
{
    Data* data = get_data();
    bool current = data->current_is_long;
    data->current_is_long = !current;
    save_data();
}

void set_long_quad_time(uint8_t value)
{
    get_data()->long_quad_time = value;
    save_data();
}

void set_short_quad_time(uint8_t value)
{
    get_data()->short_quad_time = value;
    save_data();
}

bool has_any_data()
{
    return persist_exists(DATA_KEY);
}

void save_data()
{
    persist_write_data(DATA_KEY, &m_data, sizeof(Data));
}

GColor8 get_background_color()
{
    return get_data()->background_color;
}

GColor8 get_foreground_color()
{
    return get_data()->foreground_color;
}

bool is_dark_theme()
{
    uint8_t background = get_background_color().argb;
    uint8_t black = GColorBlack.argb;
    return (background == black);
}

void toggle_theme()
{
    Data* data = get_data();
    GColor8 previous_background_color = data->background_color;
    GColor8 previous_foreground_color = data->foreground_color;

    data->background_color = previous_foreground_color;
    data->foreground_color = previous_background_color;
    save_data();
}

bool use_auto_start()
{
    return get_data()->auto_start;
}

void toggle_auto_start()
{
    Data* data = get_data();
    data->auto_start = !data->auto_start;
    save_data();
}

bool use_auto_kill()
{
    return get_data()->auto_kill;
}

void toggle_auto_kill()
{
    Data* data = get_data();
    data->auto_kill = !data->auto_kill;
    save_data();
}

uint8_t get_current_quad_time()
{
    Data* data = get_data();
    if(data->current_is_long)
    {
        return data->long_quad_time;
    } else
    {
        return data->short_quad_time;
    }
}