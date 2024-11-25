#pragma once

#include <format>
#include "types.hpp"

#include <stdexcept>

#include <spdlog/spdlog.h>

namespace avio {

class Error : public std::runtime_error {
 public:
  using std::runtime_error::runtime_error;

  template <class... Types>
  Error(std::format_string<Types...> fmt_string, Types&&... in_args)
      : std::runtime_error(
            std::format(fmt_string, std::forward<Types>(in_args)...)) {}
};

std::shared_ptr<spdlog::logger> get_logger();

}  // namespace avio

#define AV_ASSERT(cond)         \
  do {                          \
    if (!(cond)) {              \
      throw avio::Error(#cond); \
    }                           \
  } while (false)
#define AV_ASSERT_MSG(cond, format, ...)                        \
  do {                                                          \
    if (!(cond)) {                                              \
      throw avio::Error("\"" #cond "\". " format, __VA_ARGS__); \
    }                                                           \
  } while (false)

#define AV_LOG(level, format, ...) \
  avio::get_logger()->level(format, __VA_ARGS__)