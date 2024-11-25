#pragma once

#include <concepts>
#include <type_traits>
#include <cstdlib>

namespace avio {
  template<typename T>
  void zero_mem(T& val) {
    if constexpr(std::is_pointer_v<T>) {
      memset(val, 0, sizeof(*val));
    } else {
      memset(&val, 0, sizeof(val));
    }
  }
}