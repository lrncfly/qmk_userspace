/**
 * Copyright 2022 Charly Delay <charly@codesink.dev> (@0xcharly)
 * Copyright 2023 casuanoob <casuanoob@hotmail.com> (@casuanoob)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include QMK_KEYBOARD_H
#include "config.h"
#include "layers.h"
#include "screen.h"
#include "argos_rgb.h"

enum lrncfly_keycodes {
    LRNCFLY_LCD_BRIGHTNESS_UP = SAFE_RANGE,
    LRNCFLY_LCD_BRIGHTNESS_DOWN,
};

const uint16_t PROGMEM combo4[] = {KC_V, KC_B, COMBO_END};

combo_t key_combos[] = {COMBO(combo4, KC_RBRC)};
// Automatically enable sniping-mode on the pointer layer.
// #define DILEMMA_AUTO_SNIPING_ON_LAYER LAYER_POINTER

#define ESC_MED LT(LAYER_MEDIA, KC_ESC)
#define SPC_NAV LT(LAYER_NAVIGATION, KC_SPC)
#define TAB_FUN LT(LAYER_FUNCTION, KC_TAB)
#define ENT_SYM LT(LAYER_SYMBOLS, KC_ENT)
#define BSP_NUM LT(LAYER_NUMERAL, KC_BSPC)
#define SPC_NUM LT(LAYER_NUMERAL, KC_SPC)
#define _L_PTR(KC) LT(LAYER_POINTER, KC)

#include "keymap.h"

// clang-format off
/** \brief QWERTY layout (3 rows, 10 columns). */
#define LAYOUT_LAYER_BASE                                                                     \
        KC_Q, KC_W, KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,          KC_I,   KC_O,    KC_P, \
        KC_A, KC_S, KC_D,    KC_F,    KC_G,    KC_H,    KC_J,          KC_K,   KC_L, KC_QUOT, \
        KC_Z, KC_X, KC_C,    KC_V,    KC_B,    KC_N,    KC_M,       KC_COMM, KC_DOT, KC_SLSH, \
                 ESC_MED, TAB_FUN, SPC_NAV, ENT_SYM, BSP_NUM, LGUI_T(KC_DEL)

/** Convenience row shorthands. */
#define _______________DEAD_HALF_ROW_______________ XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX
#define ______________HOME_ROW_GACS_L______________ XXXXXXX, KC_LALT, KC_LCTL, KC_LSFT, XXXXXXX
#define ______________HOME_ROW_GACS_R______________ XXXXXXX, KC_LSFT, KC_LCTL, KC_LALT, XXXXXXX

/*
 * Layers used on the Dilemma.
 *
 * These layers started off heavily inspired by the Miryoku layout, but trimmed
 * down and tailored for a stock experience that is meant to be fundation for
 * further personalization.
 *
 * See https://github.com/manna-harbour/miryoku for the original layout.
 */

/**
 * \brief Function layer.
 *
 * Secondary right-hand layer has function keys mirroring the numerals on the
 * primary layer with extras on the pinkie column, plus system keys on the inner
 * column. App is on the tertiary thumb key and other thumb keys are duplicated
 * from the base layer to enable auto-repeat.
 */
#define LAYOUT_LAYER_FUNCTION                                                                 \
    _______________DEAD_HALF_ROW_______________, KC_PSCR,   KC_F7,   KC_F8,   KC_F9,  KC_F12, \
    ______________HOME_ROW_GACS_L______________, KC_SCRL,   KC_F4,   KC_F5,   KC_F6,  KC_F11, \
    _______________DEAD_HALF_ROW_______________, KC_PAUS,   KC_F1,   KC_F2,   KC_F3,  KC_F10, \
                      XXXXXXX, _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX

/**
* \brief Media layer.
*
* Tertiary left- and right-hand layer is media and RGB control.  This layer is
* symmetrical to accomodate the left- and right-hand trackball.
*/
#define LAYOUT_LAYER_MEDIA                                                                    \
    RM_VALD, RM_PREV, RM_TOGG, RM_NEXT, RM_VALU, RM_SATD, RM_PREV, RM_TOGG, RM_NEXT, RM_SATU, \
    LRNCFLY_LCD_BRIGHTNESS_DOWN, KC_VOLD, KC_MUTE, KC_VOLU, LRNCFLY_LCD_BRIGHTNESS_UP, KC_MPRV, KC_VOLD, KC_MUTE, KC_VOLU, KC_MNXT, \
    _______________DEAD_HALF_ROW_______________, _______________DEAD_HALF_ROW_______________, \
                      _______, KC_MPLY, KC_MSTP, KC_MSTP, KC_MPLY, KC_MUTE

/** \brief Mouse emulation and pointer functions. */
#define LAYOUT_LAYER_POINTER                                                                  \
    QK_BOOT,  EE_CLR, XXXXXXX, DPI_MOD, S_D_MOD, S_D_MOD, DPI_MOD, XXXXXXX,  EE_CLR, QK_BOOT, \
    ______________HOME_ROW_GACS_L______________, ______________HOME_ROW_GACS_R______________, \
    _______, DRGSCRL, SNIPING, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, SNIPING, DRGSCRL, _______, \
                      MS_BTN3, MS_BTN2, MS_BTN1, MS_BTN1, MS_BTN2, MS_BTN3

/**
 * \brief Navigation layer.
 *
 * Primary right-hand layer (left home thumb) is navigation and editing. Cursor
 * keys are on the home position, line and page movement below, clipboard above,
 * caps lock and insert on the inner column. Thumb keys are duplicated from the
 * base layer to avoid having to layer change mid edit and to enable auto-repeat.
 */
#define LAYOUT_LAYER_NAVIGATION                                                               \
    _______________DEAD_HALF_ROW_______________, _______________DEAD_HALF_ROW_______________, \
    ______________HOME_ROW_GACS_L______________, CW_TOGG, KC_LEFT,   KC_UP, KC_DOWN, KC_RGHT, \
    KC_UNDO, KC_CUT, KC_COPY, KC_PASTE, XXXXXXX,  KC_INS, KC_HOME, KC_PGUP, KC_PGDN,  KC_END, \
                     XXXXXXX,  XXXXXXX, _______,  XXXXXXX, XXXXXXX, XXXXXXX

/**
* \brief Numeral layout.
*
* Primary left-hand layer (right home thumb) is numerals and symbols. Numerals
* are in the standard numpad locations with symbols in the remaining positions.
* `KC_DOT` is duplicated from the base layer.
*/
#define LAYOUT_LAYER_NUMERAL                                                                 \
    KC_LBRC, KC_7, KC_8,   KC_9, KC_RBRC, _______________DEAD_HALF_ROW_______________, \
    KC_SCLN, KC_4, KC_5,   KC_6,  KC_EQL, ______________HOME_ROW_GACS_R______________, \
    KC_GRV,  KC_1, KC_2,   KC_3, KC_BSLS, _______________DEAD_HALF_ROW_______________, \
                   KC_0, KC_DOT, KC_MINS, XXXXXXX, _______, XXXXXXX

/**
* \brief Symbols layer.
*
* Secondary left-hand layer has shifted symbols in the same locations to reduce
* chording when using mods with shifted symbols. `KC_LPRN` is duplicated next to
* `KC_RPRN`.
*/
#define LAYOUT_LAYER_SYMBOLS                                                                  \
    KC_LCBR, KC_AMPR, KC_ASTR, KC_LPRN, KC_RCBR, _______________DEAD_HALF_ROW_______________, \
    KC_COLN,  KC_DLR, KC_PERC, KC_CIRC, KC_PLUS, ______________HOME_ROW_GACS_R______________, \
    KC_TILD, KC_EXLM,   KC_AT, KC_HASH, KC_PIPE, _______________DEAD_HALF_ROW_______________, \
                      KC_RPRN,  KC_GRV, KC_UNDS, _______, XXXXXXX, XXXXXXX

/**
* \brief Add Home Row mod to a layout.
*
* Expects a 10-key per row layout.  Adds support for GACS (Gui, Alt, Ctl, Shift)
* home row.  The layout passed in parameter must contain at least 20 keycodes.
*
* This is meant to be used with `LAYER_ALPHAS_QWERTY` defined above, eg.:
*
*     HOME_ROW_MOD_GACS(LAYER_ALPHAS_QWERTY)
*/
#define _HOME_ROW_MOD_GACS(                                   \
    L00, L01, L02, L03, L04, R05, R06, R07, R08, R09,         \
    L10, L11, L12, L13, L14, R15, R16, R17, R18, R19,         \
    ...)                                                      \
             L00,         L01,         L02,         L03, L04, \
             R05,         R06,         R07,         R08, R09, \
             L10, LALT_T(L11), LCTL_T(L12), RSFT_T(L13), L14, \
             R15, RSFT_T(R16), LCTL_T(R17), LALT_T(R18), R19, \
      __VA_ARGS__
#define HOME_ROW_MOD_GACS(...) _HOME_ROW_MOD_GACS(__VA_ARGS__)

/**
* \brief Add pointer layer keys to a layout.
*
* Expects a 10-key per row layout.  The layout passed in parameter must contain
* at least 30 keycodes.
*
* This is meant to be used with `LAYER_ALPHAS_QWERTY` defined above, eg.:
*
*     POINTER_MOD(LAYER_ALPHAS_QWERTY)
*/
#define _POINTER_MOD(                                                   \
    L00, L01, L02, L03, L04, R05, R06, R07, R08, R09,                   \
    L10, L11, L12, L13, L14, R15, R16, R17, R18, R19,                   \
    L20, L21, L22, L23, L24, R25, R26, R27, R28, R29,                   \
    ...)                                                                \
              L00, L01, L02, L03, L04, R05, R06, R07, R08,        R09,  \
              L10, L11, L12, L13, L14, R15, R16, R17, R18,        R19,  \
      _L_PTR(L20), L21, L22, L23, L24, R25, R26, R27, R28, _L_PTR(R29), \
      __VA_ARGS__
#define POINTER_MOD(...) _POINTER_MOD(__VA_ARGS__)

#define LAYOUT_wrapper(...) LAYOUT_split_3x5_3(__VA_ARGS__)

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [LAYER_BASE]       = LAYOUT_wrapper(POINTER_MOD(HOME_ROW_MOD_GACS(LAYOUT_LAYER_BASE))),
    [LAYER_FUNCTION]   = LAYOUT_wrapper(LAYOUT_LAYER_FUNCTION),
    [LAYER_NAVIGATION] = LAYOUT_wrapper(LAYOUT_LAYER_NAVIGATION),
    [LAYER_MEDIA]      = LAYOUT_wrapper(LAYOUT_LAYER_MEDIA),
    [LAYER_NUMERAL]    = LAYOUT_wrapper(LAYOUT_LAYER_NUMERAL),
    [LAYER_POINTER]    = LAYOUT_wrapper(LAYOUT_LAYER_POINTER),
    [LAYER_SYMBOLS]    = LAYOUT_wrapper(LAYOUT_LAYER_SYMBOLS),
};

// clang-format on

#ifdef ENCODER_MAP_ENABLE
// clang-format off
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
    [LAYER_BASE]       = {ENCODER_CCW_CW(MS_WHLD, MS_WHLU), ENCODER_CCW_CW(KC_VOLD, KC_VOLU)},
    [LAYER_FUNCTION]   = {ENCODER_CCW_CW(KC_DOWN, KC_UP),   ENCODER_CCW_CW(KC_LEFT, KC_RGHT)},
    [LAYER_NAVIGATION] = {ENCODER_CCW_CW(KC_PGDN, KC_PGUP), ENCODER_CCW_CW(KC_VOLU, KC_VOLD)},
    [LAYER_MEDIA]      = {ENCODER_CCW_CW(KC_PGDN, KC_PGUP), ENCODER_CCW_CW(KC_VOLU, KC_VOLD)},
    [LAYER_POINTER]    = {ENCODER_CCW_CW(RM_HUED, RM_HUEU), ENCODER_CCW_CW(RM_SATD, RM_SATU)},
    [LAYER_NUMERAL]    = {ENCODER_CCW_CW(RM_VALD, RM_VALU), ENCODER_CCW_CW(RM_SPDD, RM_SPDU)},
    [LAYER_SYMBOLS]    = {ENCODER_CCW_CW(RM_PREV, RM_NEXT), ENCODER_CCW_CW(KC_LEFT, KC_RGHT)},
};
// clang-format on
#endif // ENCODER_MAP_ENABLE

void keyboard_post_init_user(void) {
    if (is_keyboard_left()) {
        init_custom_dashboard();
        load_custom_dashboard();
    }
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed) {
        screen_note_activity();
    }

    switch (keycode) {
        case LRNCFLY_LCD_BRIGHTNESS_UP:
            if (record->event.pressed) {
                backlight_increase(); // Steps up by 1 (out of 16)
            }
            return false;
        case LRNCFLY_LCD_BRIGHTNESS_DOWN:
            if (record->event.pressed) {
                backlight_decrease(); // Steps down by 1 (out of 16)
            }
            return false;
    }
    screen_process_keycode(keycode, record);
    return true;
};

#ifdef RGB_MATRIX_ENABLE
static const uint8_t modifier_indicator_leds[] = {10, 11, 12, 47, 48, 49};

static HSV modifier_indicator_hsv(uint8_t mod) {
    if (mod & MOD_MASK_SHIFT) return (HSV){HSV_RED};
    if (mod & MOD_MASK_CTRL) return (HSV){HSV_BLUE};
    if (mod & MOD_MASK_ALT) return (HSV){HSV_GREEN};
    if (mod & MOD_MASK_GUI) return (HSV){HSV_WHITE};
    return (HSV){HSV_BLACK};
}

static RGB indicator_rgb(HSV hsv) {
    if (hsv.v > 0) {
        hsv.v = rgb_matrix_config.hsv.v;
    }
    return hsv_to_rgb(hsv);
}

RGB dilemma_layer_indicator_rgb(uint8_t layer) {
    return indicator_rgb(dilemma_layer_indicator_hsv(layer));
}

RGB argos_rgb_default_layer_color(uint8_t layer) {
    return hsv_to_rgb(dilemma_layer_indicator_hsv(layer));
}

bool rgb_matrix_indicators_advanced_keymap(uint8_t led_min, uint8_t led_max) {
    if (!rgb_matrix_is_enabled()) {
        return false;
    }

    const uint8_t layer = get_highest_layer(layer_state);
    const uint8_t mods  = get_mods() | get_oneshot_mods();
    const RGB     layer_rgb = dilemma_layer_indicator_rgb(layer);

    for (uint8_t i = led_min; i < led_max && i < RGB_MATRIX_LED_COUNT; i++) {
        if (g_led_config.flags[i] & LED_FLAG_UNDERGLOW) {
            if (layer != LAYER_BASE) {
                RGB_MATRIX_INDICATOR_SET_COLOR(i, layer_rgb.r, layer_rgb.g, layer_rgb.b);
            }
        }
    }

    static const uint8_t modifier_masks[] = {MOD_MASK_SHIFT, MOD_MASK_CTRL, MOD_MASK_ALT, MOD_MASK_GUI};
    uint8_t              indicator_index = 0;
    for (uint8_t i = 0; i < sizeof(modifier_masks) / sizeof(modifier_masks[0]); i++) {
        if (!(mods & modifier_masks[i])) {
            continue;
        }
        if (indicator_index >= sizeof(modifier_indicator_leds) / sizeof(modifier_indicator_leds[0])) {
            break;
        }

        const uint8_t led = modifier_indicator_leds[indicator_index++];
        const RGB      rgb = indicator_rgb(modifier_indicator_hsv(modifier_masks[i]));
        if (g_led_config.flags[led] & LED_FLAG_UNDERGLOW) {
            RGB_MATRIX_INDICATOR_SET_COLOR(led, rgb.r, rgb.g, rgb.b);
        }
    }

    return false;
}
#endif

void housekeeping_task_user(void) {
    housekeeping_custom_dashboard();
}

/*                                                                      \
 * Left Side (Indices 0-35)                 Right Side (Indices 36-71)  \
 * [00][01][02][03][04]                     [36][37][38][39][40]        \
 * [05][06][07][08][09]                     [41][42][43][44][45]        \
 * [10][11][12][13][14]                     [46][47][48][49][50]        \
 *         [15][16][17]                     [51][52][53]                \
 *                                                                      \
 * Underglow (Example placement)                                        \
 * [18][19]...[35]                          [54][55]...[71]             \
 */
// clang-format off
