#include "icons.h"

#include "persistance.h"

static GBitmap *config_icon;
static GBitmap *play_icon;
static GBitmap *breath_icon;
static GBitmap *pause_icon;
static GBitmap *swap_icon;

static GBitmap* get_icon(uint32_t id, GBitmap* icon)
{
    if(icon == NULL)
    {
        icon = gbitmap_create_with_resource(id);
    }

    return icon;
}

static void destroy_icon(GBitmap* icon)
{
    if(icon == NULL)
    {
        gbitmap_destroy(icon);
    }
}

GBitmap* get_config_icon()
{
    uint32_t icon_id;
    if(is_dark_theme())
    {
        icon_id = RESOURCE_ID_CONFIG_BLACK_ICON;
    } else
    {
        icon_id = RESOURCE_ID_CONFIG_WHITE_ICON;
    }
    return get_icon(icon_id, config_icon);
}

GBitmap* get_play_icon()
{
    uint32_t icon_id;
    if(is_dark_theme())
    {
        icon_id = RESOURCE_ID_PLAY_BLACK_ICON;
    } else
    {
        icon_id = RESOURCE_ID_PLAY_WHITE_ICON;
    }
    return get_icon(icon_id, play_icon);
}

GBitmap* get_breath_icon()
{
    return get_icon(RESOURCE_ID_BREATH_ICON, breath_icon);
}

GBitmap* get_pause_icon()
{
    uint32_t icon_id;
    if(is_dark_theme())
    {
        icon_id = RESOURCE_ID_PAUSE_BLACK_ICON;
    } else
    {
        icon_id = RESOURCE_ID_PAUSE_WHITE_ICON;
    }
    return get_icon(icon_id, pause_icon);
}

GBitmap* get_swap_icon()
{
    uint32_t icon_id;
    if(is_dark_theme())
    {
        icon_id = RESOURCE_ID_SWAP_BLACK_ICON;
    } else
    {
        icon_id = RESOURCE_ID_SWAP_WHITE_ICON;
    }
    return get_icon(icon_id, swap_icon);
}

void destroy_all_icons()
{
    destroy_icon(config_icon);
    destroy_icon(play_icon);
    destroy_icon(breath_icon);
    destroy_icon(pause_icon);
    destroy_icon(swap_icon);
}
