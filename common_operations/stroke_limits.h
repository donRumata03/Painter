//
// Created by Vova on 25.09.2020.
//

#pragma once

#include <ostream>
#include "data_representation/stroke.h"
#include "basic_constraining.h"

struct stroke_limit_descriptor
{
	double min_dx, max_dx; /// delta x
	double min_dy, max_dy; /// delta y

	double min_width, max_width; /// width

	RangeRectangle<double> image_rectangle;

	[[nodiscard]] double image_dx() const { return image_rectangle.max_x - image_rectangle.min_x; }
	[[nodiscard]] double image_dy() const { return image_rectangle.max_y - image_rectangle.min_y; }

	[[nodiscard]] bool validate_self() const {
		if (min_dx >= max_dx) return false;
		if (min_dy >= max_dy) return false;
		if (image_dx() < max_dx) return false;
		if (image_dy() < max_dy) return false;

		return true;
	}

	[[nodiscard]] bool stroke_satisfies_requirements(const stroke& stroke) const {
		/// Check width:
		if (stroke.width < min_width or stroke.width > max_width) return false;

/*
		/// Check x and y:
		double min_x_found = std::numeric_limits<double>::max(), min_y_found = std::numeric_limits<double>::max(),
			   max_x_found = std::numeric_limits<double>::min(), max_y_found = std::numeric_limits<double>::min();

		auto process_point = [&](const stroke::Point& p) {
			min_x_found = std::min(min_x_found, p.x);
			min_y_found = std::min(min_y_found, p.y);

			max_x_found = std::max(max_x_found, p.x);
			max_y_found = std::max(max_y_found, p.y);
		};

		process_point(stroke.p0);
		process_point(stroke.p1);
		process_point(stroke.p2);
*/
		RangeRectangle<double> stroke_bounding_box = stroke.get_bounding_box();

		auto dx = stroke_bounding_box.max_x - stroke_bounding_box.min_x;
		auto dy = stroke_bounding_box.max_y - stroke_bounding_box.min_y;

		return dx <= max_dx and dx >= min_dx and dy <= max_dy and dy >= min_dy;
	}

	bool constrain_stroke_to_requirements(stroke& stroke) const {
		// Returns initial state of stroke
		if (stroke_satisfies_requirements(stroke)) return true;

		// Constrain width independently:
		stroke.width = std::clamp(stroke.width, min_width, max_width);
/*
		stroke.width = std::max(stroke.width, min_width);
		stroke.width = std::min(stroke.width, max_width);
*/
/*

		auto constrain_point = [&](stroke::Point& Point){
			Point.x = std::clamp(Point.x, min_);
		};
*/

		// Scale it:
		constrain_stroke_size_parameters(stroke,
		                                 min_dx,
		                                 max_dx,

		                                 min_dy,
		                                 max_dy,

		                                 min_width,
		                                 max_width);

		// Move it:
		carefully_constrain_stroke_to_fit_into_rect(stroke, image_rectangle);


		return false;
	}

	friend std::ostream &operator<< (std::ostream &os, const stroke_limit_descriptor &descriptor)
	{
		os << "Stroke limit descriptor { " << "min_dx: " << descriptor.min_dx << " max_dx: " << descriptor.max_dx << " min_dy: " << descriptor.min_dy
		   << " max_dy: " << descriptor.max_dy << " min_width: " << descriptor.min_width << " max_width: "
		   << descriptor.max_width << " image_rectangle: " << descriptor.image_rectangle << " }";
		return os;
	}
};
