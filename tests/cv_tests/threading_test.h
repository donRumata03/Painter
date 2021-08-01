//
// Created by vova on 01.08.2021.
//


#pragma once

#include <io_api/image_io_utils.h>

/**
 * Does num threads return to system defaults after one parallel region?!
 */
inline void test_cv_threading() {
	auto print_threads = [](){
		std::cout << "Num Threads: " << cv::getNumThreads() << std::endl;
	};

	Image i = make_default_image(1920, 1080);

	auto par_cv_operation = [&](){
		fill_image(i, { 1, 0, 1 });
	};

	print_threads();
	cv::setNumThreads(0);
	print_threads();
	par_cv_operation();
	par_cv_operation();
	par_cv_operation();
	print_threads();
	par_cv_operation();
	print_threads();


}

