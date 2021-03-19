//
// Created by Vova on 13.03.2021.
//

#pragma once

#include "common_operations/image_adaptive_params.h"

inline auto get_default_path_distributor (const fs::path& path)
{
	auto svg = SVG_service(path);
	svg.split_paths();

	ZoneResourceDistributor actor(svg, {});

	return actor;
}


inline void test_zone_distributor(const fs::path& path)
{
	auto actor = get_default_path_distributor(path);

	actor.add_resource_distribution_to_plot();
	actor.show_plot_with_parameters();

	actor.add_distributed_resource_to_plot(10'000, 1);
	actor.show_plot_with_parameters();
}

inline void test_counting_zone_inequality(const fs::path& path) {
	auto actor = get_default_path_distributor(path);

	actor.print_inequality_account();
	actor.add_cumulative_inequality_graph_to_plot();
	show_plot({ .window_title = "Cumulative Inequality Graph" });
}

inline void test_plotting_inequality_graph(const fs::path& path) {
	auto actor = get_default_path_distributor(path);

	actor.add_cumulative_inequality_graph_to_plot();
	show_plot();
}

inline void test_comparing_dummy_and_non_dummy_distributor() {
	// TODO…
}
