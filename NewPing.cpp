#include "NewPing.h"

NewPing::NewPing(uint8_t trigger_pin, uint8_t echo_pin, unsigned int max_cm_distance)
{
	_triggerPin = trigger_pin;
	_echoPin = echo_pin;

	_one_pin_mode = (trigger_pin == echo_pin); // Automatic one pin mode detection per sensor.

	set_max_distance(max_cm_distance); // Call function to set the max sensor distance.

	pinMode(echo_pin, INPUT);	  // Set echo pin to input (on Teensy 3.x (ARM), pins default to disabled, at least one pinMode() is needed for GPIO mode).
	pinMode(trigger_pin, OUTPUT); // Set trigger pin to output (on Teensy 3.x (ARM), pins default to disabled, at least one pinMode() is needed for GPIO mode).

	digitalWrite(_triggerPin, LOW); // Trigger pin should already be low, but set to low to make sure.
}

// ---------------------------------------------------------------------------
// Standard ping methods
// ---------------------------------------------------------------------------

unsigned int NewPing::ping(unsigned int max_cm_distance)
{
	if (max_cm_distance > 0)
		set_max_distance(max_cm_distance); // Call function to set a new max sensor distance.

	if (!ping_trigger())
		return NO_ECHO; // Trigger a ping, if it returns false, return NO_ECHO to the calling function.

	while (digitalRead(_echoPin)) // Wait for the ping echo.
	{
		if (micros() > _max_time)
		{
			return NO_ECHO; // Stop the loop and return NO_ECHO (false) if we're beyond the set maximum distance.
		}
	}
	return (micros() - (_max_time - _maxEchoTime) - PING_OVERHEAD); // Calculate ping time, include overhead.
}

unsigned long NewPing::ping_cm(unsigned int max_cm_distance)
{
	unsigned long echoTime = NewPing::ping(max_cm_distance); // Calls the ping method and returns with the ping echo distance in uS.
	return (echoTime / US_ROUNDTRIP_CM);					 // Call the ping method and returns the distance in centimeters (no rounding).
}

unsigned long NewPing::ping_in(unsigned int max_cm_distance)
{
	unsigned long echoTime = NewPing::ping(max_cm_distance); // Calls the ping method and returns with the ping echo distance in uS.
	return (echoTime / US_ROUNDTRIP_IN);					 // Call the ping method and returns the distance in inches (no rounding).
}

unsigned long NewPing::ping_median(uint8_t it, unsigned int max_cm_distance)
{
	unsigned int uS[it], last;
	uint8_t j, i = 0;
	unsigned long t;
	uS[0] = NO_ECHO;

	if (max_cm_distance > 0)
		set_max_distance(max_cm_distance); // Call function to set a new max sensor distance.

	while (i < it)
	{
		t = micros();  // Start ping timestamp.
		last = ping(); // Send ping.

		if (last != NO_ECHO)
		{ // Ping in range, include as part of median.
			if (i > 0)
			{												// Don't start sort till second ping.
				for (j = i; j > 0 && uS[j - 1] < last; j--) // Insertion sort loop.
					uS[j] = uS[j - 1];						// Shift ping array to correct position for sort insertion.
			}
			else
				j = 0;	  // First ping is sort starting point.
			uS[j] = last; // Add last ping to array in sorted position.
			i++;		  // Move to next ping.
		}
		else
			it--; // Ping out of range, skip and don't include as part of median.

		if (i < it && micros() - t < PING_MEDIAN_DELAY)
			delay((PING_MEDIAN_DELAY + t - micros()) >> 10); // Millisecond delay between pings.
	}
	return (uS[it >> 1]); // Return the ping distance median.
}

// ---------------------------------------------------------------------------
// Standard and timer interrupt ping method support functions (not called directly)
// ---------------------------------------------------------------------------

bool NewPing::ping_trigger()
{
	if (_one_pin_mode)
		pinMode(_triggerPin, OUTPUT); // Set trigger pin to output.

	digitalWrite(_triggerPin, HIGH);  // Set trigger pin high, this tells the sensor to send out a ping.
	delayMicroseconds(TRIGGER_WIDTH); // Wait long enough for the sensor to realize the trigger pin is high.
	digitalWrite(_triggerPin, LOW);	  // Set trigger pin back to low.

	if (_one_pin_mode)
		pinMode(_triggerPin, INPUT); // Set trigger pin to input (this is technically setting the echo pin to input as both are tied to the same pin).

	if (digitalRead(_echoPin))
		return false;										// Previous ping hasn't finished, abort.
	_max_time = micros() + _maxEchoTime + MAX_SENSOR_DELAY; // Maximum time we'll wait for ping to start (most sensors are <450uS, the SRF06 can take up to 34,300uS!)
	while (!digitalRead(_echoPin))							// Wait for ping to start.
		if (micros() > _max_time)
			return false; // Took too long to start, abort.

	_max_time = micros() + _maxEchoTime; // Ping started, set the time-out.
	return true;						 // Ping started successfully.
}

void NewPing::set_max_distance(unsigned int max_cm_distance)
{
	_maxEchoTime = min(max_cm_distance + 1, (unsigned int)MAX_SENSOR_DISTANCE + 1) * US_ROUNDTRIP_CM; // Calculate the maximum distance in uS (no rounding).
}

// ---------------------------------------------------------------------------
// Conversion methods (rounds result to nearest cm or inch).
// ---------------------------------------------------------------------------

unsigned int NewPing::convert_cm(unsigned int echoTime)
{
	return (echoTime / US_ROUNDTRIP_CM); // Convert uS to centimeters (no rounding).
}

unsigned int NewPing::convert_in(unsigned int echoTime)
{
	return (echoTime / US_ROUNDTRIP_IN); // Convert uS to inches (no rounding).
}
