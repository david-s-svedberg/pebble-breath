#include "main_window_logic.h"

#include "config_menu_window.h"
#include "persistance.h"
#include "icons.h"

#define FPS (20)
#define MAX_BREATH_CIRCLE_RADIUS (50)
#define MIN_BREATH_CIRCLE_RADIUS (10)

static const uint16_t refresh_interval_ms = 1000 / FPS;
static GPoint m_circle_center;
static GRect m_circle_empty_rect;
static GRect m_circle_full_rect;
static GRect m_main_layer_text_area;
static GFont m_text_font;

static const char* BREATH_IN_TEXT = "Breath In";
static const char* BREATH_OUT_TEXT = "Breath Out";

typedef enum {
    OrificeNONE,
    Mouth,
    Nose,
} Orifice;

typedef enum {
    ActionTypeNONE,
    BreatheIn,
    BreatheOut,
    HoldFullBreath,
    HoldEmptyBreath,
} ActionType;

typedef struct {
    uint32_t original_ms;
    uint32_t remaining_ms;
    uint32_t animation_ms;
    Orifice orifice;
    ActionType type;
} Action;

typedef struct {
  Action *array;
  size_t used;
  size_t size;
} ActionArray;

void initArray(ActionArray *a, size_t initialSize) {
  a->array = malloc(initialSize * sizeof(Action));
  a->used = 0;
  a->size = initialSize;
}

void insertArray(ActionArray *a, Action element) {
  // a->used is the number of used entries, because a->array[a->used++] updates a->used only *after* the array has been accessed.
  // Therefore a->used can go up to a->size
  if (a->used == a->size) {
    a->size *= 2;
    a->array = realloc(a->array, a->size * sizeof(Action));
  }
  a->array[a->used++] = element;
}

void freeArray(ActionArray *a) {
  free(a->array);
  a->array = NULL;
  a->used = a->size = 0;
}

Window* m_main_window;
ActionBarLayer* m_action_bar;
StatusBarLayer* m_status_bar;

Layer* m_main_layer;

static ActionArray m_actions;
static Action* m_current_action;
uint16_t m_current_action_index;

static AppTimer* m_refresh_timer = NULL;

static bool m_running;

static void stop_breathing();
static void refresh_main_layer(void* data);

static const uint32_t const segments[] = { 50, 25, 50 };
static const VibePattern m_vibration_pattern =
{
    .durations = segments,
    .num_segments = ARRAY_LENGTH(segments),
};

static void debug_seed()
{
    initArray(&m_actions, 2);
    Action in =
    {
        .type = BreatheIn,
        .orifice = Mouth,
        .original_ms = 4000,
        .remaining_ms = 4000,
        .animation_ms = 0,
    };
    Action out =
    {
        .type = BreatheOut,
        .orifice = Mouth,
        .original_ms = 4000,
        .remaining_ms = 4000,
        .animation_ms = 0,
    };
    Action hold_empty =
    {
        .type = HoldEmptyBreath,
        .orifice = Mouth,
        .original_ms = 4000,
        .remaining_ms = 4000,
        .animation_ms = 0,
    };
    Action hold_full =
    {
        .type = HoldFullBreath,
        .orifice = Mouth,
        .original_ms = 4000,
        .remaining_ms = 4000,
        .animation_ms = 0,
    };

    insertArray(&m_actions, in);
    insertArray(&m_actions, out);
    insertArray(&m_actions, hold_empty);
    insertArray(&m_actions, in);
    insertArray(&m_actions, hold_full);
}

static void on_sec_tick(struct tm *tick_time, TimeUnits units_changed)
{
    m_current_action->remaining_ms -= 1000;
    m_current_action->animation_ms = m_current_action->original_ms - m_current_action->remaining_ms;
    if(m_current_action->remaining_ms <= 0)
    {
        vibes_enqueue_custom_pattern(m_vibration_pattern);
        m_current_action_index++;
        if(m_current_action_index < m_actions.used)
        {
            m_current_action = &m_actions.array[m_current_action_index];
        } else {
            stop_breathing();
            if(use_auto_kill())
            {
                exit_reason_set(APP_EXIT_ACTION_PERFORMED_SUCCESSFULLY);
                window_stack_remove(m_main_window, true);
            } else {
                reset_breathing();
            }
        }
    }
}

static void update_action_bar_icons()
{
    GBitmap* middle_icon = m_running ? get_pause_icon() : get_play_icon();

    action_bar_layer_set_icon_animated(m_action_bar, BUTTON_ID_UP, get_swap_icon(), true);
    action_bar_layer_set_icon_animated(m_action_bar, BUTTON_ID_SELECT, middle_icon, true);
    action_bar_layer_set_icon_animated(m_action_bar, BUTTON_ID_DOWN, get_config_icon(), true);
}

static void schedule_main_layer_refresh()
{
    bool rescheduled = false;
    if(m_refresh_timer != NULL)
    {
        rescheduled = app_timer_reschedule(m_refresh_timer, refresh_interval_ms);
    }
    if(!rescheduled)
    {
        m_refresh_timer = app_timer_register(refresh_interval_ms, refresh_main_layer, NULL);
    }

}

static void cancel_main_layer_refresh()
{
    if(m_refresh_timer != NULL)
    {
        app_timer_cancel(m_refresh_timer);
        m_refresh_timer = NULL;
    }
}

static void refresh_main_layer(void* data)
{
    m_refresh_timer = NULL;
    layer_mark_dirty(m_main_layer);
    schedule_main_layer_refresh();
}


void start_breathing()
{
    m_running = true;
    tick_timer_service_subscribe(SECOND_UNIT, on_sec_tick);
    update_action_bar_icons();
    light_enable(true);
    schedule_main_layer_refresh();
}

static void stop_breathing()
{
    m_running = false;
    tick_timer_service_unsubscribe();
    update_action_bar_icons();
    light_enable(false);
    cancel_main_layer_refresh();
}


void goto_config_window(ClickRecognizerRef recognizer, void* context)
{
    stop_breathing();
    setup_config_menu_window();
}

void toggle_running(ClickRecognizerRef recognizer, void* context)
{
    m_running ? stop_breathing() : start_breathing();
}

void toggle_exercise(ClickRecognizerRef recognizer, void* context)
{
    // TODO: Add more exercises
    reset_breathing();
}

void reset_breathing()
{
    debug_seed();
    m_current_action_index = 0;
    m_current_action = &m_actions.array[m_current_action_index];
    stop_breathing();
    layer_mark_dirty(m_main_layer);
}

void setup_layers(
    Layer* main_layer,
    ActionBarLayer* action_bar,
    StatusBarLayer* status_bar,
    Window* main_window)
{
    m_main_layer = main_layer;

    GRect bounds = layer_get_bounds(main_layer);
    m_circle_center = GPoint((bounds.origin.x + bounds.size.w)/2 - 2, (bounds.origin.y + bounds.size.h)/2 - 10);
    m_main_layer_text_area = GRect(bounds.origin.x, bounds.size.h - bounds.origin.y - 20 - 8, bounds.size.w, 20);
    m_text_font = fonts_get_system_font(FONT_KEY_GOTHIC_18);

    m_circle_empty_rect = GRect(m_circle_center.x - MIN_BREATH_CIRCLE_RADIUS, m_circle_center.y - MIN_BREATH_CIRCLE_RADIUS, MIN_BREATH_CIRCLE_RADIUS * 2, MIN_BREATH_CIRCLE_RADIUS * 2);
    m_circle_full_rect = GRect(m_circle_center.x - MAX_BREATH_CIRCLE_RADIUS, m_circle_center.y - MAX_BREATH_CIRCLE_RADIUS, MAX_BREATH_CIRCLE_RADIUS * 2, MAX_BREATH_CIRCLE_RADIUS * 2);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "empty: x:%d y:%d, w:%d, h:%d", m_circle_empty_rect.origin.x, m_circle_empty_rect.origin.y, m_circle_empty_rect.size.w, m_circle_empty_rect.size.h);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "full: x:%d y:%d, w:%d, h:%d", m_circle_full_rect.origin.x, m_circle_full_rect.origin.y, m_circle_full_rect.size.w, m_circle_full_rect.size.h);

    m_action_bar = action_bar;
    m_status_bar = status_bar;
    m_main_window = main_window;
}

void update_main_window(Window *window)
{
    window_set_background_color(window, get_background_color());
    status_bar_layer_set_colors(m_status_bar, get_background_color(), get_foreground_color());
    action_bar_layer_set_background_color(m_action_bar, get_foreground_color());

    update_action_bar_icons();

    layer_mark_dirty(m_main_layer);
}

void update_main_layer(struct Layer *layer, GContext *ctx)
{
    if(m_current_action != NULL)
    {
        float progress_procentage = 1 - (float)(m_current_action->original_ms - m_current_action->animation_ms) / ((float)m_current_action->original_ms);
        graphics_context_set_fill_color(ctx, get_foreground_color());
        graphics_context_set_text_color(ctx, get_foreground_color());
        if(m_running)
        {
            m_current_action->animation_ms += refresh_interval_ms;
        }
        switch (m_current_action->type)
        {
            case BreatheIn:
            {
                uint8_t radius = ((MAX_BREATH_CIRCLE_RADIUS - MIN_BREATH_CIRCLE_RADIUS) * progress_procentage) + MIN_BREATH_CIRCLE_RADIUS;
                APP_LOG(APP_LOG_LEVEL_DEBUG, "radius: %d, progress_procentage: %d", radius, (int)(progress_procentage * 100));
                graphics_fill_circle(ctx, m_circle_center, radius);
                if(m_running)
                {
                    graphics_draw_text(ctx, BREATH_IN_TEXT, m_text_font, m_main_layer_text_area, GTextOverflowModeFill, GTextAlignmentCenter, NULL);
                }
                break;
            }
            case BreatheOut:
            {
                uint8_t radius = ((MAX_BREATH_CIRCLE_RADIUS - MIN_BREATH_CIRCLE_RADIUS) * (1 - progress_procentage)) + MIN_BREATH_CIRCLE_RADIUS;
                APP_LOG(APP_LOG_LEVEL_DEBUG, "radius: %d, progress_procentage: %d", radius, (int)(progress_procentage * 100));
                graphics_fill_circle(ctx, m_circle_center, radius);
                if(m_running)
                {
                    graphics_draw_text(ctx, BREATH_OUT_TEXT, m_text_font, m_main_layer_text_area, GTextOverflowModeFill, GTextAlignmentCenter, NULL);
                }
                break;
            }
            case HoldEmptyBreath:
            {
                int32_t start_angle = progress_procentage * 360;
                APP_LOG(APP_LOG_LEVEL_DEBUG, "start_angle: %d", (int)start_angle);
                graphics_fill_radial(ctx, m_circle_empty_rect, GOvalScaleModeFillCircle, MIN_BREATH_CIRCLE_RADIUS, DEG_TO_TRIGANGLE(start_angle), DEG_TO_TRIGANGLE(360));
                graphics_draw_text(ctx, "Hold Empty Breath", m_text_font, m_main_layer_text_area, GTextOverflowModeFill, GTextAlignmentCenter, NULL);
                break;
            }
            case HoldFullBreath:
            {
                int32_t start_angle = progress_procentage * 360;
                APP_LOG(APP_LOG_LEVEL_DEBUG, "start_angle: %d", (int)start_angle);
                graphics_fill_radial(ctx, m_circle_full_rect, GOvalScaleModeFillCircle, MAX_BREATH_CIRCLE_RADIUS, DEG_TO_TRIGANGLE(start_angle), DEG_TO_TRIGANGLE(360));
                graphics_draw_text(ctx, "Hold Full Breath", m_text_font, m_main_layer_text_area, GTextOverflowModeFill, GTextAlignmentCenter, NULL);
                break;
            }
            default:
                break;
        }
    }
}