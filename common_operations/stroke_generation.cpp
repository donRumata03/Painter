//
// Created by Vova on 06.10.2020.
//

#include "stroke_generation.h"

stroke generate_stroke (const stroke_limit_descriptor &stroke_descriptor)
{
	double image_dx = stroke_descriptor.image_rectangle.max_x - stroke_descriptor.image_rectangle.min_x;
	double image_dy = stroke_descriptor.image_rectangle.max_y - stroke_descriptor.image_rectangle.min_y;

	// assert(image_dx >= stroke_descriptor.max_dx);
	assert(stroke_descriptor.validate_self());


	std::random_device rd{};
	std::mt19937 gen { rd() };

	stroke res_stroke;

	// Firstly generate the stroke without its position
	// (all the three points are from <(0, stroke_descriptor.min_x); (0, stroke_descriptor.min_y)>):
	auto gen_x = [&](){ return randval(0, stroke_descriptor.max_dx, gen); };
	auto gen_y = [&](){ return randval(0, stroke_descriptor.max_dy, gen); };

	res_stroke.p0.x = gen_x();
	res_stroke.p0.y = gen_y();

	res_stroke.p1.x = gen_x();
	res_stroke.p1.y = gen_y();

	res_stroke.p2.x = gen_x();
	res_stroke.p2.y = gen_y();

	// Find its bounding box:
	auto bounding_box = res_stroke.get_bounding_box();
	double max_shift_x = image_dx - bounding_box.dx();
	double max_shift_y = image_dy - bounding_box.dy();

//	std::cout << res_stroke << std::endl;
//	std::cout << max_shift_x << " " << max_shift_y << std::endl;

	// Generate positioning:
	double shift_x = stroke_descriptor.image_rectangle.min_x + randval(0, max_shift_x, gen);
	double shift_y = stroke_descriptor.image_rectangle.min_y + randval(0, max_shift_y, gen);

	// Shift the stroke:
	shift_stroke(res_stroke, { shift_x, shift_y });

	// Width is easy to generate:
	res_stroke.width = randval(stroke_descriptor.min_width, stroke_descriptor.max_width, gen);

	return res_stroke;
}
