#include "Logger.h"

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace global {

Logger& Logger::get_instance() {
  static Logger logger;
  return logger;
}

Logger::Logger() {
  auto file_logger = spdlog::basic_logger_mt("File Logger", "logs/latest.txt", true);
  file_logger->set_level(spdlog::level::trace);
  loggers_.push_back(std::move(file_logger));

  auto stdout_logger = spdlog::stdout_color_mt("Stdout Logger");
  stdout_logger->set_level(spdlog::level::trace);
  loggers_.push_back(std::move(stdout_logger));
}

}  // namespace global