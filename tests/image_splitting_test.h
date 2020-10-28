//
// Created by vova on 28.10.2020.
//


#pragma once

#include "common_operations/image_splitting.h"

inline void test_zone_scheduling() {
	std::cout << schedule_image_splitting(100, 100, 2, 2, 0.1).get_1d_cells() << std::endl;
}
