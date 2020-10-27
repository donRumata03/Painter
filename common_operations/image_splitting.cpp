//
// Created by vova on 27.10.2020.
//

#include "image_splitting.h"

////////////////////////////////////////////////

ImageZoneRepresentation::ImageZoneRepresentation(std::vector<std::vector<ImageCell>> cells) : cells_2d(std::move(cells)) {}

auto ImageZoneRepresentation::get_2d_cells() const { return cells_2d; }

auto ImageZoneRepresentation::get_1d_cells() const {
	if (!cells_1d) {
		cells_1d.emplace();
		cells_1d->reserve(cells_2d.size() * cells_2d.at(0).size());

		for (auto& cell_set : cells_2d) {
			for (auto &cell : cell_set) {
				cells_1d->push_back(cell);
			}
		}

	}

	return cells_1d;
}

////////////////////////////////////////////////////////////////////////


ImageZoneRepresentation schedule_image_splitting(size_t image_w, size_t image_h) {

}
