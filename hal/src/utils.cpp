#include "utils.h"
#include "pico/time.h"

namespace hal {

#ifdef NO_SYS
void sleep_milli(const uint32_t time_ms) { sleep_ms(time_ms); }
#endif
void panic() { asm volatile("BKPT"); }

} // namespace hal
