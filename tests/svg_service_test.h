//
// Created by Vova on 03.03.2021.
//

#pragma once

#include <vector_graphics/SVG_service.h>



inline void test_splitting_SVG(const std::string& filename) {
	fs::path path = filename;

	SVG_service svg_manager(filename, true);
	svg_manager.split_paths();


}
