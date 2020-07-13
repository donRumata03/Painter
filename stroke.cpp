//
// Created by Vova on 13.07.2020.
//

#include "stroke.h"

stroke::stroke (stroke::point point1, stroke::point point2, stroke::point point3, double _width, color _color)
			: p1(std::move(point1)),
			  p2(std::move(point2)),
			  p3(std::move(point3)),
			  width(_width),
			  background_color(_color)
{}


stroke::point stroke::coords_at (double t)
{
	assert(t >= 0 && t <= 1);



	return {  };
}

double stroke::height_at (double t)
{
	assert(t >= 0 && t <= 1);

	// TODO

	return 0;
}
