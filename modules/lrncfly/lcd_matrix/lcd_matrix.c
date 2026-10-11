#include QMK_KEYBOARD_H

#include <stdio.h>
#include <string.h>

#include "argos_rgb.h"
#include "bk_pointing_modes.h"
#include "lcd_matrix.h"
#include "lvgl.h"
#include "transactions.h"

#define LCD_MATRIX_COLS 12
#define LCD_MATRIX_ROWS 16
#define LCD_MATRIX_CELLS (LCD_MATRIX_COLS * LCD_MATRIX_ROWS)
#define LCD_MATRIX_CANVAS_COLS (LCD_MATRIX_COLS * 2)
#define LCD_MATRIX_CANVAS_ROWS (LCD_MATRIX_ROWS * 2)
#define LCD_MATRIX_CANVAS_PIXELS (LCD_MATRIX_CANVAS_COLS * LCD_MATRIX_CANVAS_ROWS)
#define LCD_MATRIX_REFRESH_MS 100
#define LCD_MATRIX_SYNC_MS 80
#define LCD_MATRIX_SYNC_RETRY_MS 100
#define LCD_MATRIX_SYNC_HEARTBEAT_MS 500
#define LCD_MATRIX_TRAIL_FADE 24
#define LCD_MATRIX_MOTION_VISIBLE_MS 2000

#define LCD_MATRIX_CLAMP(value, min_value, max_value) ((value) < (min_value) ? (min_value) : ((value) > (max_value) ? (max_value) : (value)))

typedef struct {
    uint8_t layer;
    uint8_t mods;
    uint8_t pointer_mode;
    bool matrix_page;
    int16_t dx;
    int16_t dy;
} __attribute__((packed)) lcd_matrix_sync_t;

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} lcd_matrix_rgb_t;

static lv_obj_t *matrix_screen;
static lv_obj_t *matrix_caption;
static lv_obj_t *matrix_canvas;
static lv_obj_t *dashboard_screen;
static lcd_matrix_rgb_t frame[LCD_MATRIX_CELLS];
static lcd_matrix_rgb_t motion_trail[LCD_MATRIX_CELLS];
static lv_color_t canvas_buffer[LCD_MATRIX_CANVAS_PIXELS];
static uint8_t current_layer;
static uint8_t current_mods;
static uint8_t current_pointer_mode;
static bool matrix_page;
static bool lcd_ready;
static uint32_t last_render;
static uint32_t last_sync;
static uint32_t last_successful_sync;
static int32_t pending_dx;
static int32_t pending_dy;
static int16_t pointer_x = LCD_MATRIX_COLS / 2;
static int16_t pointer_y = LCD_MATRIX_ROWS / 2;
static uint32_t last_motion;

static const lcd_matrix_rgb_t layer_fallback_colors[] = {
    {255, 255, 255},
    {96, 190, 255},
    {190, 120, 255},
    {255, 170, 64},
    {80, 220, 190},
    {255, 100, 130},
    {220, 220, 80},
};

static lcd_matrix_rgb_t lcd_matrix_layer_color(uint8_t layer) {
    lcd_matrix_rgb_t color = layer_fallback_colors[layer % ARRAY_SIZE(layer_fallback_colors)];
#if defined(RGB_MATRIX_ENABLE) || defined(RGBLIGHT_ENABLE)
    RGB argos_color;
    argos_rgb_get_layer_color(layer, &argos_color);
    if (argos_color.r || argos_color.g || argos_color.b) {
        color = (lcd_matrix_rgb_t){argos_color.r, argos_color.g, argos_color.b};
    }
#endif
    return color;
}

static uint16_t lcd_matrix_index(uint8_t x, uint8_t y) {
    return (uint16_t)y * LCD_MATRIX_COLS + x;
}

static void lcd_matrix_pixel(uint8_t x, uint8_t y, lcd_matrix_rgb_t color) {
    if (x < LCD_MATRIX_COLS && y < LCD_MATRIX_ROWS) {
        frame[lcd_matrix_index(x, y)] = color;
    }
}

static void lcd_matrix_add_pixel(uint8_t x, uint8_t y, lcd_matrix_rgb_t color, uint8_t level) {
    if (x >= LCD_MATRIX_COLS || y >= LCD_MATRIX_ROWS) {
        return;
    }

    lcd_matrix_rgb_t *dst = &frame[lcd_matrix_index(x, y)];
    dst->r = MIN(255, dst->r + ((uint16_t)color.r * level / 255));
    dst->g = MIN(255, dst->g + ((uint16_t)color.g * level / 255));
    dst->b = MIN(255, dst->b + ((uint16_t)color.b * level / 255));
}

static void lcd_matrix_draw_layer_stack(void) {
    for (uint8_t layer = 0; layer < 7; layer++) {
        const uint8_t y = 2 + layer * 2;
        lcd_matrix_rgb_t color = lcd_matrix_layer_color(layer);
        if (layer != current_layer) {
            color.r /= 3;
            color.g /= 3;
            color.b /= 3;
        }
        for (uint8_t x = 0; x < (layer == current_layer ? LCD_MATRIX_COLS : LCD_MATRIX_COLS - 3); x++) {
            lcd_matrix_pixel(x, y, color);
        }
    }

    const lcd_matrix_rgb_t active = lcd_matrix_layer_color(current_layer);
    for (uint8_t y = 5; y < 11; y++) {
        lcd_matrix_pixel(5, y, active);
        lcd_matrix_pixel(6, y, active);
    }
    for (uint8_t x = 4; x < 8; x++) {
        lcd_matrix_pixel(x, 4, active);
        lcd_matrix_pixel(x, 11, active);
    }
}

static void lcd_matrix_draw_modifiers(void) {
    const lcd_matrix_rgb_t color = lcd_matrix_layer_color(current_layer);
    const uint8_t modifiers[] = {MOD_MASK_CTRL, MOD_MASK_SHIFT, MOD_MASK_ALT, MOD_MASK_GUI};
    uint8_t slot = 0;

    for (uint8_t i = 0; i < ARRAY_SIZE(modifiers); i++) {
        if ((current_mods & modifiers[i]) == 0) {
            continue;
        }
        const uint8_t x0 = 2 + (slot % 2) * 5;
        const uint8_t y0 = 4 + (slot / 2) * 7;
        for (uint8_t x = 0; x < 3; x++) {
            lcd_matrix_pixel(x0 + x, y0, color);
            lcd_matrix_pixel(x0 + x, y0 + 4, color);
        }
        for (uint8_t y = 0; y < 5; y++) {
            lcd_matrix_pixel(x0, y0 + y, color);
        }
        lcd_matrix_pixel(x0 + 2, y0 + 2 + slot % 3, color);
        slot++;
    }
}

static void lcd_matrix_draw_pointer_mode(void) {
    const lcd_matrix_rgb_t color = lcd_matrix_layer_color(current_layer);
    const uint8_t center_x = LCD_MATRIX_COLS / 2;
    const uint8_t center_y = LCD_MATRIX_ROWS / 2;

    if (current_pointer_mode == MODE_DRAGSCROLL) {
        for (uint8_t i = 0; i < 4; i++) {
            lcd_matrix_pixel(center_x, center_y - 3 + i * 2, color);
            lcd_matrix_pixel(center_x - 1, center_y - 2 + i * 2, color);
            lcd_matrix_pixel(center_x + 1, center_y - 2 + i * 2, color);
        }
        return;
    }

    for (int8_t offset = -4; offset <= 4; offset++) {
        if (offset != -1 && offset != 0 && offset != 1) {
            lcd_matrix_pixel(center_x + offset, center_y, color);
            lcd_matrix_pixel(center_x, center_y + offset, color);
        }
    }
    for (uint8_t y = center_y - 1; y <= center_y + 1; y++) {
        for (uint8_t x = center_x - 1; x <= center_x + 1; x++) {
            lcd_matrix_pixel(x, y, color);
        }
    }
}

static bool lcd_matrix_fade_motion_trail(void) {
    bool visible = false;
    for (uint16_t i = 0; i < LCD_MATRIX_CELLS; i++) {
        lcd_matrix_rgb_t *trail = &motion_trail[i];
        trail->r = trail->r > LCD_MATRIX_TRAIL_FADE ? trail->r - LCD_MATRIX_TRAIL_FADE : 0;
        trail->g = trail->g > LCD_MATRIX_TRAIL_FADE ? trail->g - LCD_MATRIX_TRAIL_FADE : 0;
        trail->b = trail->b > LCD_MATRIX_TRAIL_FADE ? trail->b - LCD_MATRIX_TRAIL_FADE : 0;
        visible = visible || trail->r != 0 || trail->g != 0 || trail->b != 0;
    }
    return visible;
}

static void lcd_matrix_draw_motion(void) {
    const lcd_matrix_rgb_t color = lcd_matrix_layer_color(current_layer);
    const uint32_t elapsed = timer_elapsed32(last_motion);
    for (uint16_t i = 0; i < LCD_MATRIX_CELLS; i++) {
        lcd_matrix_add_pixel(i % LCD_MATRIX_COLS, i / LCD_MATRIX_COLS, motion_trail[i], 255);
    }

    if (elapsed < 1200) {
        lcd_matrix_add_pixel(pointer_x, pointer_y, color, (uint8_t)(255 - MIN(220, elapsed / 5)));
        if (pointer_x > 0) {
            lcd_matrix_add_pixel(pointer_x - 1, pointer_y, color, 100);
        }
    }
}

static void lcd_matrix_draw_idle(void) {
    static const char duck[5][8] = {
        ".OOO...",
        "*O#O...",
        "..OO..O",
        ".OOOOOO",
        "..OOOO.",
    };
    const lcd_matrix_rgb_t water = {25, 90, 150};
    const lcd_matrix_rgb_t body = {220, 180, 30};
    const lcd_matrix_rgb_t beak = {255, 120, 35};
    const uint8_t shift = (timer_read32() / 700) % 4;

    for (uint8_t x = 0; x < LCD_MATRIX_COLS; x++) {
        lcd_matrix_pixel(x, 1, (x + shift) % 4 == 0 ? (lcd_matrix_rgb_t){100, 210, 255} : water);
        lcd_matrix_pixel(x, 2, water);
        lcd_matrix_pixel(x, 3, water);
    }
    for (uint8_t y = 0; y < 5; y++) {
        for (uint8_t x = 0; x < 7; x++) {
            const char pixel = duck[y][x];
            lcd_matrix_rgb_t color;
            switch (pixel) {
                case 'O':
                    color = body;
                    break;
                case '*':
                    color = beak;
                    break;
                case '#':
                    color = (lcd_matrix_rgb_t){0, 0, 0};
                    break;
                default:
                    continue;
            }
            lcd_matrix_pixel(2 + x, 5 + y, color);
        }
    }
}

static const char *lcd_matrix_layer_name(uint8_t layer) {
    static const char *const names[] = {"BASE", "FUNCTION", "NAVIGATION", "MEDIA", "POINTER", "NUMERAL", "SYMBOLS"};
    return layer < ARRAY_SIZE(names) ? names[layer] : "LAYER";
}

static const char *lcd_matrix_mods_name(void) {
    static char caption[32];
    static const struct {
        uint8_t mask;
        const char *name;
    } names[] = {
        {MOD_MASK_CTRL, "CTRL"},
        {MOD_MASK_SHIFT, "SHIFT"},
        {MOD_MASK_ALT, "ALT"},
        {MOD_MASK_GUI, "GUI"},
    };

    size_t length = 0;
    caption[0] = '\0';
    for (uint8_t i = 0; i < ARRAY_SIZE(names); i++) {
        if ((current_mods & names[i].mask) == 0) {
            continue;
        }
        const int written = snprintf(caption + length, sizeof(caption) - length, "%s%s", length ? " " : "", names[i].name);
        if (written < 0 || (size_t)written >= sizeof(caption) - length) {
            break;
        }
        length += written;
    }
    return caption;
}

static const char *lcd_matrix_pointer_name(uint8_t mode) {
    static const char *const names[] = {"POINTER", "SNIPING", "SCROLL", "CURSOR", "BRIGHTNESS", "ZOOM", "VOLUME", "TABS", "HISTORY"};
    return mode < ARRAY_SIZE(names) ? names[mode] : "POINTER";
}

static void lcd_matrix_set_caption(const char *text) {
    if (matrix_caption != NULL) {
        lv_label_set_text(matrix_caption, text);
    }
}

static void lcd_matrix_render(void) {
    if (!lcd_ready || matrix_screen == NULL || timer_elapsed32(last_render) < LCD_MATRIX_REFRESH_MS) {
        return;
    }
    last_render = timer_read32();
    memset(frame, 0, sizeof(frame));
    const bool motion_trail_visible = lcd_matrix_fade_motion_trail();

    if (current_pointer_mode != MODE_NORMAL) {
        lcd_matrix_draw_pointer_mode();
        lcd_matrix_set_caption(lcd_matrix_pointer_name(current_pointer_mode));
    } else if (current_mods != 0) {
        lcd_matrix_draw_modifiers();
        lcd_matrix_set_caption(lcd_matrix_mods_name());
    } else if (current_layer != 0) {
        lcd_matrix_draw_layer_stack();
        lcd_matrix_set_caption(lcd_matrix_layer_name(current_layer));
    } else if (timer_elapsed32(last_motion) < LCD_MATRIX_MOTION_VISIBLE_MS || motion_trail_visible) {
        lcd_matrix_draw_motion();
        lcd_matrix_set_caption("BASE TRACKBALL");
    } else {
        lcd_matrix_draw_idle();
        lcd_matrix_set_caption("BASE");
    }

    memset(canvas_buffer, 0, sizeof(canvas_buffer));
    for (uint8_t y = 0; y < LCD_MATRIX_ROWS; y++) {
        for (uint8_t x = 0; x < LCD_MATRIX_COLS; x++) {
            const lcd_matrix_rgb_t color = frame[lcd_matrix_index(x, y)];
            const lv_color_t lv_color = lv_color_make(color.r, color.g, color.b);
            const uint16_t canvas_x = x * 2;
            const uint16_t canvas_y = (LCD_MATRIX_ROWS - 1 - y) * 2;
            canvas_buffer[canvas_y * LCD_MATRIX_CANVAS_COLS + canvas_x] = lv_color;
            canvas_buffer[canvas_y * LCD_MATRIX_CANVAS_COLS + canvas_x + 1] = lv_color;
            canvas_buffer[(canvas_y + 1) * LCD_MATRIX_CANVAS_COLS + canvas_x] = lv_color;
            canvas_buffer[(canvas_y + 1) * LCD_MATRIX_CANVAS_COLS + canvas_x + 1] = lv_color;
        }
    }
    lv_obj_invalidate(matrix_canvas);
}

static void lcd_matrix_show_page(bool show_matrix) {
    matrix_page = show_matrix;
    if (!lcd_ready) {
        return;
    }
    lv_scr_load(show_matrix ? matrix_screen : dashboard_screen);
    if (show_matrix) {
        last_render = 0;
        lcd_matrix_render();
    }
}

static void lcd_matrix_receive_sync(uint8_t in_len, const void *in_data, uint8_t out_len, void *out_data) {
    (void)out_len;
    (void)out_data;
    if (in_len != sizeof(lcd_matrix_sync_t)) {
        return;
    }
    const lcd_matrix_sync_t *sync = (const lcd_matrix_sync_t *)in_data;
    const bool page_changed = matrix_page != sync->matrix_page;
    current_layer = sync->layer;
    current_mods = sync->mods;
    current_pointer_mode = sync->pointer_mode;
    if (page_changed) {
        lcd_matrix_show_page(sync->matrix_page);
    }
    if (sync->dx != 0 || sync->dy != 0) {
        const int16_t dx = sync->dx;
        const int16_t dy = sync->dy;
        pointer_x = LCD_MATRIX_CLAMP(pointer_x + dx / 8, 0, LCD_MATRIX_COLS - 1);
        pointer_y = LCD_MATRIX_CLAMP(pointer_y - dy / 8, 0, LCD_MATRIX_ROWS - 1);
        last_motion = timer_read32();
        motion_trail[lcd_matrix_index(pointer_x, pointer_y)] = lcd_matrix_layer_color(current_layer);
    }
}

void keyboard_post_init_lcd_matrix_user(void) {
    transaction_register_rpc(LCD_MATRIX_SYNC, lcd_matrix_receive_sync);

    if (!is_keyboard_left()) {
        return;
    }

    dashboard_screen = lv_scr_act();
    matrix_screen = lv_obj_create(NULL);
    lv_obj_remove_style_all(matrix_screen);
    lv_obj_set_size(matrix_screen, LCD_WIDTH, LCD_HEIGHT);
    lv_obj_set_style_bg_color(matrix_screen, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(matrix_screen, LV_OPA_COVER, LV_PART_MAIN);

    matrix_caption = lv_label_create(matrix_screen);
    lv_obj_set_style_text_color(matrix_caption, lv_color_white(), LV_PART_MAIN);
    lv_obj_align(matrix_caption, LV_ALIGN_TOP_MID, 0, 2);
    matrix_canvas = lv_canvas_create(matrix_screen);
    lv_canvas_set_buffer(matrix_canvas, canvas_buffer, LCD_MATRIX_CANVAS_COLS, LCD_MATRIX_CANVAS_ROWS, LV_IMG_CF_TRUE_COLOR);
    lv_img_set_zoom(matrix_canvas, 2048);
    lv_img_set_antialias(matrix_canvas, false);
    lv_obj_align(matrix_canvas, LV_ALIGN_CENTER, 0, 12);

    lcd_ready = true;
    lcd_matrix_show_page(false);
}

void housekeeping_task_lcd_matrix_user(void) {
    if (is_keyboard_master()) {
        current_layer = get_highest_layer(layer_state);
        current_mods = get_mods();
        current_pointer_mode = bkpd_mode_get_active_id();

        const uint32_t elapsed = timer_elapsed32(last_sync);
        const bool has_motion = pending_dx != 0 || pending_dy != 0;
        const bool heartbeat_due = timer_elapsed32(last_successful_sync) >= LCD_MATRIX_SYNC_HEARTBEAT_MS;
        if (!is_keyboard_left() && (has_motion || heartbeat_due || elapsed >= LCD_MATRIX_SYNC_MS)) {
            const lcd_matrix_sync_t sync = {
                .layer = current_layer,
                .mods = current_mods,
                .pointer_mode = current_pointer_mode,
                .matrix_page = matrix_page,
                .dx = LCD_MATRIX_CLAMP(pending_dx, INT16_MIN, INT16_MAX),
                .dy = LCD_MATRIX_CLAMP(pending_dy, INT16_MIN, INT16_MAX),
            };
            if (transaction_rpc_send(LCD_MATRIX_SYNC, sizeof(sync), &sync)) {
                pending_dx -= sync.dx;
                pending_dy -= sync.dy;
                last_successful_sync = timer_read32();
            } else if (timer_elapsed32(last_sync) < LCD_MATRIX_SYNC_RETRY_MS) {
                return;
            }
            last_sync = timer_read32();
        }
    }

    if (is_keyboard_left() && matrix_page) {
        lcd_matrix_render();
    }
}

bool process_record_lcd_matrix_user(uint16_t keycode, keyrecord_t *record) {
    if (keycode == LCD_MATRIX_TOGGLE) {
        if (record->event.pressed && is_keyboard_master()) {
            lcd_matrix_show_page(!matrix_page);
        }
        return false;
    }
    return true;
}

report_mouse_t pointing_device_task_lcd_matrix_user(report_mouse_t mouse_report) {
    int16_t dx = mouse_report.x;
    int16_t dy = mouse_report.y;
    if (dx == 0 && dy == 0) {
        dx = mouse_report.h;
        dy = mouse_report.v;
    }
    if (dx != 0 || dy != 0) {
        if (is_keyboard_left()) {
            pointer_x = LCD_MATRIX_CLAMP(pointer_x + dx / 8, 0, LCD_MATRIX_COLS - 1);
            pointer_y = LCD_MATRIX_CLAMP(pointer_y - dy / 8, 0, LCD_MATRIX_ROWS - 1);
            last_motion = timer_read32();
            motion_trail[lcd_matrix_index(pointer_x, pointer_y)] = lcd_matrix_layer_color(current_layer);
        } else {
            pending_dx = LCD_MATRIX_CLAMP(pending_dx + dx, INT16_MIN, INT16_MAX);
            pending_dy = LCD_MATRIX_CLAMP(pending_dy + dy, INT16_MIN, INT16_MAX);
        }
    }
    return mouse_report;
}
