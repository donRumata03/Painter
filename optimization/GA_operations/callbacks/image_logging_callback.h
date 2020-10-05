//
// Created by Vova on 05.10.2020.
//

#pragma once

#include <utility>

#include "painter_pch.h"

/**
 * Renders and logs all the intermediate states as images to folders to a given one.
 * Does this and copies all the generated folder to a new one, specially for generated  in destructor
 */
class image_logging_callback
{
	// TODO!

	std::string path_for_logging;
	std::string path_for_copying;

public:
	image_logging_callback(std::string  path_for_logging, std::string  path_for_copying)
		: path_for_copying(std::move(path_for_copying)), path_for_logging(std::move(path_for_logging)) {}

	~image_logging_callback(); // todo
};


