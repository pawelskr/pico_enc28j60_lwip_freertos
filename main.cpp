#include "alarm.h"
#include "enc28j60.h"
#include "enc28j60_registers.h"
#include "gpio_wrapper.h"
#include "hardware/spi.h"
#include "pico/stdlib.h"
#include "spi.h"
#include <array>
#include <cstring>
#include <stdio.h>

#include "httpd.h"
#include "lwip/dhcp.h"
#include "lwip/inet.h"
#include "lwip/init.h"
#include "lwip/netif.h"
#include "lwip/stats.h"
#include "lwip/tcp.h"
#include "lwip/timeouts.h"
#include "netif/etharp.h"

constexpr uint8_t MISO_PIN = 4;
constexpr uint8_t MOSI_PIN = 3;
constexpr uint8_t CLK_PIN = 2;
constexpr uint8_t ENC_IRQ = 15;
constexpr uint8_t LED_PIN = 25;

drivers::gpio::Gpio EncRstPin{0, GPIO_OUT};
drivers::gpio::Gpio EncCsPin{1, GPIO_OUT};
drivers::spi::Config spi0Config{spi0, CLK_PIN, MOSI_PIN, MISO_PIN, 8 * 1000000};
drivers::spi::SpiWrapper spi0_{spi0Config};

drivers::enc28j60::Config EncConfig{EncCsPin, EncRstPin, spi0_};
drivers::enc28j60::enc28j60 eth_driver{EncConfig};

drivers::gpio::Gpio BoardLed{LED_PIN, GPIO_OUT};

void gpio_callback(uint gpio, uint32_t events) {
    // Put the GPIO event(s) that just happened into event_str
    // so we can print it
    uint8_t xd = 0;
    xd++;
}

#define ETHERNET_MTU 1500
constexpr drivers::enc28j60::enc28j60::MacAddress mac{0x0a, 0xbd, 0x7d, 0x95, 0xd3, 0xa5};

static err_t netif_output(struct netif *netif, struct pbuf *p) {
    LINK_STATS_INC(link.xmit);

    auto &controller = *static_cast<drivers::enc28j60::enc28j60 *>(netif->state);

    if (not controller.send_packet(static_cast<uint8_t *>(p->payload), p->tot_len)) {
        printf("Cannot sent packet....");
        return ERR_ABRT;
    }

    printf("Sent packet with len %d[%d]!  %x:%x:%x:%x:%x:%x %x:%x:%x:%x:%x:%x \r\n", p->tot_len,
           p->len, static_cast<uint8_t *>(p->payload)[0], static_cast<uint8_t *>(p->payload)[1],
           static_cast<uint8_t *>(p->payload)[2], static_cast<uint8_t *>(p->payload)[3],
           static_cast<uint8_t *>(p->payload)[4], static_cast<uint8_t *>(p->payload)[5],
           static_cast<uint8_t *>(p->payload)[6], static_cast<uint8_t *>(p->payload)[7],
           static_cast<uint8_t *>(p->payload)[8], static_cast<uint8_t *>(p->payload)[9],
           static_cast<uint8_t *>(p->payload)[10], static_cast<uint8_t *>(p->payload)[11]

    );
    return ERR_OK;
}

static void netif_status_callback(struct netif *netif) {
    printf("netif status changed %s\n", ip4addr_ntoa(netif_ip4_addr(netif)));
}

static void netif_link_callback(struct netif *netif) { printf("netif link changed\n"); }

static err_t netif_init(struct netif *netif) {
    netif->linkoutput = netif_output;
    netif->output = etharp_output;
    //    netif->output_ip6 = ethip6_output;
    netif->mtu = ETHERNET_MTU;
    netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_ETHERNET |
                   NETIF_FLAG_IGMP | NETIF_FLAG_MLD6;
    //    MIB2_INIT_NETIF(netif, snmp_ifType_ethernet_csmacd, 100000000);
    memcpy(netif->hwaddr, mac.data(), sizeof(netif->hwaddr));
    netif->hwaddr_len = sizeof(netif->hwaddr);
    printf("LWIP Init \n");
    return ERR_OK;
}

std::array<uint8_t, ETHERNET_MTU> buffer{};

int main() {
    stdio_init_all();

    EncRstPin.init();
    EncCsPin.init();
    BoardLed.init();
    spi0_.init();

    sleep_ms(3000);

    // ENC28J60 INIT
    if (not eth_driver.init(mac)) {
        hal::panic();
    }

    netif net_if;

    lwip_init();

    if (netif_add(&net_if, IP4_ADDR_ANY, IP4_ADDR_ANY, IP4_ADDR_ANY,
                  static_cast<void *>(&eth_driver), netif_init, netif_input) == nullptr) {
        printf("mch_net_init: netif_add (mchdrv_init) failed\n");
        return -1;
    }

    net_if.name[0] = 'e';
    net_if.name[1] = '0';

    netif_set_status_callback(&net_if, netif_status_callback);
    netif_set_link_callback(&net_if, netif_link_callback);

    netif_set_default(&net_if);
    netif_set_up(&net_if);

    dhcp_start(&net_if);
    httpd_init();
    pbuf *ptr = nullptr;

    while (not eth_driver.is_link_up())
        ;

    while (true) {
        if (eth_driver.link_state_changed()) {
            if (eth_driver.is_link_up()) {
                netif_set_link_up(&net_if);
                printf("**** NETIF: LINK IS UP!\r\n");
            } else {
                netif_set_link_down(&net_if);
                printf("**** NETIF: LINK IS DOWN!\r\n");
            }
        }

        if (eth_driver.get_number_of_packets() > 0) {
            auto packet_info = eth_driver.get_incoming_packet_info();
            auto bytes_received =
                eth_driver.get_incoming_packet(packet_info, buffer.data(), buffer.max_size());
            ptr = pbuf_alloc(PBUF_RAW, packet_info.byte_count, PBUF_POOL);
            if (ptr != nullptr) {
                pbuf_take(ptr, static_cast<const void *>(buffer.data()), packet_info.byte_count);

                uint8_t *bffer = static_cast<uint8_t *>(ptr->payload);
                printf("Received packet with len [%d/%d] %d!   DST: %x:%x:%x:%x:%x:%x  SRC: "
                       "%x:%x:%x:%x:%x:%x \r\n",
                       ptr->len, packet_info.byte_count, packet_info.next_packet_pointer, bffer[0],
                       bffer[1], bffer[2], bffer[3], bffer[4], bffer[5], bffer[6], bffer[7],
                       bffer[8], bffer[9], bffer[10], bffer[11]);

                LINK_STATS_INC(link.recv);

                if (net_if.input(ptr, &net_if) != ERR_OK) {
                    printf("Error processing frame input\r\n");
                    pbuf_free(ptr);
                }
            }

            buffer.fill(0);
        }

        sys_check_timeouts();
        // Blink LED
        BoardLed.set();
        sleep_ms(100);
        BoardLed.reset();
        sleep_ms(100);
    }
}
