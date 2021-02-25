#include "image_adaptive_params.h"
#include "io_api/image_io_utils.h"


size_t calc_strokes_count(const cv::Mat& img, const cv::Size& size, const size_t max_strokes)
{
    cv::Mat gray, thresh, u_img = convert_image_from_floating_point(img);
    cv::cvtColor(u_img, gray, cv::COLOR_BGR2GRAY);
    cv::threshold(gray, thresh, 0, 1, cv::THRESH_BINARY);

    size_t pixels = cv::sum(thresh)[0];
    return max_strokes * (pixels / (double)(size.width * size.height));
}
