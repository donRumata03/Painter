//
// Created by vova on 27.10.2020.
//

/*
 * 1) Make xs and ys arrays
 * 2) Make 2D array of cells
 * 3) Have ability 1D array of cells
 */


#pragma once

#include <painter_pch.h>

#include <utility>
#include <ostream>

struct ImageCell {
	friend std::ostream &operator<<(std::ostream &os, const ImageCell &cell);

	ImageCell(size_t bottom, size_t upper, size_t left, size_t right);

	size_t bottom;
	size_t upper;

	size_t left;
	size_t right;
};

class ImageZoneRepresentation {
public:
    ImageZoneRepresentation() = delete;

    explicit ImageZoneRepresentation(std::vector<std::vector<ImageCell>> cells);

    // Getters:
    [[nodiscard]] std::vector<std::vector<ImageCell>> get_2d_cells() const;
	std::vector<ImageCell> get_1d_cells() const;

private:
	std::vector<std::vector<ImageCell>> cells_2d;
	mutable std::optional<std::vector<ImageCell>> cells_1d;
};

ImageZoneRepresentation schedule_image_splitting(size_t image_w, size_t image_h,
												 size_t x_cells, size_t y_cells,
												 double relative_cell_overlay_size);

