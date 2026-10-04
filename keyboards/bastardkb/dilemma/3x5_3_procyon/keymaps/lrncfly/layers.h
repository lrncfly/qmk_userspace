#ifndef LAYERS_H
#define LAYERS_H

#include <stddef.h>
#include <stdint.h>

enum dilemma_keymap_layers {
    LAYER_BASE = 0,
    LAYER_FUNCTION,
    LAYER_NAVIGATION,
    LAYER_MEDIA,
    LAYER_POINTER,
    LAYER_NUMERAL,
    LAYER_SYMBOLS,
};

static inline const char *dilemma_layer_name(uint8_t layer) {
    static const struct {
        enum dilemma_keymap_layers layer;
        const char *name;
    } dilemma_layer_names[] = {
        {LAYER_BASE, "BASE"},
        {LAYER_FUNCTION, "FUNCTION"},
        {LAYER_NAVIGATION, "NAVIGATE"},
        {LAYER_MEDIA, "MEDIA"},
        {LAYER_POINTER, "POINTER"},
        {LAYER_NUMERAL, "NUMBERS"},
        {LAYER_SYMBOLS, "SYMBOLS"},
    };

    for (size_t i = 0; i < sizeof(dilemma_layer_names) / sizeof(dilemma_layer_names[0]); i++) {
        if (dilemma_layer_names[i].layer == layer) {
            return dilemma_layer_names[i].name;
        }
    }
    return NULL;
}

#endif
