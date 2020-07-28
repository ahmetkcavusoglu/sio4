#pragma once

void drawLine(struct SSD1306& display, int8_t ax, int8_t ay, int8_t bx, int8_t by, bool jitter);
void drawNum(struct SSD1306& display, int8_t digit, int8_t left, int8_t top, int8_t right, int8_t bottom, bool jitter);
void drawLetter(struct SSD1306& display, char letter, int8_t left, int8_t top, int8_t right, int8_t bottom, bool jitter);
void drawColon(struct SSD1306& display, int8_t radius, int8_t left, int8_t top, int8_t right, int8_t bottom, bool jitter);
void drawPercent(struct SSD1306& display, int8_t left, int8_t top, int8_t right, int8_t bottom, bool jitter);

template <typename T> T getMid(T a, T b) {
  return a + ((b - a) / 2);
}

