#pragma once

#include "hardware/spi.h"
#include "ispi.h"

namespace drivers::spi {

struct Config {
    spi_inst_t *spi_handle;
    uint8_t CLK_Pin;
    uint8_t MOSI_Pin;
    uint8_t MISO_Pin;
    uint32_t baudrate_Hz;
    spi_cpol_t clock_polarization = SPI_CPOL_0;
    spi_cpha_t clock_phase = SPI_CPHA_0;
    spi_order_t bit_order = SPI_MSB_FIRST;
};

class SpiWrapper : public ISpi {
  public:
    explicit SpiWrapper(const Config &confiig);

    bool init();
    size_t read(uint8_t *dst, const size_t len) override;
    bool write(const uint8_t *src, const size_t len) override;
    bool transceive(const uint8_t *src, uint8_t *dst, const size_t tx_len,
                    const size_t rx_len) override;

  private:
    const Config &config_;
};

} // namespace drivers::spi