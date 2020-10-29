//
// Created by vova on 27.10.2020.
//

#include "image_splitting_scheduling.h"


std::ostream &operator<<(std::ostream &os, const ImageCell &cell) {
	os << "ImageCell { "
	   << "min_y: " << cell.min_y << " max_y: " << cell.max_y
	   << " min_x: " << cell.min_x << " max_x: " << cell.max_x << " }";
	return os;
}

ImageCell::ImageCell(size_t bottom, size_t upper, size_t left, size_t right)
			: min_y(bottom), max_y(upper), min_x(left), max_x(right) {}

////////////////////////////////////////////////

ImageZoneRepresentation::ImageZoneRepresentation(std::vector<std::vector<ImageCell>> cells, size_t _image_w, size_t _image_h)
									: cells_2d(std::move(cells)), image_w(_image_w), image_h(_image_h)
{

}

const std::vector<std::vector<ImageCell>>& ImageZoneRepresentation::get_2d_cells() const { return cells_2d; }

const std::vector<ImageCell>& ImageZoneRepresentation::get_1d_cells() const
{
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

size_t ImageZoneRepresentation::get_image_w () const
{
	return image_w;
}

size_t ImageZoneRepresentation::get_image_h () const
{
	return image_h;
}

size_t ImageZoneRepresentation::get_x_zones () const
{
	return cells_2d.size();
}

size_t ImageZoneRepresentation::get_y_zones () const
{
	return cells_2d[0].size();
}

std::ostream& operator<< (std::ostream& os, const ImageZoneRepresentation& zones)
{
	os << "image_w: " << zones.image_w << " image_h: " << zones.image_h << " cells_2d: "
	   << zones.cells_2d;
	return os;
}


////////////////////////////////////////////////////////////////////////

static ImageZoneRepresentation construct_cells(const std::vector<std::pair<size_t, size_t>>& xs,
                                        const std::vector<std::pair<size_t, size_t>>& ys,
                                        size_t image_w, size_t image_h
                                        )
{
	std::vector<std::vector<ImageCell>> res;
	res.reserve(xs.size());

	for (const auto & x : xs) {
		auto& this_col = res.emplace_back();
		this_col.reserve(ys.size());
		for (const auto & y : ys) {
			this_col.emplace_back(
					y.first,     // min_y;
					y.second,    // max_y;

					x.first,     // min_x;
					x.second     // max_x;
					); // ImageCell { ... };
		}
	}

	return ImageZoneRepresentation(std::move(res), image_w, image_h);
}

static void extend_middle_borders(std::vector<std::pair<size_t, size_t>>& values, double relative_cell_overlay_size) {
	std::vector<double> cell_sizes;
	cell_sizes.reserve(values.size());
	for(auto& range : values) cell_sizes.push_back(double(range.second - range.first));
	double average_value = count_average(cell_sizes);
	auto shifting_value = size_t(std::round(average_value * relative_cell_overlay_size));

	for (size_t border_index = 0; border_index < values.size(); ++border_index) {
		if (border_index != 0) {
			values[border_index].first -= shifting_value;
		}
		if (border_index != values.size() - 1) {
			values[border_index].second += shifting_value;
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

	std::cout << "Xs before extending: " << xs_distribution << std::endl;
	std::cout << "Ys before extending: " << ys_distribution << std::endl;

	// Extend xs:
	extend_middle_borders(xs_distribution, relative_cell_overlay_size);
	extend_middle_borders(ys_distribution, relative_cell_overlay_size);

	std::cout << "Xs after extending: " << xs_distribution << std::endl;
	std::cout << "Ys after extending: " << ys_distribution << std::endl;


	return construct_cells(xs_distribution, ys_distribution, image_w, image_h);
	// return ImageZoneRepresentation{std::vector<std::vector<ImageCell>>()};
}
