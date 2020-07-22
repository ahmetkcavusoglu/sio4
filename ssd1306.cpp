#include "ssd1306.h"

#include <SPI.h>

// -------------------------------------------------------------------------------------------------
// Search for the SSD1306.pdf 'Advance Information' from Solomon Systech.
// -------------------------------------------------------------------------------------------------

namespace {

  constexpr int8_t c_dataCommandPin = A3;
  constexpr int8_t c_chipSelectPin = A5;
  constexpr int8_t c_resetPin = A4;

  constexpr int8_t c_cmdDisplayOff      = 0xae;
  constexpr int8_t c_cmdDisplayOn       = 0xaf;
  constexpr int8_t c_cmdDisplayResume   = 0xa4;

  constexpr int8_t c_cmdNormalDisplay   = 0xa6;
  constexpr int8_t c_cmdInverseDisplay  = 0xa7;
  constexpr int8_t c_cmdSetContrast     = 0x81;

  constexpr int8_t c_cmdSetChargePump   = 0x8d;
  constexpr int8_t c_chargePumpDisable  = 0x10;
  constexpr int8_t c_chargePumpEnable   = 0x14;

  constexpr int8_t c_cmdSetDivideRatio  = 0xd5;
  constexpr int8_t c_cmdSetPreCharge    = 0xd9;
  constexpr int8_t c_cmdSetCompPins     = 0xda;
  constexpr int8_t c_cmdSetVComDeselect = 0xdb;

  constexpr int8_t c_cmdMemoryAddrMode  = 0x20;
  constexpr int8_t c_horizMode          = 0x00;
  constexpr int8_t c_vertMode           = 0x01;
  constexpr int8_t c_pageMode           = 0x02;

  constexpr int8_t c_cmdSegRemap        = 0xa0;
  constexpr int8_t c_cmdComScan         = 0xc0;
  constexpr int8_t c_scanInc            = 0x00;
  constexpr int8_t c_scanDec            = 0x08;

  // -----------------------------------------------------------------------------------------------

  enum SpiCommandOrData {
    SpiCommand, SpiData,
  };

  void sendSpi(SpiCommandOrData cmdOrData, int8_t* bytes, size_t len) {
    // Command == DC pin LOW, data == DC pin HIGH.
    digitalWrite(c_dataCommandPin, cmdOrData == SpiCommand ? LOW : HIGH);

    // 20MHz, MSB first, clock phase and polarity choice.
    SPI.beginTransaction(SPISettings(20000000, MSBFIRST, SPI_MODE0));

    // Choose our slave and transfer the command(s) or data.
    digitalWrite(c_chipSelectPin, LOW);
    if (len == 1) {
      SPI.transfer(bytes[0]);
    } else {
      SPI.transfer(bytes, len);
    }

    // Deselect the slave and finish.
    digitalWrite(c_chipSelectPin, HIGH);
    SPI.endTransaction();
  }

  void sendSpi(int8_t byte) {
    sendSpi(SpiCommand, &byte, 1);
  }

  void sendSpi(int8_t byte0, int8_t byte1) {
    int8_t bytes[2] = { byte0, byte1 };
    sendSpi(SpiCommand, bytes, 2);
  }
}

// -------------------------------------------------------------------------------------------------
// The buffered backing for our pixel data.

uint8_t SSD1306::m_buffer[1024];

// -------------------------------------------------------------------------------------------------

void SSD1306::initialise() {
  // Out mode for all the pins.
  pinMode(c_dataCommandPin, OUTPUT);
  pinMode(c_chipSelectPin, OUTPUT);
  pinMode(c_resetPin, OUTPUT);

  // Toggle a reset.
  digitalWrite(c_resetPin, LOW);
  delay(1);
  digitalWrite(c_resetPin, HIGH);
  delay(1);

  // Initialise SPI.
  SPI.begin();

  turnOff();                                       // Disable display.

  sendSpi(c_cmdSetChargePump, c_chargePumpEnable); // Enable internal voltage charge pump.
  sendSpi(c_cmdSetDivideRatio, 0x80);              // Defaults to 1 but 0x80 seems to be the go.
  sendSpi(c_cmdSetPreCharge, 0xf1);                // Pre-charge period, phase 1 == 1, phase 2 == 15.
  sendSpi(c_cmdSetCompPins, 0x12);                 // Alternative COM pins, disable left/right remap.
  sendSpi(c_cmdSetVComDeselect, 0x40);             // Vcomh regulator output. > 0.83 x Vcc?

  sendSpi(c_cmdMemoryAddrMode, c_horizMode);       // Horizontal addressing.
  sendSpi(c_cmdSegRemap | 1);                      // Reverse segments, column 127 at SEG0.
  sendSpi(c_cmdComScan | c_scanDec);               // Reverse scan direction, column 127 to 0.

  setContrast(0xffu);                              // Full brightness.

  sendSpi(c_cmdDisplayResume);                     // Map from the internal buffer.
  turnOn();                                        // Enable display.
}

// -------------------------------------------------------------------------------------------------
// Turn display off (sleep mode) or back on.

void SSD1306::turnOff() const {
  sendSpi(c_cmdDisplayOff);
}

void SSD1306::turnOn() const {
  sendSpi(c_cmdDisplayOn);
}

// -------------------------------------------------------------------------------------------------
// Invert (black on white) or restore the display mode, and set the contrast level.

void SSD1306::nonInvert() const {
  sendSpi(c_cmdNormalDisplay);
}

void SSD1306::invert() const {
  sendSpi(c_cmdInverseDisplay);
}

void SSD1306::setContrast(uint8_t level) const {
  sendSpi(c_cmdSetContrast, level);
}

// -------------------------------------------------------------------------------------------------

void SSD1306::clear(int8_t val /*= 0*/) {
  memset(m_buffer, val, 1024);
}

void SSD1306::flush() const {
  sendSpi(SpiData, m_buffer, 1024);
}

// -------------------------------------------------------------------------------------------------
// Set a pixel in the backing buffer.  Must be 0 < x < 128 and 0 < y < 64.

void SSD1306::setPixel(int8_t x, int8_t y) {
  if (x < 0 || x > 127 || y < 0 || y > 63) {
    return;
  }

  // The pixels are stacked column wise in the display RAM which we mimic here.  The first byte in
  // the buffer represents the 8 pixels at x offset 0, and y offset 0 to 7, LSB first.  The second
  // 8-bit byte represents the 8 pixels at x offset 1, and y offset 0 to 7.
  int16_t offs = ((y / 8) * 128) + x;
  m_buffer[offs] |= (1 << (y % 8));
}

// -------------------------------------------------------------------------------------------------
