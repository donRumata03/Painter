#include "image.h"
#include "utils/logger.h"


Image make_default_image(const cv::Size& size, const Color& color) {
  return cv::Mat(size, CV_64FC3, {color.r, color.g, color.b});
}

Image make_default_image(size_t w, size_t h, const Color& color) {
  auto size = cv::Size{int(w), int(h)};
  return make_default_image(size, color);
}

Image make_default_image(size_t w, size_t h, double grey_brightness) {
  return make_default_image(w, h, {grey_brightness, grey_brightness, grey_brightness});
}

Image make_default_image(cv::Size& size, double grey_brightness) {
  return make_default_image(size, {grey_brightness, grey_brightness, grey_brightness});
}


Image open_image(const std::string& filename) {
  auto img = cv::imread(filename);

  rgb2bgr<BytePixel>(img); // Convert BGR image produced by OpenCV to RGB
  auto double_img = convert_image_to_floating_point(img); // Convert to image of doubles

  return double_img;
}

void cv_show_image(const Image& img, const std::string& image_name) {
  auto char_image = convert_image_from_floating_point(img);
  rgb2bgr<BytePixel>(char_image);

  cv::namedWindow(image_name, cv::WINDOW_NORMAL);
  cv::imshow(image_name, char_image);

  cv::waitKey(0);
}


void show_image_in_system_viewer(const Image& img, const std::string& temp_name) {
  std::string temp_folder = (std::filesystem::current_path() / "resources"s / "temp"s).string();
  if (not fs::exists(temp_folder)) {
    fs::create_directory(temp_folder);
  }

  auto space_checker = [](const std::string& which_path, const std::string& path_name) {
    if (std::any_of(which_path.begin(), which_path.end(), [](char value) { return value == ' '; })) {
      LogConsoleError("Image", "show_image_in_system_viewer") << "Bad " << path_name << ": there are spaces!";
      assert(false);
    }
  };

  space_checker(temp_folder, "temp path");
  if (!temp_name.empty()) space_checker(temp_name, "user temp file name");

  std::string filename;
  if (temp_name.empty()) {
    // Generate random (chronological) path
    auto prev_temp_files = lsdir<given_filename_encoding::utf8>(temp_folder);

    size_t last_file_identifier = 0;
    for (auto& temp_file : prev_temp_files) {

      auto path_head = split(temp_file, {'/', '\\'}).back();

      size_t first_dot_pos = path_head.find('.');
      std::string identifier = Slice(path_head, noslice, first_dot_pos);

      size_t numeric_identifier = 0;

      try {
        numeric_identifier = std::stoll(identifier);
      } catch (std::exception& e) {
        LogError("Image", "show_image_in_system_viewer") << e.what();
      }

      last_file_identifier = std::max(last_file_identifier, size_t(numeric_identifier));
    }


    std::string this_file_identifier = std::to_string(last_file_identifier + 1);
    filename = (fs::path(temp_folder) / (this_file_identifier + ".png")).string();
  } else {
    filename = (fs::path(temp_folder) / temp_name).string();
    if (not is_in('.', temp_name)) {
      filename += ".png";
    }
    LogConsoleInfo("Image", "show_image_in_system_viewer") << "Temporary image's filename is " << filename;
  }

  save_image(img, filename);

#ifdef PYTHONIC_IS_LINUX
  system(("xdg-open " + filename).c_str());
#else
  system(filename.c_str());
#endif
}

void save_image(const Image& img, const fs::path& filename) {
  fs::create_directories(filename.parent_path());

  Image output;
  switch (img.type()) {
    case CV_8UC3: // Nothing to do
      output = img;
      break;
    case CV_64FC3:
      output = convert_image_from_floating_point(img);
      bgr2rgb<BytePixel>(output);
      break;
    default:
      assert(false);
  }

  cv::imwrite(filename.string(), output);
}


Image rgb2hsv(const Image& img) {
  Image res;
  cv::cvtColor(img, res, cv::COLOR_RGB2HSV);

  return res;
}

Image hsv2rgb(const Image& img) {
  Image res;
  cv::cvtColor(img, res, cv::COLOR_HSV2RGB);

  return res;
}


Image convert_image_to_floating_point(const Image& img) {
  Image res;
  img.convertTo(res, CV_64FC3, 1. / 255);
  return res;
}

Image convert_image_from_floating_point(const Image& img) {
  Image res;
  img.convertTo(res, CV_8UC3, 255.);
  return res;
}


void fill_image(Image& image, const Color& fill_color, bool parallelize) {
  if (parallelize) {
    image.forEach<Pixel>([&fill_color](Pixel& pixel, const int position[]) {
      pixel.x = fill_color.r;
      pixel.y = fill_color.g;
      pixel.z = fill_color.b;
    });
  } else {
    auto cv_color = fill_color.to_OpenCV_Vec3();

    for (size_t y = 0; y < image.rows; ++y) {
      for (size_t x = 0; x < image.cols; ++x) {
        image.at<cv::Vec3d>(y, x) = cv_color;
      }
    }
  }
}

