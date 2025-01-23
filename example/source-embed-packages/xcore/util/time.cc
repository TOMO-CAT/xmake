#include "util/time.h"

namespace xmake {
namespace xcore {

uint64_t Time::ToNanosecond() const {
  return nanoseconds_;
}

}
}
