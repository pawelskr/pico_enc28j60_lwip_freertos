#include "enc_os_lwip_glue.h"
#include "enc28j60.h"
#include "lwip/err.h"

#include "lwip/dhcp.h"
#include "lwip/inet.h"
#include "lwip/init.h"
#include "lwip/netif.h"
#include "lwip/stats.h"
#include "lwip/tcpip.h"
#include "lwip/timeouts.h"
#include "netif/etharp.h"

#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include <cstring>

#ifdef ENC_DEBUG_ON
#include <pico/stdio.h>
#endif

constexpr size_t ETHERNET_MTU = 1500;
static drivers::enc28j60::enc28j60 *handle = nullptr;
std::array<uint8_t, ETHERNET_MTU> ethernet_frame_buffer{};
static netif net_if{};
static SemaphoreHandle_t worker_sem{};
static TaskHandle_t enc_task_handle{};

void set_eth_driver_handle(drivers::enc28j60::enc28j60 *handle_) { handle = handle_; }

err_t enc_eth_packet_output(struct netif *netif, struct pbuf *p) {
    LINK_STATS_INC(link.xmit);
    auto &controller = *static_cast<drivers::enc28j60::enc28j60 *>(netif->state);

    /** @todo: implement p->next check and send packets untils len == p->tot_len */
    if (not controller.send_packet(static_cast<uint8_t *>(p->payload), p->len)) {
        printf("Cannot sent packet....");
        return ERR_ABRT;
    }
    //
#ifdef ENC_DEBUG_ON
    printf("Sent packet with len %d[%d]!  %x:%x:%x:%x:%x:%x %x:%x:%x:%x:%x:%x \r\n", p->tot_len,
           p->len, static_cast<uint8_t *>(p->payload)[0], static_cast<uint8_t *>(p->payload)[1],
           static_cast<uint8_t *>(p->payload)[2], static_cast<uint8_t *>(p->payload)[3],
           static_cast<uint8_t *>(p->payload)[4], static_cast<uint8_t *>(p->payload)[5],
           static_cast<uint8_t *>(p->payload)[6], static_cast<uint8_t *>(p->payload)[7],
           static_cast<uint8_t *>(p->payload)[8], static_cast<uint8_t *>(p->payload)[9],
           static_cast<uint8_t *>(p->payload)[10], static_cast<uint8_t *>(p->payload)[11]

    );
#endif
    return ERR_OK;
}

static void tcpip_init_done(void *arg) { xSemaphoreGive(static_cast<SemaphoreHandle_t>(arg)); }

static err_t enc_eth_netif_init(struct netif *netif) {
    netif->linkoutput = enc_eth_packet_output;
    netif->output = etharp_output;
    netif->mtu = ETHERNET_MTU;
    netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_ETHERNET |
                   NETIF_FLAG_IGMP | NETIF_FLAG_MLD6;
    memcpy(netif->hwaddr, mac.data(), sizeof(netif->hwaddr));
    netif->hwaddr_len = sizeof(netif->hwaddr);
    printf("LWIP Init \n");
    return ERR_OK;
}

static void netif_status_callback(struct netif *netif) {
    //    printf("netif status changed %s\n", ip4addr_ntoa(netif_ip4_addr(netif)));
}

static void netif_link_callback(struct netif *netif) {
    //    printf("netif link changed\n");
}

static void enc_worker_thread(void *param) {
    xSemaphoreTake(worker_sem, portMAX_DELAY);

    drivers::enc28j60::enc28j60 &eth_driver =
        *static_cast<drivers::enc28j60::enc28j60 *>(net_if.state);
    pbuf *ptr = nullptr;

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

        while (eth_driver.get_number_of_packets() > 0) {
            auto packet_info = eth_driver.get_incoming_packet_info();
            auto bytes_received = eth_driver.get_incoming_packet(
                packet_info, ethernet_frame_buffer.data(), ethernet_frame_buffer.max_size());
            ptr = pbuf_alloc(PBUF_RAW, packet_info.byte_count, PBUF_POOL);
            if (ptr != nullptr) {
                pbuf_take(ptr, static_cast<const void *>(ethernet_frame_buffer.data()),
                          packet_info.byte_count);

                LINK_STATS_INC(link.recv);

                if (net_if.input(ptr, &net_if) != ERR_OK) {
                    printf("Error processing frame input\r\n");
                    pbuf_free(ptr);
                }
                ptr = nullptr;
            }

            ethernet_frame_buffer.fill(0);
        }
        //        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

err_t enc_driver_os_init(drivers::enc28j60::enc28j60 &eth_driver) {
#if configUSE_CORE_AFFINITY && configNUM_CORES > 1
    TaskHandle_t task_handle = xTaskGetCurrentTaskHandle();
    UBaseType_t affinity = vTaskCoreAffinityGet(task_handle);
    // we must bind the main task to one core during init
    vTaskCoreAffinitySet(task_handle, 1 << portGET_CORE_ID());
#endif

    SemaphoreHandle_t init_sem = xSemaphoreCreateBinary();
    tcpip_init(tcpip_init_done, init_sem);
    xSemaphoreTake(init_sem, portMAX_DELAY);

    worker_sem = xSemaphoreCreateBinary();

    if (xTaskCreate(enc_worker_thread, "enc_worker", 1024, nullptr, tskIDLE_PRIORITY + 4,
                    &enc_task_handle) != pdPASS) {
        return ERR_ABRT;
    }

#if configUSE_CORE_AFFINITY && configNUM_CORES > 1
    vTaskCoreAffinitySet(enc_task_handle, 1 << 1);
#endif

#if configUSE_CORE_AFFINITY && configNUM_CORES > 1
    vTaskCoreAffinitySet(task_handle, affinity);
#endif

    return ERR_OK;
}

err_t enc_driver_lwip_init(drivers::enc28j60::enc28j60 &eth_driver) {
#if configUSE_CORE_AFFINITY && configNUM_CORES > 1
    TaskHandle_t task_handle = xTaskGetCurrentTaskHandle();
    UBaseType_t affinity = vTaskCoreAffinityGet(task_handle);
    // we must bind the main task to one core during init
    vTaskCoreAffinitySet(task_handle, 1 << portGET_CORE_ID());
#endif

    if (netif_add(&net_if, IP4_ADDR_ANY, IP4_ADDR_ANY, IP4_ADDR_ANY,
                  static_cast<void *>(&eth_driver), enc_eth_netif_init, tcpip_input) == nullptr) {
        printf("netif_add failed\n");
        return ERR_ABRT;
    }

    net_if.name[0] = 'e';
    net_if.name[1] = '0';

    netif_set_status_callback(&net_if, netif_status_callback);
    netif_set_link_callback(&net_if, netif_link_callback);
    netif_set_hostname(&net_if, "PICO");

    netif_set_default(&net_if);
    netif_set_up(&net_if);
    dhcp_start(&net_if);

#if configUSE_CORE_AFFINITY && configNUM_CORES > 1
    vTaskCoreAffinitySet(task_handle, affinity);
#endif
    xSemaphoreGive(worker_sem);

    return ERR_OK;
}
