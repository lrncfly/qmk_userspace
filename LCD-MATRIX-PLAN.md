# LCD Matrix Plan

## Goal

Repurpose the visual behaviors of BastardKB's LED matrix for the ST7789 LCD
used by the Dilemma Procyon, without using the WS2812 strip backend or changing
the third-party BastardKB submodule.

The LCD keeps its existing status dashboard as the default page. A key-toggle
switches to a dedicated matrix page that presents layer, modifier, pointer,
trackball-motion, and idle visuals.

## Decisions

- [x] Keep the existing dashboard and show matrix visuals on a separate,
  key-toggleable page.
- [x] Implement the LCD-specific feature as a userspace module at
  `modules/lrncfly/lcd_matrix`.
- [x] Leave `modules/bastardkb` unchanged; reuse visual intent and behavior,
  not the WS2812 transport implementation.
- [x] Scope initial integration to
  `bastardkb/dilemma/3x5_3_procyon:vendor`.
- [x] Expose a toggle keycode and leave it unbound for user assignment.

## Implementation checklist

### 1. Module and target integration

- [x] Add `modules/lrncfly/lcd_matrix` following the userspace's QMK module
  conventions (`qmk_module.json`, module headers/source, and post-config
  declarations as needed).
- [x] Add only the required matrix/LCD module declarations to the Procyon
  vendor keymap; do not change unrelated keymaps.
- [x] Implement the module's initialization, housekeeping, and key-processing
  hooks through QMK's generated community-module dispatcher.
- [x] Keep the existing LCD module's third-party sources and RPC payload
  unchanged.

### 2. LCD page and rendering

- [x] Add a matrix page and a toggle keycode; keep the dashboard as the startup
  page.
- [x] Render a logical 12-by-16 RGB matrix using the existing LCD/LVGL display
  path, scaled and positioned to fit the 240-by-280 display.
- [x] Reuse existing rendering resources where practical; avoid adding a
  second full-screen framebuffer without measuring RP2040 RAM use.
- [x] Pace matrix refreshes so rendering does not unnecessarily consume CPU
  time or affect keyboard responsiveness.
- [x] Return cleanly to the dashboard without losing its current status
  updates or theme behavior.

### 3. Visual behavior

- [x] Reproduce the LED matrix's indicator priority on the LCD matrix page.
- [x] Map the Procyon keymap's layers to appropriate names: Base, Function,
  Navigation, Media, Pointer, Numeral, and Symbols.
- [x] Render layer colors consistently with the existing Argos layer-color
  behavior where available.
- [x] Adapt held-modifier and pointer-mode indicators to the LCD presentation.
- [x] Adapt trackball-reactive animation and an idle animation for the LCD.
- [x] Keep display logic independent of the LED strip pin, WS2812 timing, and
  interrupt-disabled sender.

### 4. Split state

- [x] Verify which half initializes and owns the LCD and which half has the
  required layer, modifier, and pointer state.
- [x] Use existing split-synchronized state where available.
- [x] Add a separate, narrowly scoped split RPC only for state the display half
  cannot otherwise obtain; do not alter the LCD module's existing RPC message.
- [ ] Verify synchronization after startup and after a missed or failed sync.

### 5. Validation

- [ ] Build `bastardkb/dilemma/3x5_3_procyon:vendor`.
- [x] Measure proxy-build firmware size and linker-reported heap/RAM usage;
  exact-target and runtime headroom still require validation.
- [ ] Verify dashboard startup, page toggling in both directions, and
  continued dashboard status/theme updates.
- [ ] Verify all seven layer names, modifier indicators, pointer modes,
  trackball animations, and idle behavior.
- [ ] Verify split operation with the LCD on its intended half, including
  state changes originating on the other half.
- [x] Confirm no unrelated keymaps or BastardKB submodule files were changed.

## Notes

- The existing LED matrix painters output a 12-by-16 RGB frame, making them a
  useful behavioral reference. Their current display transport is specifically
  for a WS2812 LED strip and is not the LCD rendering path.
- The matrix module's built-in layer-name table targets a different Dilemma
  layer layout. The Procyon layer names must be supplied explicitly.
- The current checked-out QMK firmware `main` does not contain the
  `3x5_3_procyon` keyboard source, so the exact target build is not currently
  available in this workspace. A Dilemma `3x5_3` proxy build succeeded with
  `-Wno-error` to accommodate pre-existing upstream LCD/Argos and
  keyboard/module macro warnings. Its linker map reports a 0x9cc4-byte
  (about 39.2 KiB) heap region; this is not a runtime free-heap measurement and
  the proxy is not a substitute for the exact target build or hardware testing.
- This document tracks implementation progress. Check items off as they are
  completed; update the plan if implementation reveals a necessary change in
  scope or design.
