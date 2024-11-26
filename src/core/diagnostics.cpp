#include "diagnostics.hpp"

#include <spdlog/sinks/stdout_color_sinks.h>

namespace avio {

  struct LoggingInfo {
    LoggingInfo() : _logger(spdlog::stdout_color_mt<>("Avio")) {}

    std::shared_ptr<spdlog::logger> _logger;
  };

  static LoggingInfo g_logger{};

  std::shared_ptr<spdlog::logger> get_logger() {
    return g_logger._logger;
  }

}  // namespace avio