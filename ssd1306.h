#pragma once

#include <stdint.h>

struct SSD1306 {

  void initialise();

  void turnOff() const;
  void turnOn() const;

  void nonInvert() const;
  void invert() const;
  void setContrast(uint8_t level) const;

  void clear(int8_t val = 0);
  void flush() const;

  void setPixel(int8_t x, int8_t y);

  private:

  static uint8_t m_buffer[1024];
};

