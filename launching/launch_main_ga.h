//
// Created by Vova on 02.10.2020.
//

#pragma once

#include "painter_pch.h"

#include <common_operations/image_segregation.h>

#include "optimization/error/error_computing.h"
#include "optimization/stroke_color_optimizing.h"

#include "GA_operations/all.h"

#include "launching/single_zone_workers/GA_worker.h"
#include "multizoner.h"


using multizone_GA_worker = MultizoneLaunchWrapper<GA_worker>;
// using ... = MultizoneLaunchWrapper<>;


void launch_single_zone_GA (const std::string &filename);
void launch_multizone_GA (const std::string &filename);
