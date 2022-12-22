#pragma once
#include "igpio.h"
#include "ispi.h"
#include <array>
#include <cinttypes>

namespace drivers::enc28j60 {

struct Config {
    IGpio &Cs;
    IGpio &Rst;
    ISpi &spi;
};

class enc28j60 {
  public:
    using MacAddress = std::array<uint8_t, 6>;

    struct __attribute__((packed)) PacketMetaInfo {
        uint16_t next_packet_pointer;
        uint16_t byte_count;
        uint16_t long_drop_event : 1;
        uint16_t reserved : 1;
        uint16_t carrier_event_previously_seen : 1;
        uint16_t reserved_2 : 1;
        uint16_t crc_err : 1;
        uint16_t length_check_err : 1;
        uint16_t length_out_of_range : 1;
        uint16_t received_ok : 1;
        uint16_t receive_multicast_packet : 1;
        uint16_t receive_broadcast_packet : 1;
        uint16_t dribble_nibble : 1;
        uint16_t receive_control_frame : 1;
        uint16_t receive_pause_control_frame : 1;
        uint16_t receive_unknown_opcode : 1;
        uint16_t receive_vlan_type_detected : 1;
        uint16_t zero : 1;
    };

    enc28j60(Config &config);

    bool init(const MacAddress &mac_address);
    bool is_link_up();
    uint8_t get_number_of_packets();
    uint8_t get_incoming_packet(const PacketMetaInfo &info, uint8_t *dst, const size_t max_length);
    PacketMetaInfo get_incoming_packet_info();
    bool send_packet(const uint8_t *src, const size_t len);

  private:
    Config &config_;

    void write_op(uint8_t operation, const uint8_t reg, const uint8_t data);
    uint8_t read_op(uint8_t operation, const uint8_t reg);
    void select_bank(const uint8_t address);

    void write_reg(const uint8_t addr, const uint8_t data);
    void write_reg16(const uint8_t addr, const uint16_t data);
    uint8_t read_reg(const uint8_t reg);

    void write_phy(const uint8_t reg, const uint16_t data);
    uint16_t read_phy(const uint8_t reg);

    uint8_t read_buff(uint8_t *dst, size_t len);
    void write_buff(const uint8_t *src, size_t len);

    uint8_t current_register_bank;
    uint16_t next_packet_pointer;
};
} // namespace drivers::enc28j60
