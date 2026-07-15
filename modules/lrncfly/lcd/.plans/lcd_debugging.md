# QMK Hardware Debugging Context: BastardKB Dilemma Custom LCD Module

## Workspace Architecture & Status
- **Current State**: Successfully isolated the LCD driver into a standalone user module (`modules/lrncfly/lcd/`).
- **Compilation Status**: Passes all compilation checks successfully. Keymap compiles and links to a `.uf2` file.
- **The Issue**: The code compiles, but upon flashing, the keyboard/display is currently not functional ("blank screen or unresponsive hardware"). We are shifting from compilation fixing to active runtime hardware/firmware debugging.

---

## Workspace Directory Mapping
The project structure is organized as an external QMK Userspace repository:

```text
my-qmk-userspace/
|-- LICENSE
|-- Makefile
|-- README.md
|-- bastardkb_dilemma_3x5_3_procyon_lrncfly.uf2
|-- keyboards
|   `-- bastardkb
|       |-- charybdis       <-- unused
|       |-- dilemma
|       |   |-- 3x5_2       <-- unused
|       |   |-- 3x5_3       <-- unused
|       |   |-- 3x5_3_procyon
|       |   |   |-- keyboard.json
|       |   |   `-- keymaps
|       |   |       |-- lrncfly
|       |   |       |   |-- config.h
|       |   |       |   |-- flag_index.txt
|       |   |       |   |-- info.json
|       |   |       |   |-- keymap.c
|       |   |       |   |-- keymap.h
|       |   |       |   |-- keymap.json
|       |   |       |   |-- layers.h
|       |   |       |   |-- readme.md
|       |   |       |   |-- rgb_matrix_user.inc
|       |   |       |   |-- rules.mk
|       |   |       |   |-- todo.md
|       |   |       |   `-- viable.json
|       |   |       |-- vendor   <-- unused
|       |   |       `-- via      <-- unused
|       |   |-- 4x6_4            <-- unused
|       |   `-- 4x6_4_procyon    <-- unused
|       |-- scylla               <-- unused
|       |-- skeletyl             <-- unused
|       `-- tbkmini              <-- unused
|-- layouts
|-- modules
|   |-- bastardkb
|   |   `-- dilemma_sync
|   |       |-- dilemma_sync.c
|   |       |-- dilemma_sync.h
|   |       `-- qmk_module.json
|   `-- lrncfly
|       `-- lcd
|           |-- config.h
|           |-- halconf.h          <-- (Custom ChibiOS HAL settings with PWM enabled)
|           |-- introspection.h
|           |-- mcuconf.h
|           |-- my_lcd.c           <-- (Primary rendering & lifecycle logic)
|           |-- my_lcd.h           <-- (Contains custom lcd_module_t struct layout)
|           |-- qmk_module.json
|           |-- rules.mk           <-- (Quantum Painter + LVGL + PWM Backlight flags)
|           |-- theme.c            <-- (Ported upstream asset dependency)
|           |-- theme.h
|           |-- ui_elements.c      <-- (Ported upstream container/label utility wrapper)
|           `-- ui_elements.h
|-- qmk.json
`-- users
