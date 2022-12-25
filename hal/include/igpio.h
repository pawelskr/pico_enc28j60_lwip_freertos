#pragma once

#include <cinttypes>
#include <cstddef>

namespace drivers {

class IGpio {
  public:
    virtual void set() = 0;
    virtual void reset() = 0;
};
} // namespace drivers
