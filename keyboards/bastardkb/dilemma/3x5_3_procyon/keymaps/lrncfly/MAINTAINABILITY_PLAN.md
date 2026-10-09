# `lrncfly` Keymap Maintainability Plan

This checklist tracks the maintainability improvements identified during the
code review. Work from top to bottom unless a later item becomes a prerequisite
for a specific change. Keep each change focused and verify the keymap build and
behavior after each structural step.

## Working agreement

The implementation details and intent behind some existing behavior are not
fully documented and may be distributed across the keymap, modules, and JSON
configuration. Treat current behavior as intentional until its purpose is
understood.

For **each item that is about to change**, use this sequence:

1. Trace the relevant behavior across the keymap, modules, compile-time
   configuration, and persisted/default data as appropriate.
2. Summarize what the code currently does and identify the specific behavior
   the proposed change could affect.
3. Ask Quentin to confirm the intended behavior and any relevant constraints.
   Ask about one focused decision at a time; do not infer intent from the
   current implementation alone.
4. Record the confirmed intent and rationale in the decision log below before
   implementation. If the answer changes the plan, revise the plan first.
5. Implement only the agreed scope, then verify both the intended behavior and
   that unrelated behavior is preserved.

Do not bundle several behavior decisions into one broad refactor. For a purely
structural change, still confirm the behavior that must remain invariant before
moving code.

## Decision log

Record confirmed decisions here as work proceeds. Include enough detail that a
future change can distinguish deliberate behavior from an implementation
accident.

| Date | Plan item | Confirmed intent / constraints | Verification |
|------|-----------|---------------------------------|--------------|
| 2026-10-08 | Priority 1: screen module boundaries | The physical left/LCD half alone initializes and renders the dashboard. On the base layer, show `SECONDARY` when that half is not the current master. Preserve current data sources and precedence: local QMK state, Argos layer RGB when available followed by the keymap palette, and pointing-mode DPI. | External firmware build succeeded and the user verified the behavior on the keyboard. |
| 2026-10-08 | Priority 2: chord semantics | Any active non-base layer can prefix a chord; layer precedes modifiers, which precede terminal keys. Held-context strokes are shown and recorded as a sequence (e.g. `CTRL + K CTRL + 0`); context changes end a sequence. Same-timestamp terminal key events form one simultaneous chord. Standalone dual-role taps are shown/recorded; ordinary unmodified keys are not. Preserve observed modifier order (Ctrl/Shift/Alt/GUI tie-break), US/QWERTY formatting with Shift/Caps Lock XOR, and QMK key-name fallback abbreviations such as `SPC`. Keep the 5-second display/sequence idle timeout (reset per stroke), three-entry history, 20-second history expiry, and zero-value config controls. | External firmware build succeeded and the user confirmed it ran successfully on the keyboard. |
| 2026-10-08 | Brightness defaults and RGB ceiling | Default LCD backlight to level 10 of 16 and RGB Matrix brightness to 96; reduce the configured RGB maximum from 176 to 96. Keep existing EEPROM-saved settings intact; these defaults apply when settings are initialized or EEPROM is cleared, while the new RGB maximum limits values applied through QMK brightness controls. | Build and keyboard verification pending. |
| 2026-10-09 | Priority 2: compact layer sequences and modified key display | For repeated strokes in the same active non-base layer, show the layer and physically held modifier context once, followed by only the terminal keys (e.g. `NUMERAL` then `1 2 3`); apply this to live overlay and history. Preserve full per-stroke context formatting for modifier-only sequences. Include active weak/one-shot modifiers in the context, but treat modifiers encoded in an individual QMK modified keycode as that key's output formatting rather than held context, so shifted symbols on one layer remain a single sequence. | Build and keyboard verification pending. |
| 2026-10-09 | Priority 2: standalone dual-role taps | Do not show or record standalone tap outputs from mod-tap/layer-tap keys (e.g. `S`, `D`, `SPC`, `TAB`, `ESC`). Preserve those tap outputs when they occur under an independently active modifier or non-base layer context. Continue displaying keys used with a held modifier/layer role. | Build and keyboard verification pending. |
| 2026-10-09 | Priority 3: screen lifecycle | Keep the dashboard's direct keymap lifecycle (init/load, key processing, housekeeping); module switching/pomodoro support is not planned. Remove its unused dispatch table, IDs, no-op selector, and stale dispatch/sync declarations. Leave the unrelated no-op layer-state callback for the later dormant-keymap-code review. | Source search found no callers; external firmware build and deployment succeeded. |
| 2026-10-09 | Priority 4/5: RGB defaults and indicators | Keep the existing layer palette. On first Argos initialization, seed only underglow LEDs for non-base layers; leave base and key LEDs uncustomized, and do not overwrite existing EEPROM settings. Use the first metadata-marked underglow LED for the LCD layer color. Restrict modifier indicators to LEDs 10, 11, 12, 47, 48, and 49, assigning active Shift, Ctrl, Alt, and GUI colors in that priority order. On BASE, do not paint the underglow layer black so normal RGB effects remain visible. | Implementation complete; external build and keyboard verification pending. |
| 2026-10-09 | Priority 4: layer metadata and capacity | Keep seven active keymap layers at IDs 0–6, reserve ID 7 for VIA/menu, and retain dynamic-keymap capacity of 8. Define IDs/count/reserved slot together and expose names and palette colors from one metadata table. Keep the 72-LED count explicit as keyboard hardware configuration, with `RGBLIGHT_LED_COUNT` derived from it. | Build pending. |

## Priority 1: Clarify module boundaries

- [x] **Intent gate:** Trace what the screen is expected to display, which half
  owns it, and what it should do when split state or a data source is
  unavailable. Confirm those expectations before changing dependencies.
- [x] Define and record the screen module's agreed responsibilities: display
  hardware, widgets, and rendering.
- [x] Identify the data the screen needs from the keymap, Argos, and the
  pointing-device module; confirm the intended source and update semantics for
  each value.
- [x] After confirming the desired ownership, add a small explicit interface or
  adapter for layer names/colors and device metrics instead of having the
  screen module directly depend on those implementations.
- [x] Confirm the screen module can be built without accidentally relying on
  include-order or unrelated module internals, while preserving the agreed
  runtime behavior.

## Priority 2: Extract chord tracking and formatting

- [x] **Intent gate:** Confirm chord semantics before extracting them: what is
  a chord, when does it complete or expire, modifier ordering, history
  retention, and how taps/layer-taps should appear.
- [x] Trace and record the current behavior for shifted characters, Caps Lock,
  modifier taps, layer taps, simultaneous events, and non-printable key names.
- [x] After confirmation, separate modifier-order tracking, chord completion,
  and chord history from display rendering without changing those semantics.
- [x] Move US-layout keycode-to-character conversion and keycode text
  formatting into the chord component, preserving the confirmed layout rules.
- [x] Keep LVGL object creation and updates in the screen/display component;
  define a small interface for passing formatted chord and history updates.
- [ ] Add focused tests or testable cases for the confirmed chord behavior.
- [x] Verify with an external firmware build and keyboard tests: firmware built
  and ran successfully on the keyboard.
- [ ] Confirm the detailed behavior checks: standalone
  dual-role taps; modifier ordering; layer + modifier + key formatting;
  same-context sequences and 5-second reset; context changes; simultaneous
  same-timestamp keys; history capacity, expiry, and zero-value settings.

## Priority 3: Remove or complete unused screen-module switching

- [x] **Intent gate:** Ask whether module switching or the currently unused
  module hooks are planned functionality, external integration points, or
  abandoned scaffolding.
- [x] Trace callers, build integration, and any expected future module behavior;
  record which interfaces must remain stable.
- [x] If switching is not required, remove only the confirmed-unused module
  IDs, callback-table fields, declarations, and `set_current_module()` stub.
- [x] Remove declarations in `screen.h` only after confirming they have no
  caller or external contract.
- [x] Verify the firmware build and dashboard behavior after removal.

## Priority 4: Consolidate layer and RGB configuration

- [x] **Intent gate (RGB design):** Map the RGB behavior across the keymap
  palette, keymap indicator callback, Argos defaults and EEPROM entries,
  keyboard LED flags, RGB Matrix settings, split synchronization, and screen
  color display. Summarize the current behavior and ask Quentin to confirm the
  intended color for each layer, which LEDs/zones receive it, how modifiers
  override it, brightness behavior, and the role of Argos customization.
- [x] Record the confirmed RGB design in the decision log before consolidating
  configuration. Do not replace Argos settings or palette values merely to
  make duplicated values agree.
- [x] **Intent gate (layers):** Confirm the meaning of each layer, the reserved
  menu layer, dynamic-keymap capacity, and any intentional unused layer slots.
- [x] After confirmation, define layer IDs and count in one authoritative
  place, with the reserved/menu layer related to that set.
- [x] Keep layer names and per-layer RGB palette together or expose them
  through a single layer metadata interface, preserving the confirmed design.
- [x] Confirm the physical meaning of LED zones and modifier indicators before
  centralizing their boundaries in hardware-specific configuration.
- [x] Check whether keyboard LED metadata can replace literal LED indices
  without changing which LEDs are affected. It identifies underglow LEDs but
  does not distinguish the confirmed thumb cluster, so those six indices stay
  explicit in the keymap.
- [x] Confirm the intended LED count and its source before revisiting
  `RGB_MATRIX_LED_COUNT` or other dimensions that might be derivable. The
  Procyon hardware LED map contains 72 entries; keep this explicit hardware
  dimension and derive `RGBLIGHT_LED_COUNT` from it.

## Priority 5: Specify Argos RGB defaults and displayed-color semantics

- [x] **Intent gate:** Confirm default colors per layer, whether keymap palette
  should seed Argos defaults, how existing EEPROM customizations should behave,
  and whether defaults should change after a firmware update.
- [x] Trace Argos initialization, EEPROM load/write, JSON defaults, and
  per-LED entries; record which source is authoritative in each lifecycle
  state. First-time Argos setup seeds the EEPROM; later startup loads the saved
  entries without reseeding.
- [x] **Intent gate:** Confirm how the LCD should represent a layer when its
  per-LED colors differ: a designated underglow LED, a common layer color, or
  another representative color.
- [x] Make the agreed choice explicit in the Argos color API rather than
  relying on the first entry implicitly.
- [x] Preserve existing EEPROM customizations unless Quentin explicitly
  confirms a migration/reset behavior.
- [ ] Verify agreed behavior for defaults, custom colors, disabled entries,
  passthrough entries, and split synchronization on both halves.

## Priority 6: Prune dormant keymap code

- [ ] **Intent gate:** Ask whether console text macros and RGB LED debugging
  are intentional tools that should remain available, and how the user expects
  to enable and operate them.
- [ ] If retained, isolate debug keycodes/state behind an agreed debug build
  option and document its activation behavior.
- [ ] Otherwise remove only the confirmed-unused debug code, globals, and
  custom keycodes.
- [ ] **Intent gate:** Confirm whether the no-op `layer_state_set_user()`
  callback or its commented screen-routing behavior represents planned
  functionality.
- [ ] Remove the callback/commented behavior only if no active or planned
  behavior needs it.
- [ ] Prefix remaining custom keycodes with an agreed keymap-specific name,
  preserving key positions and behaviors.

## Priority 7: Small layout/header cleanup

- [ ] **Intent gate:** Confirm whether the empty `keymap.h` is intentionally
  reserved for generated code, tooling, or future use.
- [ ] Remove it and its include only if no such dependency exists.
- [ ] Confirm the intended owner and users of the `RGB` declaration in
  `layers.h`; then make the header self-contained or move the declaration to
  an appropriate QMK-aware header.
- [ ] Keep the layer layout macros together unless they are reused or become
  difficult to navigate. Before splitting them, confirm the desired reuse and
  ownership rather than splitting solely to reduce file length.

## Verification checklist

- [ ] Before behavior-affecting tests, confirm expected outcomes with Quentin
  and add them to the decision log.
- [ ] Build `bastardkb/dilemma/3x5_3_procyon:lrncfly`.
- [ ] Confirm all seven keymap layers and the dynamic/menu layer configuration
  behave as intended.
- [ ] Test split operation with the LCD half primary and with the other half
  primary, confirming intended display ownership and status.
- [ ] Test LCD layer labels/colors, chord display/history, RGB enabled/disabled,
  and Argos per-layer custom colors against the confirmed RGB design.
- [ ] Confirm user-saved Argos RGB settings survive firmware updates and are
  not overwritten by default initialization.
