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

  struct Color {
    constexpr Color() : r(0), g(0), b(0), a(0) {}
    constexpr Color(float scalar) : r(scalar), g(scalar), b(scalar), a(scalar) {}
    constexpr Color(float in_r, float in_g, float in_b, float in_a = 1.0f) : r(in_r), g(in_g), b(in_b), a(in_a) {}
    constexpr Color(const Color& other) : r(other.r), g(other.g), b(other.b), a(other.a) {}

    union {
      struct {
        float r;
        float g;
        float b;
        float a;
      };

      float rg[2];
      float rgb[3];
      float rgba[4];
    };
  };

  namespace colors {
    inline constexpr Color black = Color(0.0f, 0.0f, 0.0f, 1.0f);
    inline constexpr Color white = Color(1.0f, 1.0f, 1.0f, 1.0f);

    inline constexpr Color red = Color(1.0f, 0.0f, 0.0f, 1.0f);
    inline constexpr Color green = Color(0.0f, 1.0f, 0.0f, 1.0f);
    inline constexpr Color blue = Color(0.0f, 0.0f, 1.0f, 1.0f);

    inline constexpr Color yellow = Color(1.0f, 1.0f, 0.0f, 1.0f);
    inline constexpr Color magenta = Color(1.0f, 0.0f, 1.0f, 1.0f);
    inline constexpr Color cyan = Color(0.0f, 1.0f, 1.0f, 1.0f);
  }

}  // namespace avio