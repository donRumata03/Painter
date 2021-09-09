#include "painter_pch.h"

// STD headers
#include <iostream>

// Libraries
#include <argparse/argparse.hpp>

// Project headers
#include "utils/logger.h"
#include "launch/launch.h"
#include "data/color.h"


int main(int argc, char *argv[]) {
  set_utf8_in_console();

  Logger::SetLogFile(std::filesystem::current_path() / "log" / "logs.txt");
  Logger::SetLoggingLevel(LogLevel::Info);

  argparse::ArgumentParser program("Painter");

  program.add_argument("path")
          .required()
          .help("Path to target image");

  program.add_argument("--params")
          .help("Path to common stroking params");

  try {
    program.parse_args(argc, argv);
  } catch (const std::runtime_error& err) {
    ConsoleError("Main") << err.what();
    Console() << program;

    Logger::Stop();
    exit(0);
  }

  Console() << console_colors::green << "Welcome to Painter™!" << console_colors::remove_all_colors;
  LogConsoleInfo("Main") << "Using up to " << std::thread::hardware_concurrency() - 1 << " threads of "
                         << std::thread::hardware_concurrency();

  auto image_path = program.get("path");

  if (!std::filesystem::exists(image_path)) {
    LogConsoleError("Main") << "Invalid image path: " << image_path;
    Logger::Stop();
    exit(1);
  }
  LogConsoleInfo("Main") << "Image path: " << image_path;

  CommonStrokingParams params;
  auto params_path = program.get("--params");
  if (!params_path.empty()) {
    if (!std::filesystem::exists(params_path)) {
      LogConsoleError("Main") << "Invalid params path: " << params_path;
      Logger::Stop();
      exit(1);
    }
    params = load_params(params_path);
    LogConsoleSuccess("Main") << "Params loaded from: " << params_path;
  }

  launch_stroking(image_path, params);

  Logger::Stop();
  return 0;
}