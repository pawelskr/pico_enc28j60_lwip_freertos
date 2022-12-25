#pragma once

#include <cinttypes>
#include <cstddef>

namespace drivers {

class ISpi {
  public:
    virtual size_t read(uint8_t *dst, const size_t len) = 0;
    virtual bool write(const uint8_t *src, const size_t len) = 0;
    virtual bool transceive(const uint8_t *src, uint8_t *dst, const size_t tx_len,
                            const size_t rx_len) = 0;
};

} // namespace drivers
