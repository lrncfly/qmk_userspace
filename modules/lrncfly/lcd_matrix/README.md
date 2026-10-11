# LCD Matrix

This userspace module renders a logical 12-by-16 RGB matrix on the Dilemma
Procyon's ST7789 display. It is an LCD presentation of matrix-style indicators,
not a WS2812 driver. The LED strip module and its GPIO timing requirements are
not used.

The Procyon vendor keymap enables this module with `bastardkb/lcd`. QMK's
community-module dispatcher calls its lifecycle, key-processing, and pointing
device hooks. `LCD_MATRIX_TOGGLE` (alias `LCDMX`) switches between the existing
status dashboard and the matrix page. It is intentionally not assigned to a key
by default.

Layer and modifier state are sent through the normal split synchronization
features; pointer mode, page selection, and trackball movement use the
`LCD_MATRIX_SYNC` transaction. The module post-config merges that transaction
with Argos RGB and LCD status transactions without changing either upstream
module.
