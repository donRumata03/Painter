#include "adaptive_params.h"
#include "utils/image.h"
#include "utils/logger.h"

size_t calc_strokes_count(const cv::Mat& img, const cv::Size& size, size_t max_strokes, Color canvas_color) {
  size_t pixels = painted_pixel_number(img, canvas_color);
  return std::max(max_strokes * (pixels / (double)(size.width * size.height)), 1.);
}

ZoneResourceDistributor::ZoneResourceDistributor(const SvgService& service, Color canvas_color,
    const std::optional<std::function<double(const Image&, Color)>>& complexity_estimator) {
  std::optional<std::function<double(const Image&, Color)>> optional_estimator = complexity_estimator;
  if (!optional_estimator) {
    optional_estimator = [](const Image& image, Color canvas_color) {
      return double(painted_pixel_number(image, canvas_color));
    };
  }
  auto real_estimator = *optional_estimator;

  std::vector<double> partial_sums(service.get_regions_count());
  Image image_container;

  for (int i = 0; i < service.get_regions_count(); i++) {
    service.load_image(image_container, i);
    partial_sums[i] = real_estimator(image_container, canvas_color);
  }

  double total_sum = std::accumulate(partial_sums.begin(), partial_sums.end(), 0.);
  zone_fractions.resize(service.get_regions_count());
  std::transform(partial_sums.begin(), partial_sums.end(), zone_fractions.begin(),
      [&total_sum](double partial_sum) { return partial_sum / total_sum; }
  );
}

std::vector<size_t> ZoneResourceDistributor::distribute_resource(
    size_t total_resource, size_t minimal_allowed_param) {
  std::vector<size_t> res(zone_fractions.size());
  std::transform(zone_fractions.begin(), zone_fractions.end(), res.begin(),
      [&total_resource, &minimal_allowed_param](double this_zone_fraction) {
        return std::max(minimal_allowed_param, size_t(std::round(this_zone_fraction * double(total_resource))));
      });
  return res;
}

void ZoneResourceDistributor::add_resource_distribution_to_plot() {
  auto density = count_number_density(zone_fractions, 0.005);

  LogInfo("Adaprive Params") << "Fractions: " << zone_fractions;
  LogInfo("Adaptive Params") << "Density: " << density;

  add_pairs_to_plot(density);
}

void ZoneResourceDistributor::add_distributed_resource_to_plot(
    size_t total_resource, size_t minimal_allowed_param) {
  auto raw_resource = distribute_resource(total_resource, minimal_allowed_param);

  size_t minimal_counter = 0;
  for (const auto& item : raw_resource) {
    if (item == minimal_allowed_param) {
      minimal_counter++;
    }
  }
  LogInfo("Adaptive Params")<< percent_plotter(minimal_counter, zone_fractions.size(), 1)
                            << " of all the zones have the lowest amount of resource!";

  std::vector<double> real_valued_distribution;
  real_valued_distribution.reserve(raw_resource.size());
  for (const auto& item : raw_resource) {
    real_valued_distribution.push_back(double(item));
  }

  auto density = count_number_density(real_valued_distribution, 0.001);
  add_pairs_to_plot(density);
}

void ZoneResourceDistributor::show_plot_with_parameters() {
  show_plot({.log_x = true, .log_y = false, .window_title = "Zone-Resource Distribution"});
}

double ZoneResourceDistributor::compute_Gini_index() {
  return count_Gini_index(zone_fractions);
}

void ZoneResourceDistributor::print_inequality_account() {
  LogInfo("Adaptive Params") << "Gini index: " << compute_Gini_index() * 100. << "%";
  LogInfo("Adaptive Params") << "Top 1% biggest zones cover " << top_1_percent_wealth_share() * 100. 
                             << " percent of image area";
  LogInfo("Adaptive Params") << "Top " << having_half_wealth_share_fraction() * 100.
                             << "% biggest zones cover half of the picture";
}

double ZoneResourceDistributor::top_1_percent_wealth_share() {
  auto sorted_fractions = get_sorted_fractions(false);
  double top_1_percent_wealth = std::accumulate(sorted_fractions.begin(),
      sorted_fractions.begin() + size_t(sorted_fractions.size() * 0.01), 0.);

  return top_1_percent_wealth / sum(zone_fractions);
}

double ZoneResourceDistributor::having_half_wealth_share_fraction() {
  auto sorted_fractions = get_sorted_fractions(false);

  double sum = std::accumulate(zone_fractions.begin(), zone_fractions.end(), 0.);
  double current_wealth_share = 0.;
  size_t index = 0;

  while (current_wealth_share < sum * 0.5 && index < sorted_fractions.size()) {
    current_wealth_share += sorted_fractions[index];
    index++;
  }

  return double(index) / zone_fractions.size();
}

void ZoneResourceDistributor::add_cumulative_inequality_graph_to_plot() {
  add_pairs_to_plot(get_cumulative_inequality_graph());
}

std::vector<double> ZoneResourceDistributor::get_sorted_fractions(bool from_lowest) {
  auto res = zone_fractions;

  if (from_lowest) {
    std::sort(res.begin(), res.end(), std::less<double>());
  } else {
    std::sort(res.begin(), res.end(), std::greater<double>());
  }

  return res;
}

std::vector<std::pair<double, double>>
ZoneResourceDistributor::get_cumulative_inequality_graph() {
  auto increasing_fractions = get_sorted_fractions(true);

  std::vector<std::pair<double, double>> res(increasing_fractions.size() + 1);
  res[0] = {0., 0.};

  size_t n = zone_fractions.size();
  double total_sum = sum(zone_fractions);
  double cumulative_sum = 0.;

  for (size_t i = 0; i < increasing_fractions.size(); ++i) {
    cumulative_sum += increasing_fractions[i];
    res[i + 1] = {double(i + 1) / double(n), cumulative_sum / total_sum};
  }

  return res;
}
/**
 * \note: if the zones are pretty ragged, this might be not an ideal estimator
 * and distributor
 */
double estimate_stroke_complexity(ZoneComplexityDescriptor zone_descriptor) {
  // TODO: test me !

  /// HyperParameters
  constexpr double max_perimeter_contribution = 1.3;  // Is essentially an asymptote of relative perimeter contribution

  constexpr double characteristic_y = 0.9;
  constexpr double x_of_characteristic_y = 5.;
  double k = -x_of_characteristic_y / std::log(1 - characteristic_y);
  LogInfo("Adaptive Params") << "[Estimating Stroke Complexity]: k is " << k;

  /// Considering circle the easiest figure of the given area

  double circles_length = 2 * std::sqrt(pi * zone_descriptor.covered_area);  // Length of a circle of the same area
  if (zone_descriptor.stroke_perimeter > circles_length + 1e-7) {
    throw std::range_error("Perimeter can't be so small!");
  }

  double relative_excess = (zone_descriptor.stroke_perimeter - circles_length) / circles_length;
  assert(relative_excess >= -1e-10);
  // If it's 10 times bigger than the circle's one, it isn't a big difference
  // between it and 9-time-bigger-zone But it isn't true for 2 and 3 difference

  double perimeter_contribution = max_perimeter_contribution * (1 - std::exp(-relative_excess / k));
  double result = zone_descriptor.covered_area * (1 + perimeter_contribution);

  return result;
}

std::vector<size_t> distribute_strokes_between_bezier_zones(
    const std::vector<ZoneComplexityDescriptor>& zone_descriptors,
    size_t total_strokes) {
  std::vector<double> complexities(zone_descriptors.size());
  std::transform(zone_descriptors.begin(), zone_descriptors.end(),
                 complexities.begin(),
                 [](auto zone) { return estimate_stroke_complexity(zone); });

  double overall_complexity = std::accumulate(complexities.begin(), complexities.end(), 0.);

  std::vector<size_t> res;
  res.reserve(zone_descriptors.size());
  std::transform(complexities.begin(), complexities.end(), res.begin(),
      [&total_strokes, &overall_complexity](double local_complexity) -> size_t {
        return size_t(std::round(double(total_strokes) * local_complexity /
                                 overall_complexity));
      });

  return res;
}
