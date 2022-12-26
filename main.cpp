#include "FreeRTOS.h"
#include "task.h"

#include "enc28j60.h"
#include "enc_os_lwip_glue.h"
#include "gpio_wrapper.h"
#include "hardware/spi.h"
#include "pico/stdlib.h"
#include "spi.h"
#include "utils.h"

#if !NO_SYS
void hal::sleep_milli(const uint32_t time_ms) { vTaskDelay(pdMS_TO_TICKS(time_ms)); }
#endif

constexpr uint8_t MISO_PIN = 4;
constexpr uint8_t MOSI_PIN = 3;
constexpr uint8_t CLK_PIN = 2;
constexpr uint8_t ENC_IRQ = 15;
constexpr uint8_t LED_PIN = 25;

drivers::gpio::Gpio EncRstPin{0, GPIO_OUT};
drivers::gpio::Gpio EncCsPin{1, GPIO_OUT};
drivers::spi::Config spi0Config{spi0, CLK_PIN, MOSI_PIN, MISO_PIN, 25 * 1000000};
drivers::spi::SpiWrapper spi0_{spi0Config};

drivers::enc28j60::Config EncConfig{EncCsPin, EncRstPin, spi0_};
drivers::enc28j60::enc28j60 eth_driver{EncConfig};

drivers::gpio::Gpio BoardLed{LED_PIN, GPIO_OUT};

void main_task(void *params) {
    if (not eth_driver.init(mac)) {
        hal::panic();
    }

    while (not eth_driver.is_link_up())
        ;

    if (enc_driver_os_init() != ERR_OK) {
        hal::panic();
    }

    if (enc_driver_lwip_init(eth_driver) != ERR_OK) {
        hal::panic();
    }

    while (true) {
        BoardLed.set();
        vTaskDelay(100);
        BoardLed.reset();
        vTaskDelay(100);
    }
}

int main() {
    stdio_init_all();

    EncRstPin.init();
    EncCsPin.init();
    BoardLed.init();
    spi0_.init();

    TaskHandle_t task{};
    xTaskCreate(main_task, "TestMainThread", configMINIMAL_STACK_SIZE, nullptr,
                tskIDLE_PRIORITY + 1, &task);

#if NO_SYS && configUSE_CORE_AFFINITY && configNUM_CORES > 1
    // If NO_SYS is set, then we must bind the main task to one core (at least while the init is
    // called)
    vTaskCoreAffinitySet(task, 1);
#endif
    vTaskStartScheduler();

    return 0;
}
