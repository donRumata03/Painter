//
// Created by vova on 28.12.2021.
//


#pragma once

#include "painter_pch.h"

#include "utils/image.h"


struct PixelPaintingData {
    li priority = 0;
    Color color{};
};




// Concept, but basically, an interface but allows inlining and optimizations

// :NOTE: The higher the priority, the more valuable the layer is
template <typename T>
concept PixelLayerTracker =

     // Constructor
     requires () {T();}

		// void add_color_layer(li priority, Color c);
	&&	requires (T tracker, li priority, Color c) { {tracker.add_color_layer(priority, c)} -> std::same_as<void>; }

		// size_t get_layers_count();
	&&  requires (const T tracker) { {tracker.get_layers_count()} -> std::same_as<size_t>; }

		//   std::optional<Color> get_pixel_color();
	&&  requires (const T tracker) { {tracker.get_pixel_color()} -> std::same_as<std::optional<Color>>; }
;


template <typename Tracker>
concept RemovablePixelLayerTracker =
  PixelLayerTracker<Tracker> &&

	// Color remove_layer_by_priority(li priority);
  requires (Tracker tracker) { {tracker.remove_layer_by_priority(li{0})} -> std::same_as<Color>; }
;


template<typename Tracker>
concept SortableAfterPixelLayerTracker =
  PixelLayerTracker<Tracker> &&

  // void sort_by_priority(&mut self);
  requires (Tracker tracker) { {tracker.sort_by_priority()}; }
;

