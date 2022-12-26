#pragma once
#include "enc28j60.h"
#include "lwip/err.h"

/** @todo: implement enc.get_mac_address() function... */
constexpr drivers::enc28j60::enc28j60::MacAddress mac{0x0a, 0xbd, 0x7d, 0x95, 0xd3, 0xa5};

err_t enc_driver_lwip_init(drivers::enc28j60::enc28j60 &eth_driver);
err_t enc_driver_os_init();
