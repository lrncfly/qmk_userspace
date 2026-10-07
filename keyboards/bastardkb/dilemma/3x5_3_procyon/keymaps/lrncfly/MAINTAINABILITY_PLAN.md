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
|      |           |                                 |              |

## Priority 1: Clarify module boundaries

- [ ] **Intent gate:** Trace what the screen is expected to display, which half
  owns it, and what it should do when split state or a data source is
  unavailable. Confirm those expectations before changing dependencies.
- [ ] Define and record the screen module's agreed responsibilities: display
  hardware, widgets, and rendering.
- [ ] Identify the data the screen needs from the keymap, Argos, and the
  pointing-device module; confirm the intended source and update semantics for
  each value.
- [ ] After confirming the desired ownership, add a small explicit interface or
  adapter for layer names/colors and device metrics instead of having the
  screen module directly depend on those implementations.
- [ ] Confirm the screen module can be built without accidentally relying on
  include-order or unrelated module internals, while preserving the agreed
  runtime behavior.

## Priority 2: Extract chord tracking and formatting

- [ ] **Intent gate:** Confirm chord semantics before extracting them: what is
  a chord, when does it complete or expire, modifier ordering, history
  retention, and how taps/layer-taps should appear.
- [ ] Trace and record the current behavior for shifted characters, Caps Lock,
  modifier taps, layer taps, simultaneous events, and non-printable key names.
- [ ] After confirmation, separate modifier-order tracking, chord completion,
  and chord history from display rendering without changing those semantics.
- [ ] Move US-layout keycode-to-character conversion and keycode text
  formatting into the chord component, preserving the confirmed layout rules.
- [ ] Keep LVGL object creation and updates in the screen/display component;
  define a small interface for passing formatted chord and history updates.
- [ ] Add focused tests or testable cases for the confirmed chord behavior.

## Priority 3: Remove or complete unused screen-module switching

- [ ] **Intent gate:** Ask whether module switching or the currently unused
  module hooks are planned functionality, external integration points, or
  abandoned scaffolding.
- [ ] Trace callers, build integration, and any expected future module behavior;
  record which interfaces must remain stable.
- [ ] If switching is not required, remove only the confirmed-unused module
  IDs, callback-table fields, declarations, and `set_current_module()` stub.
- [ ] If switching is required, agree on expected lifecycle and event behavior
  before implementing loading, initialization, dispatch, and housekeeping.
- [ ] Remove declarations in `screen.h` only after confirming they have no
  caller or external contract.

## Priority 4: Consolidate layer and RGB configuration

- [ ] **Intent gate (RGB design):** Map the RGB behavior across the keymap
  palette, keymap indicator callback, Argos defaults and EEPROM entries,
  keyboard LED flags, RGB Matrix settings, split synchronization, and screen
  color display. Summarize the current behavior and ask Quentin to confirm the
  intended color for each layer, which LEDs/zones receive it, how modifiers
  override it, brightness behavior, and the role of Argos customization.
- [ ] Record the confirmed RGB design in the decision log before consolidating
  configuration. Do not replace Argos settings or palette values merely to
  make duplicated values agree.
- [ ] **Intent gate (layers):** Confirm the meaning of each layer, the reserved
  menu layer, dynamic-keymap capacity, and any intentional unused layer slots.
- [ ] After confirmation, define layer IDs and count in one authoritative
  place, with the reserved/menu layer related to that set.
- [ ] Keep layer names and per-layer RGB palette together or expose them
  through a single layer metadata interface, preserving the confirmed design.
- [ ] Confirm the physical meaning of LED zones and modifier indicators before
  centralizing their boundaries in hardware-specific configuration.
- [ ] Check whether keyboard LED metadata can replace literal LED indices
  without changing which LEDs are affected.
- [ ] Confirm the intended LED count and its source before revisiting
  `RGB_MATRIX_LED_COUNT` or other dimensions that might be derivable.

## Priority 5: Specify Argos RGB defaults and displayed-color semantics

- [ ] **Intent gate:** Confirm default colors per layer, whether keymap palette
  should seed Argos defaults, how existing EEPROM customizations should behave,
  and whether defaults should change after a firmware update.
- [ ] Trace Argos initialization, EEPROM load/write, JSON defaults, and
  per-LED entries; record which source is authoritative in each lifecycle
  state.
- [ ] **Intent gate:** Confirm how the LCD should represent a layer when its
  per-LED colors differ: a designated underglow LED, a common layer color, or
  another representative color.
- [ ] Make the agreed choice explicit in the Argos color API rather than
  relying on the first entry implicitly.
- [ ] Preserve existing EEPROM customizations unless Quentin explicitly
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
