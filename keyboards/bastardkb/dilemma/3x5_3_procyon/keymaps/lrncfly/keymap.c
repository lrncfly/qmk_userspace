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

#ifdef CONSOLE_ENABLE
#    include "print.h"
#endif

#include "modules/lrncfly/my_lcd/my_lcd.h"
enum custom_keycodes { QK_REG = SAFE_RANGE, QK_HELP, MY_DB_TOGG, MY_DB_STEP, LCD_BUP, LCD_BDN };

bool led_debug_enabled   = false;
int  current_debug_index = 0;

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
#define _L_LCD(KC) LT(LAYER_LCD, KC)
#define _L_PTR(KC) LT(LAYER_POINTER, KC)

#ifndef POINTING_DEVICE_ENABLE
#    define DRGSCRL KC_NO
#    define DPI_MOD KC_NO
#    define S_D_MOD KC_NO
#    define SNIPING KC_NO
#endif // !POINTING_DEVICE_ENABLE

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
    LCD_BDN, KC_VOLD, KC_MUTE, KC_VOLU, LCD_BUP, KC_MPRV, KC_VOLD, KC_MUTE, KC_VOLU, KC_MNXT, \
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

#define LAYOUT_LAYER_LCD                                                                           \
    _______,    LCD_BDN, LCD_BUP, QK_REG, MY_DB_TOGG, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, _______, \
    XXXXXXX, MY_DB_STEP,   LCDPR,  LCDNE,    XXXXXXX, QK_HELP, XXXXXXX,   LCDPR,   LCDNE, XXXXXXX, \
         _______________DEAD_HALF_ROW_______________, _______________DEAD_HALF_ROW_______________, \
                           XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX

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

/**
* \brief Add lcd layer keys to a layout.
*
* Expects a 10-key per row layout.  The layout passed in parameter must contain
* at least 30 keycodes.
*
* This is meant to be used with `LAYER_ALPHAS_QWERTY` defined above, eg.:
*
*     LCDMOD(LAYER_ALPHAS_QWERTY)
*/
#define _LCD_MOD(                                                      \
    L00, L01, L02, L03, L04, R05, R06, R07, R08, R09,                  \
    L10, L11, L12, L13, L14, R15, R16, R17, R18, R19,                  \
    L20, L21, L22, L23, L24, R25, R26, R27, R28, R29,                  \
    ...)                                                               \
     _L_LCD(L00), L01, L02, L03, L04, R05, R06, R07, R08, _L_LCD(R09), \
             L10, L11, L12, L13, L14, R15, R16, R17, R18,         R19, \
             L20, L21, L22, L23, L24, R25, R26, R27, R28,         R29, \
      __VA_ARGS__
#define LCD_MOD(...) _LCD_MOD(__VA_ARGS__)

#define LAYOUT_wrapper(...) LAYOUT_split_3x5_3(__VA_ARGS__)

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [LAYER_BASE]       = LAYOUT_wrapper(LCD_MOD(POINTER_MOD(HOME_ROW_MOD_GACS(LAYOUT_LAYER_BASE)))),
    [LAYER_FUNCTION]   = LAYOUT_wrapper(LAYOUT_LAYER_FUNCTION),
    [LAYER_NAVIGATION] = LAYOUT_wrapper(LAYOUT_LAYER_NAVIGATION),
    [LAYER_MEDIA]      = LAYOUT_wrapper(LAYOUT_LAYER_MEDIA),
    [LAYER_NUMERAL]    = LAYOUT_wrapper(LAYOUT_LAYER_NUMERAL),
    [LAYER_POINTER]    = LAYOUT_wrapper(LAYOUT_LAYER_POINTER),
    [LAYER_SYMBOLS]    = LAYOUT_wrapper(LAYOUT_LAYER_SYMBOLS),
    [LAYER_LCD]        = LAYOUT_wrapper(LAYOUT_LAYER_LCD),
};

// clang-format on
#ifdef POINTING_DEVICE_ENABLE
#    ifdef DILEMMA_AUTO_SNIPING_ON_LAYER
layer_state_t layer_state_set_user(layer_state_t state) {
    dilemma_set_pointer_sniping_enabled(layer_state_cmp(state, DILEMMA_AUTO_SNIPING_ON_LAYER));
    return state;
}
#    endif // DILEMMA_AUTO_SNIPING_ON_LAYER
#endif     // POINTING_DEVICE_ENABLE

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

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
#ifdef CONSOLE_ENABLE
        case QK_REG:
            if (record->event.pressed) {
                SEND_STRING_DELAY("Regards,\nQ. Lebastard\nBastard Keyboards", 5);
            }
            break;
        case QK_HELP:
            if (record->event.pressed) {
                SEND_STRING_DELAY("I hope this helps, let me know if there's anything else!", 5);
            }
            break;
        case MY_DB_TOGG:
            if (record->event.pressed) {
                led_debug_enabled = !led_debug_enabled;
                if (led_debug_enabled) {
                    uprintf("RGB Debug: ON\n");
                    rgb_matrix_enable_noeeprom();
                    // STOP the animation engine from drawing
                    rgb_matrix_mode_noeeprom(RGB_MATRIX_NONE);
                    rgb_matrix_set_color_all(0, 0, 0);
                    current_debug_index = 0;
                } else {
                    uprintf("RGB Debug: OFF\n");
                    rgb_matrix_mode_noeeprom(RGB_MATRIX_SOLID_COLOR);
                }
            }
            return false;
        case MY_DB_STEP:
            if (record->event.pressed && led_debug_enabled) {
                current_debug_index = (current_debug_index + 1) % RGB_MATRIX_LED_COUNT;

                // We still print to console here so you can see the log
                uprintf("Index: %d | Flags: %d\n", current_debug_index, g_led_config.flags[current_debug_index]);
            }
            return false;
#endif                        // CONSOLE_ENABLE
        case QK_BACKLIGHT_UP: // Or whatever your custom keycode is named
            if (record->event.pressed) {
                tap_code16(BL_UP); // Correct 16-bit quantum keycode handler
            }
            return false;

        case QK_BACKLIGHT_DOWN: // Or whatever your custom keycode is named
            if (record->event.pressed) {
                tap_code16(BL_DOWN); // Correct 16-bit quantum keycode handler
            }
            return false;
    }
    return true;
};

#ifdef RGB_MATRIX_ENABLE
bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    // underglow to honor RM_TOGG
    if (!rgb_matrix_is_enabled()) {
        return false;
    }

    uint8_t layer = get_highest_layer(layer_state);
    uint8_t mods  = get_mods() | get_oneshot_mods();

    // 1. Loop through all the LEDs to determine their color individualistically
    for (uint8_t i = led_min; i <= led_max; i++) {
        if (g_led_config.flags[i] == 2) { // Target only Underglow
            // Show modifiers beside screen/trackpad
            bool is_modifier_zone = (i >= 14 && i <= 17) || (i >= 50 && i <= 53);

            HSV hsv = (HSV){HSV_BLACK}; // Default to off

            // 2. If mods are active AND this specific LED is in the modifier
            // zone, use mod colors
            if (mods && is_modifier_zone) {
                if (mods & MOD_MASK_SHIFT)
                    hsv = (HSV){HSV_RED};
                else if (mods & MOD_MASK_CTRL)
                    hsv = (HSV){HSV_BLUE};
                else if (mods & MOD_MASK_ALT)
                    hsv = (HSV){HSV_GREEN};
                else if (mods & MOD_MASK_GUI)
                    hsv = (HSV){HSV_WHITE};
            }
            // 3. Otherwise, use standard layer colors for the underglow
            else {
                switch (layer) {
                        // clang-format off
                    case LAYER_FUNCTION:   hsv = (HSV){HSV_AZURE};       break;
                    case LAYER_NAVIGATION: hsv = (HSV){HSV_CHARTREUSE};  break;
                    case LAYER_MEDIA:      hsv = (HSV){HSV_CORAL};       break;
                    case LAYER_POINTER:    hsv = (HSV){HSV_CYAN};        break;
                    case LAYER_SYMBOLS:    hsv = (HSV){HSV_GOLD};        break;
                    case LAYER_NUMERAL:    hsv = (HSV){HSV_PINK};        break;
                    case LAYER_LCD:        hsv = (HSV){HSV_GOLDENROD};   break;
                        // clang-format on
                }
            }

            // 4. Apply global brightness matching BEFORE converting to RGB
            if (hsv.v > 0) {
                hsv.v = rgb_matrix_config.hsv.v;
            }

            RGB rgb = hsv_to_rgb(hsv);
            RGB_MATRIX_INDICATOR_SET_COLOR(i, rgb.r, rgb.g, rgb.b);
        }
    }

    return false;
}
#endif

layer_state_t layer_state_set_user(layer_state_t state) {
    if (is_keyboard_master() && is_keyboard_left()) {
        switch (get_highest_layer(state)) {
            case LAYER_NAVIGATION:
            case LAYER_POINTER:
                break;
            default:
                break;
        }
    }
    return state;
}

// 1. Declare your custom dashboard functions so keymap.c can see them
// void init_my_custom_dashboard(void);
void housekeeping_my_custom_dashboard(void);

// 2. Native QMK Boot Hook: Triggers right when the keyboard powers on
void keyboard_post_init_user(void) {
    // init_my_custom_dashboard();
}
// Use QMK's official automated display initialization callback
void lvgl_ui_init(void) {
    init_my_custom_dashboard();
}
// 3. Native QMK Loop Hook: Triggers continuously while you type to refresh the screen
void housekeeping_task_user(void) {
    housekeeping_my_custom_dashboard();
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
