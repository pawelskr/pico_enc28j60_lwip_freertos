#include "enc28j60.h"
#include "alarm.h"
#include "enc28j60_registers.h"

namespace {
constexpr uint16_t RXSTART_INIT = 0x0;
constexpr uint16_t RXSTOP_INIT = (0x1FFF - 0x0600 - 1);
constexpr uint16_t TXSTART_INIT = (0x1FFF - 0x0600);
constexpr uint16_t TXSTOP_INIT = 0x1FFF;
} // namespace

namespace drivers::enc28j60 {

enc28j60::enc28j60(Config &config) : config_{config} {}

bool enc28j60::init(const MacAddress &mac_address) {

    config_.Cs.set();

    config_.Rst.reset();
    hal::sleep_milli(100);
    config_.Rst.set();

    write_op(ENC28J60_SOFT_RESET, 0x00, ENC28J60_SOFT_RESET);
    hal::sleep_milli(2);
    /** Oscillator ready */
    while (!(read_reg(ESTAT) & ESTAT_CLKRDY))
        ;

    /** RX buffer ptr */
    write_reg16(ERXST, RXSTART_INIT);
    write_reg16(ERXRDPT, RXSTART_INIT);
    write_reg16(ERXND, RXSTOP_INIT);

    /** TX buffer ptr */
    write_reg16(ETXST, TXSTART_INIT);
    write_reg16(ETXND, TXSTOP_INIT);

    write_phy(PHLCON, 0x476);

    read_phy(PHHID1);
    /** FILTERS setup */
    write_reg(ERXFCON, ERXFCON_UCEN | ERXFCON_CRCEN | ERXFCON_PMEN | ERXFCON_BCEN);

    write_reg16(EPMM0, 0x303f);
    write_reg16(EPMCS, 0xf7f9);

    /** Set the MARXEN bit in MACON1 to enable the MAC to receive frames. If using full duplex, most
     * applications should also set TXPAUS and RXPAUS to allow IEEE defined flow control to function
     */
    select_bank(MACON1);
    write_op(ENC28J60_BIT_FIELD_SET, MACON1, MACON1_MARXEN);
    /** Configure the PADCFG, TXCRCEN and FULDPX bits of MACON3. */
    write_op(ENC28J60_BIT_FIELD_SET, MACON3, MACON3_PADCFG0 | MACON3_TXCRCEN | MACON3_FRMLNEN);

    /** Program the MAMXFL registers with the maxi- mum frame length to be permitted to be received
     * or transmitted. MAX PDU - offset */
    write_reg16(MAMXFL, 1500);

    /** Configure the Back-to-Back Inter-Packet Gap register, MABBIPG. Most applications will pro-
     * gram this register with 15h when Full-Duplex mode is used and 12h when Half-Duplex mode is
     * used. */
    write_reg(MABBIPG, 0x12);
    read_reg(MABBIPG);

    /** Configure the Non-Back-to-Back Inter-Packet Gap register low byte, MAIPGL. Most applications
     * will program this register with 12h. If half duplex is used, the Non-Back-to-Back
     * Inter-Packet Gap register high byte, MAIPGH, should be programmed. Most applications will
     * program this register to 0Ch.*/
    write_reg16(MAIPG, 0x0C12);

    read_reg(MAIPG);
    read_reg(0x07 | 0x40 | 0x80);

    write_reg(MAADR5, mac_address[0]);
    write_reg(MAADR4, mac_address[1]);
    write_reg(MAADR3, mac_address[2]);
    write_reg(MAADR2, mac_address[3]);
    write_reg(MAADR1, mac_address[4]);
    write_reg(MAADR0, mac_address[5]);

    write_phy(PHCON2, PHCON2_HDLDIS);

    /** Start receiving */
    select_bank(ECON1);
    write_op(ENC28J60_BIT_FIELD_SET, EIE, EIE_INTIE | EIE_PKTIE | EIR_LINKIF);
    write_op(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_RXEN);

    uint8_t rev = read_reg(EREVID);
    return rev > 0;
}

bool enc28j60::is_link_up() { return (read_phy(PHSTAT2) & PHSTAT2_LSTAT); }

void enc28j60::write_op(const uint8_t op, const uint8_t addr, const uint8_t data) {
    config_.Cs.reset();
    const uint8_t operation = op | (addr & ENC_ADDR_MASK);
    config_.spi.write(&operation, sizeof(operation));
    config_.spi.write(&data, sizeof(data));
    config_.Cs.set();
}

uint8_t enc28j60::read_op(const uint8_t op, const uint8_t reg) {
    config_.Cs.reset();
    const uint8_t operation = op | (reg & ENC_ADDR_MASK);
    uint8_t incoming_data{};

    config_.spi.write(&operation, 1);
    config_.spi.read(&incoming_data, 1);

    if (reg & 0x80) {
        /** @note If this is MAC register, then read dummy byte first */
        config_.spi.read(&incoming_data, 1);
    }

    config_.Cs.set();
    return incoming_data;
}

void enc28j60::select_bank(const uint8_t address) {
    if (current_register_bank != (address & BANK_MASK)) {
        write_op(ENC28J60_BIT_FIELD_CLR, ECON1, ECON1_BSEL0 | ECON1_BSEL1);
        write_op(ENC28J60_BIT_FIELD_SET, ECON1, (address & BANK_MASK) >> 5);
        current_register_bank = address & BANK_MASK;
    }
}

void enc28j60::write_reg(const uint8_t addr, const uint8_t data) {
    select_bank(addr);
    write_op(ENC28J60_WRITE_CTRL_REG, addr, data);
}

void enc28j60::write_reg16(const uint8_t addr, const uint16_t data) {
    //    enc28j60::write_op_16bit(ENC28J60_WRITE_CTRL_REG, addr, data);
    write_reg(addr, data & 0xff);
    write_reg(addr + 1, data >> 8);
}

uint8_t enc28j60::read_reg(const uint8_t reg) {
    select_bank(reg);
    return read_op(ENC28J60_READ_CTRL_REG, reg);
}

void enc28j60::write_phy(const uint8_t reg, const uint16_t data) {
    /** 1. Write the address of the PHY register to write to into the MIREGADR register. */
    write_reg(MIREGADR, reg);
    /** 2. Write the lower 8 bits of data to write into the MIWRL register. */
    write_reg16(MIWR, data);
    /** 3. Write the upper 8 bits of data to write into the MIWRH register.
     * Writing to this register auto- matically begins the MIIM transaction, so it must be written
     * to after MIWRL. The MISTAT.BUSY bit becomes set. */
    while (enc28j60::read_reg(MISTAT) & MISTAT_BUSY)
        ;
}

uint16_t enc28j60::read_phy(const uint8_t reg) {
    /** 1. Write the address of the PHY register to read from into the MIREGADR register.  */
    enc28j60::write_reg(MIREGADR, reg);
    uint8_t xd = enc28j60::read_reg(MIREGADR);

    /** 2. Set the MICMD.MIIRD bit. The read operation begins and the MISTAT.BUSY bit is set. */
    enc28j60::write_reg(MICMD, MICMD_MIIRD);

    /** 3. Wait 10.24 μs. Poll the MISTAT.BUSY bit to be certain that the operation is complete.
     While busy, the host controller should not start any MIISCAN operations or write to the MIWRH
    register. When the MAC has obtained the register contents, the BUSY bit will clear itself.  */
    while (enc28j60::read_reg(MISTAT) & MISTAT_BUSY)
        ;

    /** 4. Clear the MICMD.MIIRD bit. */
    enc28j60::write_reg(MICMD, 0x00);

    /** 5. Read the desired data from the MIRDL and MIRDH registers. The order that these bytes are
     * accessed is unimportant. */
    uint8_t out_L = enc28j60::read_reg(MIRDL);
    uint8_t out_H = enc28j60::read_reg(MIRDH);
    return (out_H << 8) | out_L;
}

size_t enc28j60::read_buff(uint8_t *src, size_t len) {
    config_.Cs.reset();
    const uint8_t operation = ENC28J60_READ_BUF_MEM;
    config_.spi.write(&operation, 1);
    auto ret = config_.spi.read(src, len);
    config_.Cs.set();

    return ret;
}

void enc28j60::write_buff(const uint8_t *src, size_t len) {
    config_.Cs.reset();
    const uint8_t operation = ENC28J60_WRITE_BUF_MEM;
    config_.spi.write(&operation, 1);
    config_.spi.write(src, len);
    config_.Cs.set();
}

uint8_t enc28j60::get_number_of_packets() { return read_reg(EPKTCNT); }

size_t enc28j60::get_incoming_packet(const PacketMetaInfo &info, uint8_t *dst,
                                     const size_t max_length) {
    const size_t bytes_to_be_received = info.byte_count > max_length ? max_length : info.byte_count;
    size_t bytes_read = 0;

    if (dst != nullptr) {
        bytes_read = read_buff(dst, bytes_to_be_received);
    }

    while (bytes_read < bytes_to_be_received) {
        bytes_read += read_buff(dst + bytes_read, bytes_to_be_received - bytes_read);
        write_reg16(ERXRDPT, next_packet_pointer + bytes_read);
    }

    next_packet_pointer = info.next_packet_pointer;
    write_reg16(ERXRDPT, info.next_packet_pointer);
    write_op(ENC28J60_BIT_FIELD_SET, ECON2, ECON2_PKTDEC);

    return bytes_read;
}

bool enc28j60::send_packet(const uint8_t *src, const size_t len) {

    /* Latest errata sheet: DS80349C
     * always reset transmit logic (Errata Issue 12) */
    write_op(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_TXRST);
    write_op(ENC28J60_BIT_FIELD_CLR, ECON1, ECON1_TXRST);

    // Set the write pointer to start of transmit buffer area
    write_reg16(ETXST, TXSTART_INIT);
    write_reg16(EWRPT, TXSTART_INIT);

    // Set the TXND pointer to correspond to the packet size given
    // write per-packet control byte (0x00 means use macon3 settings)
    //    write_op(ENC28J60_WRITE_BUF_MEM, 0, 0x00);
    const uint8_t PPC = 0;
    write_buff(&PPC, 1);
    write_buff(src, len);

    write_reg16(ETXND, TXSTART_INIT + len);

    // Send data over network
    write_op(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_TXRTS);

    // http://ww1.microchip.com/downloads/en/DeviceDoc/80349c.pdf
    while (not(read_reg(EIR) & EIR_TXIF))
        ;

    uint8_t status = read_reg(ESTAT);
    if (status & ESTAT_TXABRT) {
        return false;
    }

    return true;
}

enc28j60::PacketMetaInfo enc28j60::get_incoming_packet_info() {
    PacketMetaInfo ret{};
    write_reg16(ERDPT, next_packet_pointer);
    read_buff(reinterpret_cast<uint8_t *>(&ret), sizeof(PacketMetaInfo));

    return ret;
}

bool enc28j60::link_state_changed() {
    if (current_link_state != is_link_up()) {
        current_link_state = !current_link_state;
        return true;
    }

    return false;
}

} // namespace drivers::enc28j60
