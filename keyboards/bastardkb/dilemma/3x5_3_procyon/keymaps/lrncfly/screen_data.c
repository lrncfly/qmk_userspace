#include QMK_KEYBOARD_H
#include "layers.h"
#include "screen_data.h"

#ifdef RGB_MATRIX_ENABLE
#    include "argos_rgb.h"
#endif

#include "bk_pointing_device.h"

static const char *get_layer_name(uint8_t layer) {
    const char *name = dilemma_layer_name(layer);
    return name ? name : "UNKNOWN";
}

const char *lrncfly_screen_get_chord_layer_name(void) {
    uint8_t layer = get_highest_layer(layer_state);
    return layer == LAYER_BASE ? NULL : get_layer_name(layer);
}

void lrncfly_screen_get_dashboard_data(screen_dashboard_data_t *data) {
    *data = (screen_dashboard_data_t){0};
    data->layer = get_highest_layer(layer_state);
    data->layer_name = get_layer_name(data->layer);
    data->chord_layer_name = data->layer == LAYER_BASE ? NULL : data->layer_name;
    data->status_text = data->layer == LAYER_BASE && !is_keyboard_master() ? "SECONDARY" : data->layer_name;

    switch (data->layer) {
        case LAYER_NAVIGATION:
        case LAYER_POINTER:
            data->view = SCREEN_DASHBOARD_POINTER;
            break;
        case LAYER_MEDIA:
            data->view = SCREEN_DASHBOARD_MEDIA;
            break;
        default:
            data->view = SCREEN_DASHBOARD_DEFAULT;
            break;
    }

#ifdef RGB_MATRIX_ENABLE
    if (!argos_rgb_get_layer_color(data->layer, &data->layer_rgb)) {
        data->layer_rgb = dilemma_layer_indicator_rgb(data->layer);
    }
#endif

    switch (data->view) {
        case SCREEN_DASHBOARD_DEFAULT:
#ifdef WPM_ENABLE
            data->wpm = get_current_wpm();
#endif
            break;
        case SCREEN_DASHBOARD_POINTER:
            data->default_dpi = bkpd_mode_get_dpi(MODE_NORMAL);
            data->minimum_default_dpi = bkpd_get_minimum_default_dpi();
            data->maximum_default_dpi = bkpd_get_maximum_default_dpi();
            data->sniping_dpi = bkpd_mode_get_dpi(MODE_SNIPING);
            data->minimum_sniping_dpi = bkpd_get_minimum_sniping_dpi();
            data->maximum_sniping_dpi = bkpd_get_maximum_sniping_dpi();
            break;
        case SCREEN_DASHBOARD_MEDIA:
            data->lcd_brightness = get_backlight_level();
#ifdef RGB_MATRIX_ENABLE
            data->rgb_enabled = rgb_matrix_is_enabled();
            data->rgb_brightness = rgb_matrix_get_val();
#endif
            break;
    }
}
