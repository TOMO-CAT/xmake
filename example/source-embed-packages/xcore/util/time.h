#pragma once

#include <cstdlib>
#include "common/header.pb.h"

namespace xmake {
namespace xcore {

class Time {
 public:
  Time() = default;
  ~Time() = default;

 public:
  uint64_t ToNanosecond() const;

 private:
  uint64_t nanoseconds_ = 0;
};

}
}
