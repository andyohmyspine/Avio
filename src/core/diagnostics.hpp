#pragma once

#include "types.hpp"
#include <format>

#include <stdexcept>

namespace avio {

class Error : public std::runtime_error {
public:
  using std::runtime_error::runtime_error;

  template<class ... Types>
  Error(std::format_string<Types...> fmt_string, Types&&... in_args)
    : std::runtime_error(std::format(fmt_string, std::forward<Types>(in_args)...)) {
  }
};

}

#define AV_ASSERT(cond) do { if(!(cond)) { throw avio::Error(#cond); } } while(false)
#define AV_ASSERT_MSG(cond, format, ...) do { if(!(cond)) { throw avio::Error(#cond format, __VA_ARGS__); } } while(false)