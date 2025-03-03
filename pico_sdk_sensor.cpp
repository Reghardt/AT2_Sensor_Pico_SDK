#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/spi.h"
#include "hardware/i2c.h"
#include "hardware/watchdog.h"
#include "LoRa.h"
#include "NewPing.h"
#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;

#define GPIO_ON 1
#define GPIO_OFF 0

#define TRIGGER_PIN 17
#define ECHO_PIN 16
#define MAX_DISTANCE 450

#define LORA_CS_PIN 13
#define LORA_RST_PIN 11
#define LORA_DIO0_PIN 10

class FixedSizeVector
{
private:
    std::vector<double> data;
    size_t maxSize;
    size_t currentIndex;
    bool isFull;

public:
    FixedSizeVector(size_t size) : maxSize(size), currentIndex(0), isFull(false)
    {
        data.resize(size);
    }

    void add(double value)
    {
        data[currentIndex] = value;
        currentIndex = (currentIndex + 1) % maxSize;
        if (currentIndex == 0)
        {
            isFull = true;
        }
    }

    double average() const
    {
        size_t count = isFull ? maxSize : currentIndex;
        double sum = std::accumulate(data.begin(), data.begin() + count, 0.0);
        return count == 0 ? 0 : sum / count;
    }
};

LoRa loRa = LoRa(spi1, LORA_CS_PIN, LORA_RST_PIN, LORA_DIO0_PIN);
NewPing sonar = NewPing(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

int main()
{
    stdio_init_all();
    printf("Started\n");

    if (watchdog_caused_reboot())
    {
        printf("Rebooted by Watchdog!\n");
    }
    watchdog_enable(3000, 1);
    watchdog_update();

    sonar.begin();

    loRa.begin(868E6);
    loRa.setTxPower(13);
    loRa.setSignalBandwidth(62.5E3);
    loRa.setCodingRate4(5);
    loRa.setSpreadingFactor(7);
    loRa.setGain(6);
    loRa.enableCrc();

    gpio_set_function(PICO_DEFAULT_LED_PIN, GPIO_FUNC_SIO);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    gpio_put(PICO_DEFAULT_LED_PIN, GPIO_OFF);

    json j;
    j["tv"] = 1;

    // FixedSizeVector vec(5);

    // vec.add(sonar.ping_cm());
    // sleep_ms(200);
    // vec.add(sonar.ping_cm());
    // sleep_ms(200);
    // vec.add(sonar.ping_cm());
    // sleep_ms(200);
    // vec.add(sonar.ping_cm());
    // sleep_ms(200);
    // vec.add(sonar.ping_cm());
    // sleep_ms(200);

    while (true)
    {

        gpio_put(PICO_DEFAULT_LED_PIN, GPIO_ON);
        sleep_ms(250);
        gpio_put(PICO_DEFAULT_LED_PIN, GPIO_OFF);
        uint16_t reading = sonar.ping_cm();
        // vec.add(sonar.ping_cm());
        printf("Reading: %u\n", reading);

        sleep_ms(250);

        j["r"] = reading;

        auto mspk = json::to_msgpack(j);

        loRa.beginPacket();
        for (const uint8_t &c : mspk)
        {
            loRa.print(c);
        }
        loRa.endPacket();

        j["tv"] = (int)j["tv"] + 1;

        watchdog_update();
    }
}

// sleep_ms(200);
// gpio_put(TRIGGER_PIN, 1); // Set trigger pin high, this tells the sensor to send out a ping.
// sleep_us(TRIGGER_WIDTH);  // Wait long enough for the sensor to realize the trigger pin is high.
// gpio_put(TRIGGER_PIN, 0); // Set trigger pin back to low.

// while (gpio_get(ECHO_PIN) == false)
// {
// }
// uint32_t start = time_us_32();

// while (gpio_get(ECHO_PIN) == true)
// {
// }
// uint32_t end = time_us_32();
// printf("start: %uus\n", start);
// printf("end: %uus\n", end);
// auto diff = (uint32_t)(end - start);
// printf("diff: %uus\n", diff);
// auto seconds = (float_t)diff / (float_t)1000 / (float_t)1000;
// printf("Dist: %fcm\n", (seconds * 346) / (float_t)2);
