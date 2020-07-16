//
// Created by Vova on 16.07.2020.
//

#pragma once

#include "utils/image_io_utils.h"

void draw_line_unchecked(Image& image,
		const std::pair<lint, lint>& start, const std::pair<lint, lint>& end);
void draw_line();
