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
        : std::runtime_error(std::format(fmt_string, std::forward<Types>(in_args)...)) {}
  };

  std::shared_ptr<spdlog::logger> get_logger();

}  // namespace avio

#include <concepts>
namespace avio {
  void check(std::convertible_to<bool> auto condition) {
    if(!condition) {
      throw avio::Error("Condition failed.");
    }
  }

  template<typename ... Args>
  void check_msg(std::convertible_to<bool> auto condition, fmt::format_string<Args...> format, Args&&... args) {
    if(!condition) {
      throw avio::Error("Condition failed. {}", fmt::format(format, std::forward<Args>(args)...));
    }
  }

  namespace log {
    template <typename... Args>
    void trace(fmt::format_string<Args...> format, Args&&... args) {
      avio::get_logger()->trace(format, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void debug(fmt::format_string<Args...> format, Args&&... args) {
      avio::get_logger()->debug(format, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void info(fmt::format_string<Args...> format, Args&&... args) {
      avio::get_logger()->info(format, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void warn(fmt::format_string<Args...> format, Args&&... args) {
      avio::get_logger()->warn(format, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void error(fmt::format_string<Args...> format, Args&&... args) {
      avio::get_logger()->error(format, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void critical(fmt::format_string<Args...> format, Args&&... args) {
      avio::get_logger()->critical(format, std::forward<Args>(args)...);
    }
  }  // namespace log
}  // namespace avio

namespace avio {
  struct GuardedInvoke {
    template <typename Func>
    GuardedInvoke(Func&& func) {
      try {
        func();
      } catch (const Error& error) {
        log::critical("Exception caught: {}", error.what());
        throw;
      } catch (const std::runtime_error& error) {
        log::critical("Exception caught: {}", error.what());
        throw;
      } catch (const std::exception& error) {
        log::critical("Exception caught: {}", error.what());
        throw;
      }
    }
  };
}  // namespace avio

#define AV_COMMON_CATCH(...) avio::GuardedInvoke AV_PASTE2(_guarded_invoke, __LINE__) = 