//
// Created by vova on 28.12.2021.
//

#pragma once

#include "painter_pch.h"
#include "utils/image.h"
#include "pixel_layer_tracker.h"

/**
 * Stores only pixel number and color with highest priority
 */
class OneColorPixelLayerTracker {
private:
    li highest_priority = 0;
    std::optional<Color> upper_color = std::nullopt;
    size_t layers_count = 0;

public:
    // Constructs defaultly //
    OneColorPixelLayerTracker() {}
    void add_color_layer(li priority, Color c);
    size_t get_layers_count() const;
    std::optional<Color> get_pixel_color();
};
