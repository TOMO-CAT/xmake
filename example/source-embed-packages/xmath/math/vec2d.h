#pragma once

#include "util/time.h"

namespace xmake {
namespace xmath {

class Vec2d {
 public:
  constexpr Vec2d() noexcept : Vec2d(0.0, 0.0) {
  }
  constexpr Vec2d(const double x, const double y) noexcept : x_(x), y_(y) {
  }

 public:
  double LengthSquare() const;

 private:
  double x_ = 0.0;
  double y_ = 0.0;
};

}
}
