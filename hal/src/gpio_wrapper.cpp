#include "gpio_wrapper.h"
#include "hardware/gpio.h"

namespace drivers::gpio {

Gpio::Gpio(uint32_t gpio_pin, uint32_t dir) : pin_{gpio_pin}, dir_{dir} {}

void Gpio::init() {
    gpio_init(pin_);
    gpio_set_dir(pin_, dir_);
}

void Gpio::set() { gpio_put(pin_, 1); }

void Gpio::reset() { gpio_put(pin_, 0); }

} // namespace drivers::gpio
