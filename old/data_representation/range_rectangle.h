//
// Created by Vova on 29.09.2020.
//

#pragma once

#include <ostream>
#include <utils/Logger.h>
#include "painter_pch.h"
#include "color.h"

template<class T>
struct RangeRectangle {
	T min_x, max_x;
	T min_y, max_y;

	[[nodiscard]] T dx() const { return max_x - min_x; }
	[[nodiscard]] T dy() const { return max_y - min_y; }

	[[nodiscard]] bool point_satisfies_requirements(const point& point) const {
		return (point.x >= min_x and point.x <= max_x) and (point.y >= min_y and point.y <= max_y);
	}

	[[nodiscard]] bool check_being_fully_inside_of(const RangeRectangle& parent) const {
		return
				parent.point_satisfies_requirements(point{ min_x, min_y })
				and parent.point_satisfies_requirements(point{ max_x, max_y });
	}

	[[nodiscard]] bool check_other_being_fully_inside(const RangeRectangle& parent) const {
		return parent.check_being_fully_inside_of(*this);
	}

	void constrain_point(point& point) const {
		point.x = std::clamp(point.x, double(min_x), double(max_x));
		point.y = std::clamp(point.y, double(min_y), double(max_y));
	}

	void constrain_rect_to_fit_into_me(RangeRectangle& other) {
		other.min_x = std::clamp(other.min_x, min_x, max_x);
		other.max_x = std::clamp(other.max_x, min_x, max_x);

		other.min_y = std::clamp(other.min_y, min_y, max_y);
		other.max_y = std::clamp(other.max_y, min_y, max_y);

		if (other.min_x == other.max_x or other.min_y == other.max_y) {
			LogConsoleError("RangeRectangle", "constrain_rect_to_fit_into_me")
				<< "Through one of dimensions the range intersection size is 0 ==> the result is ≈invalid!";
		}
	}

	[[nodiscard]] point get_center () const {
		return {
				(min_x + max_x) / 2.,
				(min_y + max_y) / 2.,
		};
	}

	friend std::ostream &operator<< (std::ostream &os, const RangeRectangle &rectangle)
	{
		os << "RangeRectangle { " << "min_x: " << rectangle.min_x << " max_x: " << rectangle.max_x << " min_y: " << rectangle.min_y
		   << " max_y: " << rectangle.max_y << " }";
		return os;
	}
};

template<class ReturnType = lint, std::enable_if_t<std::is_integral_v<ReturnType> || std::is_floating_point_v<ReturnType>, void*> = nullptr>
RangeRectangle<ReturnType> get_image_range_limits(const Image& image) {
	return {
			ReturnType(0), ReturnType(image.cols), ReturnType(0), ReturnType(image.rows)
	};
}



template<class T, class F>
RangeRectangle<T> convert_rect(const RangeRectangle<F>& source) {
	for (const auto& value : { source.min_x, source.min_y, source.max_x, source.max_y }) {
		auto min_allowed_T = std::numeric_limits<T>::min();
		auto max_allowed_T = std::numeric_limits<T>::max();

		if (double(min_allowed_T) > double(value)) {
			LogConsoleError("convert_rect")
				<< "Value " << value << " from initial rect is smaller than MIN value of output type ("
				<< type_name<T>()
				        << ", "
				        << min_allowed_T
				        << ")\n";
			throw std::runtime_error("Number doesn't fit into new type");
		}
		if (double(max_allowed_T) < double(value)) {
			LogConsoleError("convert_rect")
				<< "Value " << value << " from initial rect is bigger than MAX value of output type ("
				<< type_name<T>() << ", " << max_allowed_T << ")\n";
			throw std::runtime_error("Number doesn't fit into new type");
		}
	}

	RangeRectangle<T> res = {
			T (source.min_x),
			T (source.max_x),

			T (source.min_y),
			T (source.max_x),
	};

	return res;
}
