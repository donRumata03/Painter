//
// Created by Vova on 16.07.2020.
//

#pragma once

#include "io_api/image_io_utils.h"

/// Line
Image &draw_line_unchecked (Image &image, const std::pair<lint, lint> &start, const std::pair<lint, lint> &end,
                            const color &line_color = { 1., 1., 1. }, size_t thickness = 1);

Image &draw_line (Image &image, const std::pair<lint, lint> &start, const std::pair<lint, lint> &end,
                  const color &line_color = { 1., 1., 1. }, size_t thickness = 1);


/// Circle
Image& draw_circle(Image& image, circle& circle, color circle_color);
