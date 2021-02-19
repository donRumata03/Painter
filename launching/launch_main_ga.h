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

#include "vector_graphics/SVG_service.h"

class multizone_GA_launcher {

public:
    explicit multizone_GA_launcher(Image _image, size_t _zones_x, size_t _zones_y,
                                   double overlay_percent, GA_launching_params params);

    bool process_one_cell();

    void run();

    Image get_resultant_image();

    void save_result(const fs::path &filename);


    /// Memory managements stuff:
    multizone_GA_launcher(const multizone_GA_launcher &) = delete;

    multizone_GA_launcher(multizone_GA_launcher &&) = delete;

    multizone_GA_launcher operator=(const multizone_GA_launcher &) = delete;

    multizone_GA_launcher operator=(multizone_GA_launcher &&) = delete;

    ~multizone_GA_launcher() = default;


private:
    std::vector<double> glue_best_genomes();

private:
    ImageZones zones;
    std::vector<std::vector<std::shared_ptr<GA_worker>>> workers;
    std::vector<std::vector<bool>> workers_ready;

    Image image;

    image_logging_callback logger;
    size_t epochs_performed = 0;
};


using multizone_GA_worker = MultizoneLaunchWrapper<GA_worker>;
// using ... = MultizoneLaunchWrapper<>;


void launch_single_zone_GA (const std::string &filename);
void launch_multizone_GA (const std::string &filename);
void launch_svg_stroking(const std::string &filename);