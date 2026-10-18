# Argos export to QMK mapping

This document maps [`json_config/argos_config.json`](./json_config/argos_config.json)
to the `lrncfly` QMK keymap and the Argos firmware modules. The export is a
snapshot of the keyboard's Argos/VIA configuration; it is not itself the QMK
keymap source. Some fields describe firmware capabilities or app state, while
others are stored in EEPROM and may differ from the checked-in defaults.

## Keymap coordinates and layers

The export reports `rows: 8`, `cols: 5`, and `keycodes` as eight arrays of 40
16-bit QMK keycodes. The inner array is flattened in matrix row-major order:

```text
keycodes[layer][row * cols + col]
```

Rows 0-3 describe the left half (top, home, bottom, thumb); rows 4-7 describe
the right half in the same order. The two unused thumb positions on each side
are zero. For example, layer 0 starts `20, 26, 8, 21, 23`, which is
`KC_Q, KC_W, KC_E, KC_R, KC_T` from the base layer in [`keymap.c`](./keymap.c).
The following home-row entries include `9238`, `8455`, and `8713`, the encoded
QMK mod-tap keycodes corresponding to `LALT_T(KC_S)`, `LCTL_T(KC_D)`, and
`LSFT_T(KC_F)`.

The export's `layerNames` indices line up with the layer enum in `keymap.c`:

| Export index | Export name | QMK layer |
|---:|---|---|
| 0 | `base` | `LAYER_BASE` |
| 1 | `function` | `LAYER_FUNCTION` |
| 2 | `navigation` | `LAYER_NAVIGATION` |
| 3 | `media` | `LAYER_MEDIA` |
| 4 | `pointer` | `LAYER_POINTER` |
| 5 | `numeral` | `LAYER_NUMERAL` |
| 6 | `symbols` | `LAYER_SYMBOLS` |
| 7 | `layer 7` | Reserved/dynamic layer; `config.h` sets `LAYER_MENU` to 7 |

The C keymap explicitly initializes layers 0-6. Layer 7 exists in the
8-layer dynamic/VIA configuration but has no named `LAYER_*` initializer in
this keymap. Preserve the array position and layer count if generating source
from the export. `0` is `KC_NO`; `1` is `KC_TRNS` (transparent).

## Field-by-field mapping

| JSON field(s) | QMK / Argos representation | How to treat it when codifying |
|---|---|---|
| `keycodes` | The dynamic keymap matrix, encoded as QMK 16-bit keycodes. The checked-in equivalents are the `LAYOUT_split_3x5_3` entries in `keymap.c`. | Translate numeric codes to QMK names/macros and keep the exported matrix coordinates/layer indices intact. |
| `layerNames` | Human-readable names for the dynamic layers; the first seven correspond to the `LAYER_*` enum in `keymap.c`. | Use as labels/comments, not as QMK keycodes. Layer 7 is reserved by this keymap's configuration. |
| `rows`, `cols` | Export matrix dimensions: 8 rows by 5 columns. | Structural metadata for interpreting `keycodes`; not a replacement for the physical layout macro. |
| `combos` | Sixteen Argos combo slots, each with `enabled`, `output`, four `input` keycodes, and `customTerm`. Stored in Argos EEPROM and exposed to QMK through `combo_get()` in [`argos_combo.c`](../../../../../../modules/bastardkb/argos/argos_combo.c). | To make combos compile-time QMK source, generate `combo_t`/`key_combos` entries and enable combos. This snapshot has no combo payloads; its entries have zero output/input/custom term, despite `enabled: true`. Do not emit those placeholders as actual combos. |
| `comboAmount`, `keysPerCombo` | Argos capacity constants: `ARGOS_COMBO_ENTRIES` = 16 and `ARGOS_KEYS_PER_COMBO` = 4. | Firmware capability metadata, not user combo definitions. |
| `tapDances` | Fifty Argos tap-dance entries with `on_tap`, `on_hold`, `on_double_tap`, `on_tap_hold`, and `custom_tapping_term`. Runtime entries are loaded from EEPROM by [`argos_tapdance.c`](../../../../../../modules/bastardkb/argos/argos_tapdance.c). | To codify these as static QMK, generate tap-dance action definitions and replace positions using them with the corresponding tap-dance keycodes. This snapshot has zero action payloads in all entries. |
| `tapDanceAmount` | Argos capacity constant `ARGOS_TAP_DANCE_ENTRIES` = 50. | Firmware capability metadata, not a count of configured tap dances. |
| `macros`, `macroCount`, `macroBufferSize` | VIA dynamic macro data and capacity metadata; the export has an empty `macros` array, 16 slots, and an 8192-byte buffer. | Not represented by this keymap's C `keymaps` array. Keep in VIA/dynamic macro storage or explicitly translate macro sequences to QMK macro code. |
| `tappingTerm`, `comboTerm` | Exported global timing values (200 ms and 50 ms in this snapshot). On first Argos initialization, the module seeds its stored global terms from QMK `TAPPING_TERM` and `COMBO_TERM`; it also stores later Argos changes in its EEPROM config. | For reproducible compile-time defaults, define the QMK terms in keymap configuration. Do not assume the exported runtime values are necessarily the current build's macros. |
| `themeId` | `argos_config.themeId`, stored in Argos EEPROM; the Argos module initializes it to 16. | Argos UI preference, not a QMK keymap setting. |
| `hasDisplayedWelcomeMessage` | `argos_config.has_displayed_welcome_message`, stored in Argos EEPROM. | App/on-device UI state; normally omit from source keymap behavior. |
| `languageId` | Argos web-app locale (`us` in this snapshot). | UI preference, not a firmware keymap setting. |
| `viaProtocolVersion`, `argosProtocolVersion`, `qmkKeycodesVersion` | Protocol/keycode compatibility metadata. Argos protocol version and compatible keycode version are declared in [`argos.h`](../../../../../../modules/bastardkb/argos/argos.h). | Build/export compatibility metadata; do not translate into keymap behavior. |
| `rgbBrightness`, `rgbEffectSpeed`, `rgbEffectType`, `rgbHue`, `rgbSat` | QMK RGB state values from the export. QMK effect numeric values depend on the firmware's effect definitions/version. | If making these fixed defaults, use the matching QMK RGB config/API rather than copying numeric effect IDs blindly. |
| `rgbMatrix` | Per-layer/per-LED records keyed by `"layer:row:col"` with `r`, `g`, `b`, `transparent`, `on`, and `custom`. These are Argos RGB overrides, stored by [`argos_rgb.c`](../../../../../../modules/bastardkb/argos/argos_rgb.c); `transparent` corresponds to the firmware's `passthrough` flag. | Keep as LED override data if reproducing Argos behavior. This snapshot contains 576 coordinate records (8 × 72), of which 540 are marked custom. It is separate from the keycode matrix. |
| `pointingDeviceType` | Argos device type enum; value 1 is `pointing_device_type_trackpad_procyon`. The Procyon pointing module reports that type for the digitizer driver. | Hardware/build identity, not a user keymap option. |
| `pointingModes` | Fourteen mode records (IDs 0-13) for normal, sniping, drag-scroll, cursor, brightness, zoom, volume, tab switch, history, and custom modes. Fields include `dpi`, `minimumDpi`, `dpiPerStep`, `maxDpi`, `invertX/Y`, custom direction keycodes, and `autoActivateLayer`. These correspond to `MODE_*` and `bkpd_config_t` in [`bk_pointing_modes.h`](../../../../../../modules/bastardkb/bk_pointing_device/bk_pointing_modes.h) and [`bk_pointing_device.h`](../../../../../../modules/bastardkb/bk_pointing_device/bk_pointing_device.h). | This is the detailed profile data to use for pointer behavior. `autoActivateLayer` maps to each mode's `activate_on_layer`; custom direction fields apply to custom modes. |
| `defaultDPI`, `minimumDefaultDpi`, `defaultDPIConfigStep`, `snipingDPI`, `minimumSnipingDPI`, `snipingDPIConfigStep`, `defaultDPIMaxSteps`, `snipingDPIMaxSteps` | Legacy/flat pointer fields in the export. All are zero here. Current firmware stores the actual values in `pointingModes`; defaults are initialized in [`bk_pointing_modes.c`](../../../../../../modules/bastardkb/bk_pointing_device/bk_pointing_modes.c). | Do not use these zero fields to overwrite the mode profiles. For example, profile 0 in this export reports DPI 700, minimum 100, step 100, and max 3100. |
| `autoMouseLayerEnabled`, `autoPrecisionOnMouseLayerEnabled` | Pointing module configuration bits in `bkpd_config_t`; settings are handled by [`bk_pointing_device.c`](../../../../../../modules/bastardkb/bk_pointing_device/bk_pointing_device.c). | Runtime device preferences, not layer key definitions. |
| `invertXAxisDragscroll`, `invertYAxisDragscroll` | Legacy top-level axis flags. Current pointing profiles store inversion per mode as `invertX`/`invertY`. | Prefer the per-mode fields in `pointingModes`; avoid mapping these legacy fields to global QMK options without checking their intended mode. |
| `flowTapTerm`, `autoMouseTime` | QMK timing/auto-mouse metadata included in the export. | Map to the relevant QMK Flow Tap/pointing-device configuration only when that feature is enabled in the target build; they are not defined by the `lrncfly` `keymap.c`. |
| `isVIAOnly`, `isLeftHanded` | Export/app/keyboard capability flags. The keymap's [`keymap.json`](./keymap.json) enables the Argos and pointing-device community modules. | Use for conditional export/UI behavior; they are not entries in the keymap matrix. |

## What is currently in source versus runtime state

- The key assignments and seven named layers are in [`keymap.c`](./keymap.c).
- [`config.h`](./config.h) sets the eight-layer dynamic map and `LAYER_MENU = 7`;
  [`rules.mk`](./rules.mk) enables Argos dependencies including combos and tap
  dance support.
- The export currently has no actual combo or tap-dance actions. The combo and
  tap-dance capacities only say that the firmware can store up to 16 and 50.
- The export's `pointingModes` and `rgbMatrix` are actual configured runtime
  data. In particular, use `pointingModes` rather than the zeroed legacy DPI
  fields.
- Argos copies QMK combo defaults into EEPROM only on first initialization.
  After that, the EEPROM-backed Argos state can diverge from checked-in C
  defaults. Treat the JSON as the desired runtime state only if this export is
  the version you intend to reproduce.

## Suggested source-generation split

For a maintainable QMK implementation, generate the key matrix as QMK keycode
names/macros in `keymap.c`; generate any non-empty combo definitions as static
combo data; and generate tap-dance actions and references only for active
tap-dance entries. Put fixed timing and feature choices in `config.h`/`rules.mk`.
Keep Argos protocol versions, UI state, hardware identity, and unused capacity
fields out of the generated keymap. If retaining Argos runtime editing, preserve
its EEPROM-backed combo, tap-dance, pointing, and RGB paths rather than
duplicating those values in static keymap data.
