//
// Created by vova on 27.10.2020.
//

#include "image_splitting.h"


std::ostream &operator<<(std::ostream &os, const ImageCell &cell) {
	os << "ImageCell { "
		<< "bottom: " << cell.bottom << " upper: " << cell.upper
		<< " left: " << cell.left << " right: " << cell.right << " }";
	return os;
}

ImageCell::ImageCell(size_t bottom, size_t upper, size_t left, size_t right)
			: bottom(bottom), upper(upper), left(left), right(right) {}

////////////////////////////////////////////////

ImageZoneRepresentation::ImageZoneRepresentation(std::vector<std::vector<ImageCell>> cells) : cells_2d(std::move(cells)) {}

std::vector<std::vector<ImageCell>> ImageZoneRepresentation::get_2d_cells() const { return cells_2d; }

std::vector<ImageCell> ImageZoneRepresentation::get_1d_cells() const {
	if (!cells_1d) {
		cells_1d.emplace();
		cells_1d->reserve(cells_2d.size() * cells_2d.at(0).size());

		for (auto& cell_set : cells_2d) {
			for (auto &cell : cell_set) {
				cells_1d->push_back(cell);
			}
		}

	}

	return *cells_1d;
}


////////////////////////////////////////////////////////////////////////

static ImageZoneRepresentation construct_cells(const std::vector<std::pair<size_t, size_t>>& xs,
                                        const std::vector<std::pair<size_t, size_t>>& ys
                                        )
{
	std::vector<std::vector<ImageCell>> res;
	res.reserve(xs.size());

	for (const auto & x : xs) {
		auto& this_col = res.emplace_back();
		this_col.reserve(ys.size());
		for (const auto & y : ys) {
			this_col.emplace_back(
					y.first,     // bottom;
					y.second,    // upper;

					x.first,     // left;
					x.second     // right;
					); // ImageCell { ... };
		}
	}

	return ImageZoneRepresentation(std::move(res));
}

static void extend_middle_borders(std::vector<std::pair<size_t, size_t>>& values, double relative_cell_overlay_size) {
	std::vector<double> cell_sizes;
	cell_sizes.reserve(values.size());
	for(auto& range : values) cell_sizes.push_back(double(range.second - range.first));
	double average_value = count_average(cell_sizes);
	auto shifting_value = size_t(std::round(average_value * relative_cell_overlay_size));

	for (int border_index = 0; border_index < values.size(); ++border_index) {
		if (border_index != 0) {
			values[border_index].first -= shifting_value;
		}
		if (border_index != values.size() - 1) {
			values[border_index].first += shifting_value;
		}
	}
}

ImageZoneRepresentation schedule_image_splitting(size_t image_w, size_t image_h,
                                                 size_t x_cells, size_t y_cells,
                                                 double relative_cell_overlay_size)
{
	std::vector<std::pair<size_t, size_t>>
		xs_distribution = distribute_task_ranges(image_w, x_cells),
		ys_distribution = distribute_task_ranges(image_h, y_cells);

//	std::cout << "Xs: " << xs_distribution << std::endl;
//	std::cout << "Ys: " << ys_distribution << std::endl;

	// Extend xs:
	extend_middle_borders(xs_distribution, relative_cell_overlay_size);
	extend_middle_borders(ys_distribution, relative_cell_overlay_size);

	return construct_cells(xs_distribution, ys_distribution);
	// return ImageZoneRepresentation{std::vector<std::vector<ImageCell>>()};
}
