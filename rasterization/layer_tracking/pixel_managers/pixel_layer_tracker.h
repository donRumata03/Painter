//
// Created by vova on 28.12.2021.
//


#pragma once

// Basically, an interface but allows inlining and optimizations
concept PixelLayerTracker<T> =
		// void add_color_layer(li priority, Color c);
		requires (T tracker, li priority, Color c) { {tracker.add_color_layer(priority, c)} -> std::same_as<Color>; }

		// size_t get_layers_count();
	&&  requires (const T tracker) { {tracker.get_layers_count()} -> std::same_as<size_t>; }

		//   std::optional<Color> get_pixel_color();
	&&  requires (const T tracker) { {tracker.get_pixel_color()} -> std::same_as<std::optional<Color>>; }
;


concept RemovablePixelLayerTracker<T> =
		PixelLayerTracker<T> &&

		// Color remove_layer_by_priority(li priority);
        requires (T tracker) { {tracker.remove_layer_by_priority(li{0})} -> std::same_as<Color>; }
;

