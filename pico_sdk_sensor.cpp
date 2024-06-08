#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/spi.h"
#include "hardware/i2c.h"
#include "hardware/watchdog.h"
#include "LoRa.h"
#include "NewPing.h"

#define GPIO_ON 1
#define GPIO_OFF 0

#define TRIGGER_PIN 17
#define ECHO_PIN 16
#define MAX_DISTANCE 450

LoRaClass loraClass = LoRaClass();
NewPing sonar = NewPing(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

int main()
{
    stdio_init_all();
    printf("Started 2.\n");

    if (watchdog_caused_reboot())
    {
        printf("Rebooted by Watchdog!\n");
    }
    watchdog_enable(3000, 1);
    watchdog_update();

    sonar.begin();

    loraClass.begin(868E6);
    loraClass.setTxPower(13);
    loraClass.setSignalBandwidth(62.5E3);
    loraClass.setCodingRate4(5);
    loraClass.setSpreadingFactor(7);
    loraClass.setGain(6);
    loraClass.enableCrc();

    gpio_set_function(PICO_DEFAULT_LED_PIN, GPIO_FUNC_SIO);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

    while (true)
    {
        watchdog_update();
        printf("Hello, world 7!\n");

        gpio_put(PICO_DEFAULT_LED_PIN, GPIO_ON);
        sleep_ms(500);
        gpio_put(PICO_DEFAULT_LED_PIN, GPIO_OFF);
        uint16_t reading = sonar.ping_cm();
        printf("Reading: %u\n", reading);
        sleep_ms(500);

        loraClass.beginPacket();
        loraClass.print('1');
        loraClass.print('2');
        loraClass.print('3');
        loraClass.print('4');
        loraClass.endPacket();

        // loraClass.setFifoAddrPtr(0x0);
        // loraClass.print(1);
        // loraClass.print(2);
        // loraClass.print(3);
        // loraClass.print(4);
        // loraClass.setFifoAddrPtr(0x0);
        // printf("1: %x\n", loraClass.getRegValue(0x0));
        // printf("2: %x\n", loraClass.getRegValue(0x0));
        // printf("3: %x\n", loraClass.getRegValue(0x0));
        // printf("4: %x\n", loraClass.getRegValue(0x0));
    }
}

// static inline void cs_select()
// {
//     asm volatile("nop \n nop \n nop");
//     gpio_put(LORA_CS_PIN, 0); // Active low
//     asm volatile("nop \n nop \n nop");
// }

// static inline void cs_deselect()
// {
//     asm volatile("nop \n nop \n nop");
//     gpio_put(LORA_CS_PIN, 1);
//     asm volatile("nop \n nop \n nop");
// }

// void read_registers(uint8_t reg, uint8_t *buf, uint16_t len)
// {
//     // For this particular device, we send the device the register we want to read
//     // first, then subsequently read from the device. The register is auto incrementing
//     // so we don't need to keep sending the register we want, just the first.
//     reg |= READ_BIT;
//     cs_select();
//     spi_write_blocking(SPI_PORT, &reg, 1);
//     sleep_ms(10);
//     spi_read_blocking(SPI_PORT, 0, buf, len);
//     cs_deselect();
//     sleep_ms(10);
// }
