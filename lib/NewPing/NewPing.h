#pragma once
#define NewPing_h

#include <stdio.h>
#include "pico/stdlib.h"

// Shouldn't need to change these values unless you have a specific need to do so.
#define MAX_SENSOR_DISTANCE 500 // Maximum sensor distance can be as high as 500cm, no reason to wait for ping longer than sound takes to travel this distance and back. Default=500
#define US_ROUNDTRIP_CM 57		// Microseconds (uS) it takes sound to travel round-trip 1cm (2cm total), uses integer to save compiled code space. Default=57
#define US_ROUNDTRIP_IN 146		// Microseconds (uS) it takes sound to travel round-trip 1 inch (2 inches total), uses integer to save compiled code space. Default=146
#define ROUNDING_ENABLED false	// Set to "true" to enable distance rounding which also adds 64 bytes to binary size. Default=false
#define URM37_ENABLED false		// Set to "true" to enable support for the URM37 sensor in PWM mode. Default=false
#define TRIGGER_WIDTH 12		// Microseconds (uS) notch to trigger sensor to start ping. Sensor specs state notch should be 10uS, defaults to 12uS for out of spec sensors. Default=12

// Probably shouldn't change these values unless you really know what you're doing.
#define NO_ECHO 0				// Value returned if there's no ping echo within the specified MAX_SENSOR_DISTANCE or max_cm_distance. Default=0
#define MAX_SENSOR_DELAY 5800	// Maximum uS it takes for sensor to start the ping. Default=5800
#define ECHO_TIMER_FREQ 24		// Frequency (in microseconds) to check for a ping echo (every 24uS is about 0.4cm accuracy). Default=24
#define PING_MEDIAN_DELAY 30000 // Microsecond delay between pings in the ping_median method. Default=30000

// Conversion from uS to distance (round result to nearest cm or inch).
#define NewPingConvert(echoTime, conversionFactor) (max(((unsigned int)echoTime + conversionFactor / 2) / conversionFactor, (echoTime ? 1 : 0)))

#define PING_OVERHEAD 1
#define PING_TIMER_OVERHEAD 1
#define TIMER_ENABLED false
#define DO_BITWISE false

class NewPing
{
public:
	NewPing(uint8_t trigger_pin, uint8_t echo_pin, unsigned int max_cm_distance = MAX_SENSOR_DISTANCE);
	void begin();
	unsigned int ping(unsigned int max_cm_distance = 0);
	unsigned long ping_cm(unsigned int max_cm_distance = 0);
	unsigned long ping_in(unsigned int max_cm_distance = 0);
	unsigned long ping_median(uint8_t it = 5, unsigned int max_cm_distance = 0);
	static unsigned int convert_cm(unsigned int echoTime);
	static unsigned int convert_in(unsigned int echoTime);

protected:
	bool ping_trigger();
	void set_max_distance(unsigned int max_cm_distance);

	uint8_t _triggerPin;
	uint8_t _echoPin;

	unsigned int _maxEchoTime;
	unsigned long _max_time;

private:
	uint32_t micros();
};
