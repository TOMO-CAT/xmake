#include "math/vec2d.h"

namespace xmake {
namespace xmath {

double Vec2d::LengthSquare() const {
  return x_ * x_ + y_ * y_;
}

}
}
