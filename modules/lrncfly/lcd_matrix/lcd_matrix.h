#pragma once

#include QMK_KEYBOARD_H

void keyboard_post_init_lcd_matrix_user(void);
void housekeeping_task_lcd_matrix_user(void);
bool process_record_lcd_matrix_user(uint16_t keycode, keyrecord_t *record);
report_mouse_t pointing_device_task_lcd_matrix_user(report_mouse_t mouse_report);
