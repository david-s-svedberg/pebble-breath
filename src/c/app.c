#include "app.h"

#include "main_window.h"
#include "config_menu_window.h"

#include "icons.h"
#include "app_glance.h"
#include "persistance.h"

void init()
{
    setup_main_window(get_background_color(), get_foreground_color());
}

void deinit()
{
    APP_LOG(APP_LOG_LEVEL_INFO, "Deiniting Brush");

    tear_down_main_window();
    tear_down_config_menu_window();
    destroy_all_icons();
    setup_app_glance();
}