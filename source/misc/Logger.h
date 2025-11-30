#pragma once
#include <spdlog/spdlog.h>

#ifdef TRACE_LOG
#define DEBUG_LOG
#endif

namespace global {

class Logger {
 public:
  static Logger& get_instance();

  template <typename... Args>
  void log_info(spdlog::format_string_t<Args...> fmt, Args&&... args);
  template <typename T>
  void log_info(const T& message);

  template <typename... Args>
  void log_warning(spdlog::format_string_t<Args...> fmt, Args&&... args);
  template <typename T>
  void log_warning(const T& message);

  template <typename... Args>
  void log_error(spdlog::format_string_t<Args...> fmt, Args&&... args);
  template <typename T>
  void log_error(const T& message);

  template <typename... Args>
  void log_critical(spdlog::format_string_t<Args...> fmt, Args&&... args);
  template <typename T>
  void log_critical(const T& message);

  template <typename... Args>
  void log_debug(spdlog::format_string_t<Args...> fmt, Args&&... args);
  template <typename T>
  void log_debug(const T& message);

  template <typename... Args>
  void log_trace(spdlog::format_string_t<Args...> fmt, Args&&... args);
  template <typename T>
  void log_trace(const T& message);

 private:
  std::vector<std::shared_ptr<spdlog::logger>> loggers_;
  Logger();
};

}  // namespace global

namespace global {

template <typename... Args>
void Logger::log_info(spdlog::format_string_t<Args...> fmt, Args&&... args) {
  for (auto& logger : loggers_) {
    logger->info(fmt, std::forward<Args>(args)...);
    logger->flush();
  }
}

template <typename T>
void Logger::log_info(const T& message) {
  for (auto& logger : loggers_) {
    logger->info(message);
    logger->flush();
  }
}

template <typename... Args>
void Logger::log_warning(spdlog::format_string_t<Args...> fmt, Args&&... args) {
  for (auto& logger : loggers_) {
    logger->warn(fmt, std::forward<Args>(args)...);
    logger->flush();
  }
}

template <typename T>
void Logger::log_warning(const T& message) {
  for (auto& logger : loggers_) {
    logger->warn(message);
    logger->flush();
  }
}

template <typename... Args>
void Logger::log_error(spdlog::format_string_t<Args...> fmt, Args&&... args) {
  for (auto& logger : loggers_) {
    logger->error(fmt, std::forward<Args>(args)...);
    logger->flush();
  }
}

template <typename T>
void Logger::log_error(const T& message) {
  for (auto& logger : loggers_) {
    logger->error(message);
    logger->flush();
  }
}

template <typename... Args>
void Logger::log_critical(spdlog::format_string_t<Args...> fmt,
                          Args&&... args) {
  for (auto& logger : loggers_) {
    logger->critical(fmt, std::forward<Args>(args)...);
    logger->flush();
  }
}

template <typename T>
void Logger::log_critical(const T& message) {
  for (auto& logger : loggers_) {
    logger->critical(message);
    logger->flush();
  }
}

template <typename... Args>
void Logger::log_debug(spdlog::format_string_t<Args...> fmt, Args&&... args) {
#ifdef DEBUG_LOG
  for (auto& logger : loggers_) {
    logger->debug(fmt, std::forward<Args>(args)...);
    logger->flush();
  }
#endif
}

template <typename T>
void Logger::log_debug(const T& message) {
#ifdef DEBUG_LOG
  for (auto& logger : loggers_) {
    logger->debug(message);
    logger->flush();
  }
#endif
}

template <typename... Args>
void Logger::log_trace(spdlog::format_string_t<Args...> fmt, Args&&... args) {
#ifdef TRACE_LOG
  for (auto& logger : loggers_) {
    logger->trace(fmt, std::forward<Args>(args)...);
    logger->flush();
  }
#endif
}

template <typename T>
void Logger::log_trace(const T& message) {
#ifdef TRACE_LOG
  for (auto& logger : loggers_) {
    logger->trace(message);
    logger->flush();
  }
#endif
}

}  // namespace global