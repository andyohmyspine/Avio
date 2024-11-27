#pragma once

#include <format>
#include "types.hpp"

#include <stdexcept>

#define FMT_UNICODE 0
#include <spdlog/spdlog.h>

#define AV_PASTE(a, b) a##b
#define AV_PASTE2(a, b) AV_PASTE(a, b)

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

namespace avio {
  struct GuardedInvoke {
    template <typename Func>
    GuardedInvoke(Func&& func) {
      try {
        func();
      } catch (const Error& error) {
        AV_LOG(critical, "Exception caught: {}", error.what());
        throw;
      } catch (const std::runtime_error& error) {
        AV_LOG(critical, "Exception caught: {}", error.what());
        throw;
      } catch (const std::exception& error) {
        AV_LOG(critical, "Exception caught: {}", error.what());
        throw;
      }
    }
  };
}  // namespace avio

#define AV_COMMON_CATCH(...) \
  avio::GuardedInvoke AV_PASTE2(_guarded_invoke, __LINE__) = [&, __VA_ARGS__]