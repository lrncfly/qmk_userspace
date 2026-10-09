#ifndef LAYERS_H
#define LAYERS_H

#include <stddef.h>
#include <stdint.h>

#include "layer_config.h"
#include "quantum.h"

typedef struct {
    uint8_t layer;
    const char *name;
    HSV color;
} dilemma_layer_metadata_t;

static inline const dilemma_layer_metadata_t *dilemma_layer_metadata(uint8_t layer) {
    static const dilemma_layer_metadata_t metadata[] = {
        {LAYER_BASE, "BASE", {HSV_BLACK}},
        {LAYER_FUNCTION, "FUNCTION", {HSV_AZURE}},
        {LAYER_NAVIGATION, "NAVIGATE", {HSV_CHARTREUSE}},
        {LAYER_MEDIA, "MEDIA", {HSV_CORAL}},
        {LAYER_POINTER, "POINTER", {HSV_CYAN}},
        {LAYER_NUMERAL, "NUMBERS", {HSV_PINK}},
        {LAYER_SYMBOLS, "SYMBOLS", {HSV_GOLD}},
    };

    for (size_t i = 0; i < sizeof(metadata) / sizeof(metadata[0]); i++) {
        if (metadata[i].layer == layer) {
            return &metadata[i];
        }
    }
    return NULL;
}

static inline const char *dilemma_layer_name(uint8_t layer) {
    const dilemma_layer_metadata_t *metadata = dilemma_layer_metadata(layer);
    return metadata ? metadata->name : NULL;
}

#ifdef RGB_MATRIX_ENABLE
static inline HSV dilemma_layer_indicator_hsv(uint8_t layer) {
    const dilemma_layer_metadata_t *metadata = dilemma_layer_metadata(layer);
    return metadata ? metadata->color : (HSV){HSV_BLACK};
}

RGB dilemma_layer_indicator_rgb(uint8_t layer);
#endif

#endif
