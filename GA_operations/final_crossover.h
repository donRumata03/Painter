//
// Created by Vova on 08.10.2020.
//

#pragma once

#include "painter_pch.h"

class final_crossover
{

public:
	final_crossover() = default;

	std::vector<double> operator ()(const std::pair<std::vector<double>*, std::vector<double>*>& parents, const normalizer& normalizer, const GA::crossover_mode mode) {
		// todo: exchange some close stroke's points

		return (random() >= 0.5) ? (*parents.first) : (*parents.second);
	}
};


