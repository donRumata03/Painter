//
// Created by vova on 28.12.2021.
//


#pragma once

struct PixelPaintingData {
    li priority = 0;
    Color color;
};




// Concept, but basically, an interface but allows inlining and optimizations

// :NOTE: The higher the priority, the more valuable the layer is
concept PixelLayerTracker<T> =

     // Constructor
     requires () {T()}

		// void add_color_layer(li priority, Color c);
	&&	requires (T tracker, li priority, Color c) { {tracker.add_color_layer(priority, c)} -> std::same_as<Color>; }

		// size_t get_layers_count();
	&&  requires (const T tracker) { {tracker.get_layers_count()} -> std::same_as<size_t>; }

		//   std::optional<Color> get_pixel_color();
	&&  requires (const T tracker) { {tracker.get_pixel_color()} -> std::same_as<std::optional<Color>>; }
;


concept RemovablePixelLayerTracker<T> =
		TreeMapPixelLayerTracker<T> &&

		// Color remove_layer_by_priority(li priority);
        requires (T tracker) { {tracker.remove_layer_by_priority(li{0})} -> std::same_as<Color>; }
;

concept SortableAfterPixelLayerTracker<T> =
  TreeMapPixelLayerTracker<T> &&

  // void sort_by_priority(&mut self);
  requires (T tracker) { {tracker.sort_by_priority()}; }
;

