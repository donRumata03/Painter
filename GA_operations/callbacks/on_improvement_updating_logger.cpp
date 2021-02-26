//
// Created by Vova on 26.02.2021.
//

#include "on_improvement_updating_logger.h"


void OnImprovementUpdatingLogger::operator() (const std::vector<double>&, size_t this_iteration) const
{
	if (not (pythonic_random() < logging_fraction)) {
		return;
	}

	// TODO
}
