#include "Adafruit_SH110X.h"
#include "splash.h"

// CONSTRUCTORS, DESTRUCTOR ------------------------------------------------

/*!
    @brief  Constructor for I2C-interfaced SH110X displays.
    @param  w
            Display width in pixels
    @param  h
            Display height in pixels
    @param  twi
            Pointer to an existing TwoWire instance (e.g. &Wire, the
            microcontroller's primary I2C bus).
    @param  rst_pin
            Reset pin (using Arduino pin numbering), or -1 if not used
            (some displays might be wired to share the microcontroller's
            reset pin).
    @param  clkDuring
            Speed (in Hz) for Wire transmissions in SH110X library calls.
            Defaults to 400000 (400 KHz), a known 'safe' value for most
            microcontrollers, and meets the SH110X datasheet spec.
            Some systems can operate I2C faster (800 KHz for ESP32, 1 MHz
            for many other 32-bit MCUs), and some (perhaps not all)
            SH110X's can work with this -- so it's optionally be specified
            here and is not a default behavior. (Ignored if using pre-1.5.7
            Arduino software, which operates I2C at a fixed 100 KHz.)
    @param  clkAfter
            Speed (in Hz) for Wire transmissions following SH110X library
            calls. Defaults to 100000 (100 KHz), the default Arduino Wire
            speed. This is done rather than leaving it at the 'during' speed
            because other devices on the I2C bus might not be compatible
            with the faster rate. (Ignored if using pre-1.5.7 Arduino
            software, which operates I2C at a fixed 100 KHz.)
    @note   Call the object's begin() function before use -- buffer
            allocation is performed there!
*/
Adafruit_SH110X::Adafruit_SH110X(uint16_t w, uint16_t h, TwoWire *twi,
                                 int8_t rst_pin, uint32_t clkDuring,
                                 uint32_t clkAfter)
    : Adafruit_GrayOLED(1, w, h, twi, rst_pin, clkDuring, clkAfter) {}


/*!
    @brief  Destructor for Adafruit_SH110X object.
*/
Adafruit_SH110X::~Adafruit_SH110X(void) {}

// REFRESH DISPLAY ---------------------------------------------------------

/*!
    @brief  Push data currently in RAM to SH110X display.
    @note   Drawing operations are not visible until this function is
            called. Call after each graphics command, or after a whole set
            of graphics commands, as best needed by one's own application.
*/
void Adafruit_SH110X::display(void) {
  // ESP8266 needs a periodic yield() call to avoid watchdog reset.
  // With the limited size of SH110X displays, and the fast bitrate
  // being used (1 MHz or more), I think one yield() immediately before
  // a screen write and one immediately after should cover it.  But if
  // not, if this becomes a problem, yields() might be added in the
  // 32-byte transfer condition below.
  yield();

  // uint16_t count = WIDTH * ((HEIGHT + 7) / 8);
  uint8_t *ptr = buffer;
  uint8_t dc_byte = 0x40;
  uint8_t pages = ((HEIGHT + 7) / 8);

  uint8_t bytes_per_page = WIDTH;

  /*
  Serial.print("Window: (");
  Serial.print(window_x1);
  Serial.print(", ");
  Serial.print(window_y1);
  Serial.print(" -> (");
  Serial.print(window_x2);
  Serial.print(", ");
  Serial.print(window_y2);
  Serial.println(")");
  */

  uint8_t first_page = window_y1 / 8;
  //  uint8_t last_page = (window_y2 + 7) / 8;
  uint8_t page_start = min(bytes_per_page, (uint8_t)window_x1);
  uint8_t page_end = (uint8_t)max((int)0, (int)window_x2);
  /*
  Serial.print("Pages: ");
  Serial.print(first_page);
  Serial.print(" -> ");
  Serial.println(last_page);
  pages = min(pages, last_page);

  Serial.print("Page addr: ");
  Serial.print(page_start);
  Serial.print(" -> ");
  Serial.println(page_end);
  */

  for (uint8_t p = first_page; p < pages; p++) {
    uint8_t bytes_remaining = bytes_per_page;
    ptr = buffer + (uint16_t)p * (uint16_t)bytes_per_page;
    // fast forward to dirty rectangle beginning
    ptr += page_start;
    bytes_remaining -= page_start;
    // cut off end of dirty rectangle
    bytes_remaining -= (WIDTH - 1) - page_end;

    if (i2c_dev) { // I2C
      uint16_t maxbuff = i2c_dev->maxBufferSize() - 1;

      uint8_t cmd[] = {
          0x00, (uint8_t)(SH110X_SETPAGEADDR + p),
          (uint8_t)(0x10 + ((page_start + _page_start_offset) >> 4)),
          (uint8_t)((page_start + _page_start_offset) & 0xF)};

      // Set high speed clk
      i2c_dev->setSpeed(i2c_preclk);

      i2c_dev->write(cmd, 4);

      while (bytes_remaining) {
        uint8_t to_write = min(bytes_remaining, (uint8_t)maxbuff);
        i2c_dev->write(ptr, to_write, true, &dc_byte, 1);
        ptr += to_write;
        bytes_remaining -= to_write;
        yield();
      }

      // Set low speed clk
      i2c_dev->setSpeed(i2c_postclk);

    } 
  }
  // reset dirty window
  window_x1 = 1024;
  window_y1 = 1024;
  window_x2 = -1;
  window_y2 = -1;
}
