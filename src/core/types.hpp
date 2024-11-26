#pragma once

#include <cstdint>

#include <string>
#include <string_view>

#include <array>
#include <list>

namespace avio {

  using std::string;
  using std::string_view;

  template <class T, size_t Size>
  struct ArrayPool {
    inline T* allocate() {
      if (!free_list.empty()) {
        const size_t out_index = free_list.back();
        free_list.pop_back();
        return &objects[out_index];
      }
      return &objects[allocated_objects++];
    }

    inline void deallocate(T* object) {
      const size_t index = object - &objects[0];
      free_list.push_back(index);
      objects[index] = T();
    }

    std::array<T, Size> objects{};
    size_t allocated_objects = 0;
    std::list<size_t> free_list{};
  };

}  // namespace avio