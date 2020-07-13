//
// Created by Vova on 13.07.2020.
//

#ifndef PAINTER_STROKE_H
#define PAINTER_STROKE_H

#include <pythonic.h>

#include "color.h"

/*
 * It`s basically a 2nd order Bezier curve => contains
*/
struct stroke
{
	// using point = std::pair<double, double>;
	using point = point;

	point p1, p2, p3;
	double width = -1;
	color background_color;

	stroke(point point1, point point2, point point3, double _width, color _color);
	point coords_at(double t); // Count the point of Bezier curve corresponding to t value given as the argument
	double height_at(double t); // The height of the curve
};


#endif //PAINTER_STROKE_H
