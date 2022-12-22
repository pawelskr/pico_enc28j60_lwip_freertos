#include "alarm.h"
#include "pico/time.h"

namespace hal {

void sleep_milli(const uint32_t time_ms) { sleep_ms(time_ms); }
void panic() { asm volatile("BKPT"); }

} // namespace hal