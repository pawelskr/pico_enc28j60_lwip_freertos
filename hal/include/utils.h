#pragma once
#include <cstdint>

namespace hal {
void sleep_milli(const uint32_t time_ms);
void panic();
} // namespace hal
