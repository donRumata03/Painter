//
// Created by vova on 28.12.2021.
//


#pragma once

#include "painter_pch.h"
#include "utils/image.h"
#include "pixel_layer_tracker.h"

// TODO: test this!

/**
 * Stores colors with priorities in arbitrary order
 * but recovers invariant (being sorted by priority ASCENDING) when is asked to do this
 */
class SortAfterFillingPixelLayerTracker {
  std::vector<PixelPaintingData> brush_layers = {};
  bool is_sorted = true;

public:
    // Constructs defaultly //
    SortAfterFillingPixelLayerTracker() {}
    void add_color_layer(li priority, Color c);
    size_t get_layers_count() const;
    std::optional<Color> get_pixel_color() const;
    void sort_by_priority();
};

static_assert(SortableAfterPixelLayerTracker<SortAfterFillingPixelLayerTracker>);

