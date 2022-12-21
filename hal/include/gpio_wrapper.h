#pragma once
#include "igpio.h"
#include <cstdint>

namespace drivers::gpio {

class Gpio : public IGpio {
  public:
    Gpio(uint32_t gpio_pin, uint32_t dir);
    void init();
    void set() override;
    void reset() override;

  private:
    const uint32_t pin_;
    const uint32_t dir_;
};

} // namespace drivers::gpio
