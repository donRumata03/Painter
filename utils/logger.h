#pragma once

#include <utility>

#include "painter_pch.h"
#include "progress.h"


enum class LogLevel {
  Debug = 0,
  Info = 1,
  Common = 2, // Using for simple output without any additional info
  Success = 3,
  Warning = 4,
  Error = 5,
};

class Logger {
  friend class LogStreamProxy;

 public:
  struct LogParams {
    LogLevel level = LogLevel::Info;
    std::string module;
    bool output_in_console = true;
    bool output_in_file = true;
  };

  class LogStreamProxy;

  class LogStream {
    friend class LogStreamProxy;

   public:
    ~LogStream() {
      if (flush) {
        Logger::Instance().flush_mutex.lock();
        if (IsNeedToLog(dest.level)) {
          Logger::Instance().push_log(buffer.str(), dest);
        }
        buffer.str("");
        Logger::Instance().flush_mutex.unlock();
      }
    }

    template <typename T>
    LogStream &operator<<(const T &data) {
      buffer << data;
      return *this;
    }

    LogStream(const LogStream &) = delete;

   private:
    explicit LogStream(LogParams dest) : dest(std::move(dest)), buffer(Logger::buffer) {}

    LogStream(LogStream &prev) : dest(prev.dest), buffer(prev.buffer), flush(prev.flush) {
      prev.flush = false;
    }

    std::ostringstream &buffer;
    LogParams dest;
    bool flush = true;

  };

  class LogStreamProxy {
    friend class Logger;

   public:
    explicit LogStreamProxy(LogParams dest) : dest(std::move(dest)) {}

    template <typename T>
    LogStream operator<<(const T &data) {
      LogStream stream(dest);
      return (stream << data);
    }

   private:
    LogParams dest;
  };

  static void SetLogFile(const fs::path &output_path) {
    Instance().set_log_file(output_path);
  }

  static void Stop() {
    Instance().stop();
  }

  static LogLevel GetLoggingLevel() { return Instance().log_level; }

  static void SetLoggingLevel(LogLevel level) { Instance().log_level = level; }

  static bool IsNeedToLog(LogLevel level) { return (int) level >= (int) GetLoggingLevel(); }

  static void UpdateProgress(size_t add = 1) {
    Instance().update_progress(add);
  }

  static void NewProgress(size_t total, size_t start = 0) {
    Instance().new_progress(total, start);
  }

  static size_t GetCurrentProgress() {
    return Instance().progress->GetCurrent();
  }

  static Logger &Instance() {
    static Logger logger;
    return logger;
  }

  LogStreamProxy operator()(LogLevel level, const std::string &module, bool output_in_console, bool output_in_file) {
    return LogStreamProxy({level, module, output_in_console, output_in_file});
  }

 private:
  Logger() : console_stream(std::cout), is_working(true), log_level(LogLevel::Debug) {
    thread = std::thread(&Logger::run, this);
  }

  Logger(const Logger &) = delete;
  Logger &operator=(Logger &) = delete;

  inline static std::unordered_map<LogLevel, std::string> LogLevelStrings = {
          {LogLevel::Debug,   "DEBUG"},
          {LogLevel::Info,    "INFO"},
          {LogLevel::Common,  ""},
          {LogLevel::Success, "SUCCESS"},
          {LogLevel::Warning, "WARN"},
          {LogLevel::Error,   "ERROR"}
  };
  inline static std::unordered_map<LogLevel, console_colors> LogLevelColors = {
          {LogLevel::Debug,   console_colors::dark_cyan},
          {LogLevel::Info,    console_colors::remove_last_color},
          {LogLevel::Common,  console_colors::remove_last_color},
          {LogLevel::Success, console_colors::green},
          {LogLevel::Warning, console_colors::yellow},
          {LogLevel::Error,   console_colors::red}
  };

  void set_log_file(const fs::path &output_path) {
    if (!std::filesystem::exists(output_path.parent_path())) {
      std::filesystem::create_directories(output_path.parent_path());
    }
    file_stream = std::ofstream(output_path);
  }

  void stop() {
    is_working = false;
    flush_cond.notify_all();
    thread.join();
  }

  void flush_to_file() {
    while (!file_queue.empty()) {
      file_stream << file_queue.front() << std::endl;
      file_queue.pop();
    }
  }

  void flush_to_console() {
    while (!console_queue.empty()) {
      console_stream << console_queue.front() << std::endl;
      console_queue.pop();
    }
  }

  void run() {
    while (is_working) {
      if (file_queue.empty() && console_queue.empty()) {
        std::unique_lock<std::mutex> locker(wait_mutex);
        flush_cond.wait(locker, [&]() { return !file_queue.empty() || !console_queue.empty() || !is_working; });
      }
      print_mutex.lock();

      flush_to_file();
      if (progress.has_value()) {
        bool redraw = (std::chrono::duration_cast<std::chrono::milliseconds>(
                progress->GetLastUpdateTime() - last_draw).count() / 1000.) >= min_draw_delay;

        if (redraw) progress->Clear();
        flush_to_console();
        if (redraw) {
          progress->Draw();
          last_draw = progress->GetLastUpdateTime();
        }

      } else flush_to_console();

      print_mutex.unlock();

      std::this_thread::sleep_for(100ms);
    }

    flush_to_file();
    flush_to_console();
  }

  void push_log(const std::string &message, const LogParams& dest) {
    std::stringstream time, module;
    if (dest.level != LogLevel::Common) {
      const auto timestamp = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
      time << "[" << std::put_time(std::localtime(&timestamp), "%d.%m.%Y %H:%M:%S") << "]";
      if (!dest.module.empty()) module << "[" << dest.module << "] ";
      module << LogLevelStrings[dest.level] << ": ";
    }

    if (dest.output_in_console) {
      std::stringstream ss;
      ss << LogLevelColors[dest.level] << module.str() << message << console_colors::remove_all_colors;
      console_queue.push(ss.str());
    }

    if (dest.output_in_file) {
      std::stringstream ss;
      if (dest.level != LogLevel::Common) ss << time.str() << module.str();
      ss << message;
      file_queue.push(ss.str());
    }

    flush_cond.notify_one();
  }

  void new_progress(size_t total, size_t start) {
    print_mutex.lock();
    if (progress.has_value()) {
      progress->End();
      progress.reset();
    }
    progress.emplace(total, start);
    print_mutex.unlock();
  }

  void update_progress(size_t add = 1) {
    print_mutex.lock();
    progress->Update(add);
    if (!progress->IsWorking()) progress.reset();
    print_mutex.unlock();
  }

  inline static thread_local std::ostringstream buffer;
  LogLevel log_level; // Will log only with (int)level >= (int)log_level

  std::thread thread;
  std::condition_variable flush_cond;
  std::mutex flush_mutex;
  std::mutex wait_mutex;
  std::mutex print_mutex;
  bool is_working;

  std::ofstream file_stream;
  std::ostream &console_stream;

  std::queue<std::string> file_queue;
  std::queue<std::string> console_queue;

  std::optional<Progress> progress;
  std::chrono::time_point<std::chrono::system_clock> last_draw;
  const double min_draw_delay = 1. / 20;
};

/// UTILS

// TODO: find better place for that function
template <class ...T>
static std::string concat(const std::string &sep, const T &... args) {
  std::stringstream ss;
  ((ss << sep << args), ...); // left fold
  return ss.str();
}

/// LOGGER CALLERS

template <class ...T>
inline Logger::LogStreamProxy ConsoleDebug(const std::string &module = "", const T &... submodules) {
  return Logger::Instance()(LogLevel::Debug, module + concat("][", submodules...), true, false);
}

template <class ...T>
inline Logger::LogStreamProxy LogDebug(const std::string &module = "", const T &... submodules) {
  return Logger::Instance()(LogLevel::Debug, module + concat("][", submodules...), false, true);
}

template <class ...T>
inline Logger::LogStreamProxy LogConsoleDebug(const std::string &module = "", const T &... submodules) {
  return Logger::Instance()(LogLevel::Debug, module + concat("][", submodules...), true, true);
}


template <class ...T>
inline Logger::LogStreamProxy ConsoleInfo(const std::string &module = "", const T &... submodules) {
  return Logger::Instance()(LogLevel::Info, module + concat("][", submodules...), true, false);
}

template <class ...T>
inline Logger::LogStreamProxy LogInfo(const std::string &module = "", const T &... submodules) {
  return Logger::Instance()(LogLevel::Info, module + concat("][", submodules...), false, true);
}

template <class ...T>
inline Logger::LogStreamProxy LogConsoleInfo(const std::string &module = "", const T &... submodules) {
  return Logger::Instance()(LogLevel::Info, module + concat("][", submodules...), true, true);
}


template <class ...T>
inline Logger::LogStreamProxy Console(const std::string &module = "", const T &... submodules) {
  return Logger::Instance()(LogLevel::Common, module + concat("][", submodules...), true, false);
}

template <class ...T>
inline Logger::LogStreamProxy Log(const std::string &module = "", const T &... submodules) {
  return Logger::Instance()(LogLevel::Common, module + concat("][", submodules...), false, true);
}

template <class ...T>
inline Logger::LogStreamProxy LogConsole(const std::string &module = "", const T &... submodules) {
  return Logger::Instance()(LogLevel::Common, module + concat("][", submodules...), true, true);
}


template <class ...T>
inline Logger::LogStreamProxy ConsoleSuccess(const std::string &module = "", const T &... submodules) {
  return Logger::Instance()(LogLevel::Success, module + concat("][", submodules...), true, false);
}

template <class ...T>
inline Logger::LogStreamProxy LogSuccess(const std::string &module = "", const T &... submodules) {
  return Logger::Instance()(LogLevel::Success, module + concat("][", submodules...), false, true);
}

template <class ...T>
inline Logger::LogStreamProxy LogConsoleSuccess(const std::string &module = "", const T &... submodules) {
  return Logger::Instance()(LogLevel::Success, module + concat("][", submodules...), true, true);
}


template <class ...T>
inline Logger::LogStreamProxy ConsoleWarning(const std::string &module = "", const T &... submodules) {
  return Logger::Instance()(LogLevel::Warning, module + concat("][", submodules...), true, false);
}

template <class ...T>
inline Logger::LogStreamProxy LogWarning(const std::string &module = "", const T &... submodules) {
  return Logger::Instance()(LogLevel::Warning, module + concat("][", submodules...), false, true);
}

template <class ...T>
inline Logger::LogStreamProxy LogConsoleWarning(const std::string &module = "", const T &... submodules) {
  return Logger::Instance()(LogLevel::Warning, module + concat("][", submodules...), true, true);
}


template <class ...T>
inline Logger::LogStreamProxy ConsoleError(const std::string &module = "", const T &... submodules) {
  return Logger::Instance()(LogLevel::Error, module + concat("][", submodules...), true, false);
}

template <class ...T>
inline Logger::LogStreamProxy LogError(const std::string &module = "", const T &... submodules) {
  return Logger::Instance()(LogLevel::Error, module + concat("][", submodules...), false, true);
}

template <class ...T>
inline Logger::LogStreamProxy LogConsoleError(const std::string &module = "", const T &... submodules) {
  return Logger::Instance()(LogLevel::Error, module + concat("][", submodules...), true, true);
}

inline void ensure_log_cleared(const fs::path& log_path = painter_base_path / "log" / "latest")
{
  if(fs::exists(log_path)) {
    fs::remove_all(log_path);
  }
  fs::create_directories(log_path);
}
