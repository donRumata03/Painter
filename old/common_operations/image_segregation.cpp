//
// Created by Vova on 28.10.2020.
//

#include "image_segregation.h"


static Image crop_image(const Image& image, const ImageCell& cell) {
	cv::Rect cropping_rect = cv::Rect{
			static_cast<int>(cell.min_x),
			static_cast<int>(cell.min_y),

			static_cast<int>(cell.max_x - cell.min_x),
			static_cast<int>(cell.max_y - cell.min_y),
	};

	// std::cout << cell << " -> " << cropping_rect << std::endl;

	Image cropped = image(cropping_rect);
	Image res;
	cropped.copyTo(res);

	return res;
}


ImageZones split_image_into_zones (const Image& image_to_crop, const ImageZoneRepresentation& zones)
{
	assert(image_to_crop.cols == zones.get_image_w());
	assert(image_to_crop.rows == zones.get_image_h());

	std::vector<std::vector<Image>> images;
	images.reserve(zones.get_x_zones());

	for (size_t col_index = 0; col_index < zones.get_x_zones(); col_index++) {
		auto& image_col = images.emplace_back();
		image_col.reserve(zones.get_y_zones());
		for (size_t row_index = 0; row_index < zones.get_y_zones(); row_index++) {
			const auto& this_cell = zones.get_2d_cells()[col_index][row_index];
			image_col.emplace_back(crop_image(image_to_crop, this_cell));
		}
	}

	return {
			zones,
			std::move(images)
	};
}
