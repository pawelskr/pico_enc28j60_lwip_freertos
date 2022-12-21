#include "enc28j60.h"
#include "enc28j60_registers.h"
#include "hardware/spi.h"
#include "pico/stdlib.h"
#include "spi.h"
#include <array>
#include <stdio.h>

#define RST_PIN 0
#define CS_PIN 1
#define MISO_PIN 4
#define MOSI_PIN 3
#define CLK_PIN 2
#define ENC_IRQ 15
#define ENC_SPI spi0


void gpio_callback(uint gpio, uint32_t events) {
    // Put the GPIO event(s) that just happened into event_str
    // so we can print it
    uint8_t xd = 0;
    xd++;
}



std::array<uint8_t, 256> buffer{};

int main() {
    stdio_init_all();

    const uint led_pin = 25;

    // Initialize LED pin
    gpio_init(led_pin);
    gpio_set_dir(led_pin, GPIO_OUT);

    gpio_init(CS_PIN);
    gpio_set_dir(CS_PIN, GPIO_OUT);
    cs_deselect();

    gpio_init(RST_PIN);
    gpio_set_dir(RST_PIN, GPIO_OUT);
    gpio_put(RST_PIN, 0);
    sleep_ms(100);
    gpio_put(RST_PIN, 1);

    // SPI INIT HERE

    // ENC28J60 INIT
    /** Soft reset */
    enc_write_op(ENC28J60_SOFT_RESET, 0x00, ENC28J60_SOFT_RESET);
    sleep_ms(2);
    /** Oscillator ready */
    while (!(enc_read_reg(ESTAT) & ESTAT_CLKRDY))
        ;

    /** RX buffer ptr */
    enc_write_reg16(ERXST, RXSTART_INIT);
    enc_write_reg16(ERXRDPT, RXSTART_INIT);
    enc_write_reg16(ERXND, RXSTOP_INIT);

    /** TX buffer ptr */
    enc_write_reg16(ETXST, TXSTART_INIT);
    enc_write_reg16(ETXND, TXSTOP_INIT);

    enc_write_phy(PHLCON, 0x476);

    enc_read_phy(PHHID1);
    /** FILTERS setup */
    enc_write_reg(ERXFCON, ERXFCON_UCEN | ERXFCON_CRCEN | ERXFCON_PMEN | ERXFCON_BCEN);

    enc_write_reg16(EPMM0, 0x303f);
    enc_write_reg16(EPMCS, 0xf7f9);

    /** Set the MARXEN bit in MACON1 to enable the MAC to receive frames. If using full duplex, most
     * applications should also set TXPAUS and RXPAUS to allow IEEE defined flow control to function
     */
    enc_select_bank(MACON1);
    enc_write_op(ENC28J60_BIT_FIELD_SET, MACON1, MACON1_MARXEN);
    /** Configure the PADCFG, TXCRCEN and FULDPX bits of MACON3. */
    enc_write_op(ENC28J60_BIT_FIELD_SET, MACON3, MACON3_PADCFG0 | MACON3_TXCRCEN | MACON3_FRMLNEN);

    /** Program the MAMXFL registers with the maxi- mum frame length to be permitted to be received
     * or transmitted. MAX PDU - offset */
    enc_write_reg16(MAMXFL, 1500);

    /** Configure the Back-to-Back Inter-Packet Gap register, MABBIPG. Most applications will pro-
     * gram this register with 15h when Full-Duplex mode is used and 12h when Half-Duplex mode is
     * used. */
    enc_write_reg(MABBIPG, 0x12);
    enc_read_reg(MABBIPG);
    /** Configure the Non-Back-to-Back Inter-Packet Gap register low byte, MAIPGL. Most applications
     * will program this register with 12h. If half duplex is used, the Non-Back-to-Back
     * Inter-Packet Gap register high byte, MAIPGH, should be programmed. Most applications will
     * program this register to 0Ch.*/

    enc_write_reg16(MAIPG, 0x0C12);
    //    enc_write_reg(0x06 | 0x40 | 0x80, 0x0c);
    //    enc_write_reg(0x07 | 0x40 | 0x80, 0x12);

    enc_read_reg(MAIPG);
    enc_read_reg(0x07 | 0x40 | 0x80);

    uint8_t mac_read[6]{};
    enc_write_reg(MAADR5, 0x0A);
    enc_write_reg(MAADR4, 0xbd);
    enc_write_reg(MAADR3, 0x7d);
    enc_write_reg(MAADR2, 0x95);
    enc_write_reg(MAADR1, 0xd3);
    enc_write_reg(MAADR0, 0xa5);

    mac_read[5] = enc_read_reg(MAADR5);
    mac_read[4] = enc_read_reg(MAADR4);
    mac_read[3] = enc_read_reg(MAADR3);
    mac_read[2] = enc_read_reg(MAADR2);
    mac_read[1] = enc_read_reg(MAADR1);
    mac_read[0] = enc_read_reg(MAADR0);

    enc_write_phy(PHCON2, PHCON2_HDLDIS);

    /** Start receiving */
    enc_select_bank(ECON1);
    enc_write_op(ENC28J60_BIT_FIELD_SET, EIE, EIE_INTIE | EIE_PKTIE | EIR_LINKIF);
    enc_write_op(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_RXEN);

    uint8_t rev = enc_read_reg(EREVID);

    while (not((enc_read_phy(PHSTAT2) >> 2) & 1))
        ;

    uint8_t number_of_packets = 0;
    // Loop forever
    while (true) {
        number_of_packets = enc_read_reg(EPKTCNT);
        if (number_of_packets > 0) {
            enc_write_reg16(ERDPT, rx_packet_pointer);
            PacketMetaInfo info{};
            enc_read_buff(reinterpret_cast<uint8_t *>(&info), 6);
            buffer.fill(0);
            enc_read_buff(buffer.data(), info.byte_count);
            rx_packet_pointer = info.next_packet_pointer;
            enc_write_reg16(ERXRDPT, info.next_packet_pointer);
            asm volatile("BKPT");
        }

        // Blink LED
        printf("Blinking!\r\n");
        gpio_put(led_pin, true);
        sleep_ms(100);
        gpio_put(led_pin, false);
        sleep_ms(100);
    }
}