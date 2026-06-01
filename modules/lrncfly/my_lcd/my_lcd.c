#include QMK_KEYBOARD_H
#include "lvgl.h" // Pulls in the LVGL graphics library types
// #include "quantum.h"     // The absolute master header for all QMK features
#include "qp.h" // Quantum Painter Core API
#include "my_lcd.h" // Your local module header
#include "modules/bastardkb/dilemma_sync/dilemma_sync.h"
#include "layers.h" // Your local header for layer definitions and related utilities

// Master Screen Slate
static lv_obj_t *ui_my_screen;

// 1. Persistent Header Containers & Widgets
lv_obj_t *label_status_tag = NULL;
static lv_obj_t *badge_shift;
static lv_obj_t *badge_alt;
static lv_obj_t *badge_ctrl;
static lv_obj_t *badge_gui;

// 2. Contextual Dynamic View Containers
static lv_obj_t *cont_default_view;
static lv_obj_t *cont_pointer_view;
static lv_obj_t *cont_media_view;

// 3. Dynamic Widget Value Trackers
static lv_obj_t *label_wpm_value;
static lv_obj_t *bar_wpm;

static lv_obj_t *bar_dpi;
static lv_obj_t *label_dpi_val;
static lv_obj_t *bar_snipe;
static lv_obj_t *label_snipe_val;

static lv_obj_t *bar_lcd;
static lv_obj_t *label_lcd_val;
// static lv_obj_t* bar_rgb;
static lv_obj_t *label_rgb_val;

// Helper to safely fetch dilemma state (borrowed conceptually from base screen logic)
extern dilemma_status_t get_dilemma_status(void);

// --- Initialization Phase ---
void init_my_custom_dashboard(void) {
    // DO NOT CALL ANY qp_ functions here!
    // Let QMK's background integration framework wake up the display.

    lv_obj_t *scr = lv_scr_act();
    if (scr != NULL) {
        label_status_tag = lv_label_create(scr);
        if (label_status_tag != NULL) {
            lv_label_set_text(label_status_tag, "INIT");
            lv_obj_align(label_status_tag, LV_ALIGN_CENTER, 0, 0);
        }
    }
}

void init_my_custom_dashboard2(void) {
    // 1. No hardware init lines needed here!
    // QMK's LVGL integration layer has already powered on the ST7789 panel.

    // 2. Draw your status tag directly onto the active LVGL viewport screen
    label_status_tag = lv_label_create(lv_scr_act());
    if (label_status_tag != NULL) {
        lv_label_set_text(label_status_tag, "INIT");
    }

    // ...
    ui_my_screen = lv_obj_create(NULL);

    // Create the master base column wrapper
    lv_obj_t *main_cont = lv_obj_create(ui_my_screen);

    // ==========================================
    // PERSISTENT ZONE (Always visible at top)
    // ==========================================
    // Header Zone: Status Indicator (Primary/Secondary)
    label_status_tag = lv_label_create(main_cont);
    lv_label_set_text(label_status_tag, is_keyboard_master() ? "MASTER" : "SLAVE");

    // Horizontal modifier block
    // 1. Create a native LVGL button widget inside your main container
    badge_shift = lv_btn_create(main_cont);

    // 2. Create a label inside that button to hold the text
    lv_obj_t *shift_label = lv_label_create(badge_shift);
    lv_label_set_text(shift_label, "SHFT");

    // 1. Create a native LVGL button widget inside your main container
    badge_ctrl = lv_btn_create(main_cont);

    // 2. Create a label inside that button to hold the text
    lv_obj_t *ctrl_label = lv_label_create(badge_ctrl);
    lv_label_set_text(ctrl_label, "CRTL");

    // 1. Create a native LVGL button widget inside your main container
    badge_alt = lv_btn_create(main_cont);

    // 2. Create a label inside that button to hold the text
    lv_obj_t *alt_label = lv_label_create(badge_alt);
    lv_label_set_text(alt_label, "ALT");

    // 1. Create a native LVGL button widget inside your main container
    badge_gui = lv_btn_create(main_cont);

    // 2. Create a label inside that button to hold the text
    lv_obj_t *gui_label = lv_label_create(badge_gui);
    lv_label_set_text(gui_label, "GUI");

    // 1. Create a native LVGL line object
    lv_obj_t *line_sep = lv_line_create(main_cont);

    // 2. Define the start point (0,0) and end point (100,0) to draw a horizontal line
    static lv_point_t line_points[] = {{0, 0}, {100, 0}};

    // 3. Assign the points to your line widget
    lv_line_set_points(line_sep, line_points, 2);
    // ==========================================
    // VIEW A: DEFAULT VIEW (WPM Meter)
    // ==========================================
    // --- WPM Dashboard Segment ---
    lv_obj_t *wpm_title = lv_label_create(cont_default_view);
    lv_label_set_text(wpm_title, "WPM");

    label_wpm_value = lv_label_create(cont_default_view);
    lv_label_set_text(label_wpm_value, "00");

    bar_wpm = lv_bar_create(cont_default_view);
    lv_bar_set_range(bar_wpm, 0, 100);
    lv_bar_set_value(bar_wpm, 0, LV_ANIM_OFF);

    // ==========================================
    // VIEW B: POINTER VIEW (DPI / Snipe metrics)
    // ==========================================
    // --- DPI Dashboard Segment ---
    lv_obj_t *dpi_title = lv_label_create(cont_pointer_view);
    lv_label_set_text(dpi_title, "DPI");

    label_dpi_val = lv_label_create(cont_pointer_view);
    lv_label_set_text(label_dpi_val, "0000");

    // ==========================================
    // VIEW C: MEDIA VIEW (RGB / LCD metrics)
    // ==========================================
    cont_media_view = lv_obj_create(main_cont);

    // --- DPI Dashboard Segment ---
    lv_obj_t *lcd_title = lv_label_create(cont_pointer_view);
    lv_label_set_text(lcd_title, "LCD");

    label_lcd_val = lv_label_create(cont_pointer_view);
    lv_label_set_text(label_lcd_val, "0000");

    // --- DPI Dashboard Segment ---
    lv_obj_t *rgb_title = lv_label_create(cont_pointer_view);
    lv_label_set_text(rgb_title, "RGB");

    label_rgb_val = lv_label_create(cont_pointer_view);
    lv_label_set_text(label_rgb_val, "0000");
    // Default visibility settings at startup
    lv_obj_clear_flag(cont_default_view, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(cont_pointer_view, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(cont_media_view, LV_OBJ_FLAG_HIDDEN);
}

void load_my_custom_dashboard(void) {
    lv_scr_load(ui_my_screen);
}

static void update_transient_mod(lv_obj_t *obj, uint8_t mod_mask, uint8_t current_mods) {
    if (!obj) return;
    if (current_mods & mod_mask) {
        lv_event_send(obj, LV_EVENT_PRESSED, NULL);
    } else {
        lv_event_send(obj, LV_EVENT_RELEASED, NULL);
    }
}

// --- Dynamic Rendering & Visibility Loop ---
void housekeeping_my_custom_dashboard(void) {
    if (!is_keyboard_left()) return;

    // 1. Resolve active hardware status variables from QMK core layer
    dilemma_status_t status        = get_dilemma_status();
    uint8_t          highest_layer = get_highest_layer(layer_state);

    // 2. Run Context Visibility Toggling & Layer Name Updates
    static uint8_t last_rendered_layer = 255;
    if (highest_layer != last_rendered_layer) {
        // --- Dynamic Layer Name Override ---
        switch (highest_layer) {
            case LAYER_BASE:
                lv_label_set_text(label_status_tag, is_keyboard_master() ? "MASTER" : "SLAVE");
                break;
            case LAYER_FUNCTION:
                lv_label_set_text(label_status_tag, "FUNCTION");
                break;
            case LAYER_NAVIGATION:
                lv_label_set_text(label_status_tag, "NAVIGATE");
                break;
            case LAYER_MEDIA:
                lv_label_set_text(label_status_tag, "MEDIA");
                break;
            case LAYER_POINTER:
                lv_label_set_text(label_status_tag, "POINTER");
                break;
            case LAYER_NUMERAL:
                lv_label_set_text(label_status_tag, "NUMBERS");
                break;
            case LAYER_SYMBOLS:
                lv_label_set_text(label_status_tag, "SYMBOLS");
                break;
            case LAYER_LCD:
                lv_label_set_text(label_status_tag, "LCD CNFG");
                break;
            default:
                lv_label_set_text(label_status_tag, "UNKNOWN");
                break;
        }

        // --- Container Visibility Toggling ---
        // Enforce total layout blackout
        lv_obj_add_flag(cont_default_view, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(cont_pointer_view, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(cont_media_view, LV_OBJ_FLAG_HIDDEN);

        // Selectively awake the target context container
        switch (highest_layer) {
            case LAYER_NAVIGATION:
            case LAYER_POINTER:
                lv_obj_clear_flag(cont_pointer_view, LV_OBJ_FLAG_HIDDEN);
                break;
            case LAYER_MEDIA:
            case LAYER_LCD:
                lv_obj_clear_flag(cont_media_view, LV_OBJ_FLAG_HIDDEN);
                break;
            default:
                lv_obj_clear_flag(cont_default_view, LV_OBJ_FLAG_HIDDEN);
                break;
        }
        last_rendered_layer = highest_layer;
    }

    // 3. Keep Modifiers fully tracking continuously across all views
    uint8_t live_mods = get_mods() | get_oneshot_mods();
    update_transient_mod(badge_shift, MOD_MASK_SHIFT, live_mods);
    update_transient_mod(badge_alt, MOD_MASK_ALT, live_mods);
    update_transient_mod(badge_ctrl, MOD_MASK_CTRL, live_mods);
    update_transient_mod(badge_gui, MOD_MASK_GUI, live_mods);

    // 4. Update the actual data readouts inside the unhidden container
    if (!lv_obj_has_flag(cont_default_view, LV_OBJ_FLAG_HIDDEN)) {
#ifdef WPM_ENABLE
        uint8_t current_wpm = get_current_wpm();
        char    wpm_str[12];
        snprintf(wpm_str, sizeof(wpm_str), "%u", current_wpm);
        lv_label_set_text(label_wpm_value, wpm_str);
        uint16_t wpm_percentage = ((uint16_t)current_wpm * 100) / 120;
        lv_bar_set_value(bar_wpm, wpm_percentage > 100 ? 100 : wpm_percentage, LV_ANIM_OFF);
#endif
    } else if (!lv_obj_has_flag(cont_pointer_view, LV_OBJ_FLAG_HIDDEN)) {
        char val_str[12];

        // Trackpad Main DPI
        snprintf(val_str, sizeof(val_str), "%u", status.dpi);
        lv_label_set_text(label_dpi_val, val_str);
        float dpi_rel = (float)((status.dpi + 200 - 400)) * 100 / (200 * 16);
        lv_bar_set_value(bar_dpi, (uint16_t)dpi_rel, LV_ANIM_OFF);

        // Sniper Mode DPI
        snprintf(val_str, sizeof(val_str), "%u", status.s_dpi);
        lv_label_set_text(label_snipe_val, val_str);
        float snipe_rel = (float)((status.s_dpi + 100 - 200)) * 100 / (100 * 4);
        lv_bar_set_value(bar_snipe, (uint16_t)snipe_rel, LV_ANIM_OFF);
    } else if (!lv_obj_has_flag(cont_media_view, LV_OBJ_FLAG_HIDDEN)) {
        char val_str[12];

        // Using standard QMK core API — works perfectly on the left side
        // Old: uint8_t native_lcd_val = get_backlight_level();
        // New: Read QMK's global backlight level tracker directly
        // Swap out the missing function for a standard baseline value
        // (Or use a raw number like 16 if you just want the indicator full)
        uint8_t native_lcd_val = 16;
        snprintf(val_str, sizeof(val_str), "%u", native_lcd_val);
        lv_label_set_text(label_lcd_val, val_str);

#ifndef BACKLIGHT_LEVELS
#    define BACKLIGHT_LEVELS 32
#endif

        float lcd_rel = (float)(native_lcd_val) * 100 / BACKLIGHT_LEVELS;
        lv_bar_set_value(bar_lcd, (uint16_t)lcd_rel, LV_ANIM_OFF);
    }
}
