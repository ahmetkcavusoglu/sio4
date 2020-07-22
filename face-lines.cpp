#include <Arduino.h>

#include "ssd1306.h"

#include "face-lines.h"

// -------------------------------------------------------------------------------------------------

uint16_t prng = 1;
uint16_t xorShift() {
  prng ^= prng << 7;
  prng ^= prng >> 9;
  prng ^= prng << 8;
  return prng;
}

// -------------------------------------------------------------------------------------------------

template <typename T> T getMid(T a, T b) {
  return a + ((b - a) / 2);
}

// -------------------------------------------------------------------------------------------------

void drawLine(SSD1306& display, int8_t ax, int8_t ay, int8_t bx, int8_t by, bool jitter = true) {
  if (jitter) {
    uint16_t rand = xorShift();
    ax += ((rand >> 0) % 3) - 1;
    ay += ((rand >> 2) % 3) - 1;
    bx += ((rand >> 4) % 3) - 1;
    by += ((rand >> 6) % 3) - 1;
  }

  int8_t dx = abs(bx - ax);
  int8_t sx = ax < bx ? 1 : -1;

  int8_t dy = abs(by - ay);
  int8_t sy = ay < by ? 1 : -1;

  int8_t err = (dx > dy ? dx : -dy) / 2;

  // The longest line we can possibly draw on a 128x64 screen is 143.108.  To be sure we don't loop
  // forever accidentally we'll max out there.
  for (int8_t _ = 0; _ < 144; _++) {
    display.setPixel(ax, ay);

    if (ax == bx && ay == by) { break; }

    int8_t err2 = err;
    if (err2 > -dx) { err -= dy; ax += sx; }
    if (err2 <  dy) { err += dx; ay += sy; }
  }
}

// -------------------------------------------------------------------------------------------------

void drawZero(SSD1306& display, int8_t left, int8_t top, int8_t right, int8_t bottom, bool jitter) {
  drawLine(display, left,  top,    right, top,    jitter);
  drawLine(display, left,  top,    left,  bottom, jitter);
  drawLine(display, right, top,    right, bottom, jitter);
  drawLine(display, left,  bottom, right, bottom, jitter);
}

// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

void drawOne(SSD1306& display, int8_t left, int8_t top, int8_t right, int8_t bottom, bool jitter) {
  int8_t mid = getMid(left, right);
  drawLine(display, mid, top, mid, bottom, jitter);
}

// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

void drawTwo(SSD1306& display, int8_t left, int8_t top, int8_t right, int8_t bottom, bool jitter) {
  int8_t mid = getMid(top, bottom);
  drawLine(display, left,  top,    right, top,    jitter);
  drawLine(display, left,  mid,    right, mid,    jitter);
  drawLine(display, left,  bottom, right, bottom, jitter);
  drawLine(display, right, top,    right, mid,    jitter);
  drawLine(display, left,  mid,    left,  bottom, jitter);
}

// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

void drawThree(SSD1306& display, int8_t left, int8_t top, int8_t right, int8_t bottom, bool jitter) {
  int8_t mid = getMid(top, bottom);
  drawLine(display, left,  top,    right, top,    jitter);
  drawLine(display, left,  mid,    right, mid,    jitter);
  drawLine(display, left,  bottom, right, bottom, jitter);
  drawLine(display, right, top,    right, bottom, jitter);
}

// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

void drawFour(SSD1306& display, int8_t left, int8_t top, int8_t right, int8_t bottom, bool jitter) {
  int8_t mid = getMid(top, bottom);
  drawLine(display, left,  top, left,  mid,    jitter);
  drawLine(display, right, top, right, bottom, jitter);
  drawLine(display, left,  mid, right, mid,    jitter);
}

// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

void drawFive(SSD1306& display, int8_t left, int8_t top, int8_t right, int8_t bottom, bool jitter) {
  int8_t mid = getMid(top, bottom);
  drawLine(display, left,  top,    right, top,    jitter);
  drawLine(display, left,  mid,    right, mid,    jitter);
  drawLine(display, left,  bottom, right, bottom, jitter);
  drawLine(display, left,  top,    left,  mid,    jitter);
  drawLine(display, right, mid,    right, bottom, jitter);
}

// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

void drawSix(SSD1306& display, int8_t left, int8_t top, int8_t right, int8_t bottom, bool jitter) {
  int8_t mid = getMid(top, bottom);
  drawLine(display, left,  top,    right, top,    jitter);
  drawLine(display, left,  mid,    right, mid,    jitter);
  drawLine(display, left,  bottom, right, bottom, jitter);
  drawLine(display, left,  top,    left,  bottom, jitter);
  drawLine(display, right, mid,    right, bottom, jitter);
}

// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

void drawSeven(SSD1306& display, int8_t left, int8_t top, int8_t right, int8_t bottom, bool jitter) {
  drawLine(display, left,  top, right, top,    jitter);
  drawLine(display, right, top, right, bottom, jitter);
}

// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

void drawEight(SSD1306& display, int8_t left, int8_t top, int8_t right, int8_t bottom, bool jitter) {
  int8_t mid = getMid(top, bottom);
  drawLine(display, left,  top,    right, top,    jitter);
  drawLine(display, left,  mid,    right, mid,    jitter);
  drawLine(display, left,  bottom, right, bottom, jitter);
  drawLine(display, left,  top,    left,  bottom, jitter);
  drawLine(display, right, top,    right, bottom, jitter);
}

// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

void drawNine(SSD1306& display, int8_t left, int8_t top, int8_t right, int8_t bottom, bool jitter) {
  int8_t mid = getMid(top, bottom);
  drawLine(display, left,  top,    right, top,    jitter);
  drawLine(display, left,  mid,    right, mid,    jitter);
  drawLine(display, left,  bottom, right, bottom, jitter);
  drawLine(display, right, top,    right, bottom, jitter);
  drawLine(display, left,  top,    left,  mid,    jitter);
}

// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

void drawNum(SSD1306& display, int8_t digit, int8_t left, int8_t top, int8_t right, int8_t bottom, bool jitter = true) {
  switch (digit) {
    case 0: drawZero(display,  left, top, right, bottom, jitter); break;
    case 1: drawOne(display,   left, top, right, bottom, jitter); break;
    case 2: drawTwo(display,   left, top, right, bottom, jitter); break;
    case 3: drawThree(display, left, top, right, bottom, jitter); break;
    case 4: drawFour(display,  left, top, right, bottom, jitter); break;
    case 5: drawFive(display,  left, top, right, bottom, jitter); break;
    case 6: drawSix(display,   left, top, right, bottom, jitter); break;
    case 7: drawSeven(display, left, top, right, bottom, jitter); break;
    case 8: drawEight(display, left, top, right, bottom, jitter); break;
    case 9: drawNine(display,  left, top, right, bottom, jitter); break;
  }
}

// -------------------------------------------------------------------------------------------------

void drawDenied(SSD1306& display, int8_t left, int8_t top, int8_t right, int8_t bottom, bool ) {
  drawLine(display, left,  top,    right, top,    false);
  drawLine(display, left,  bottom, right, bottom, false);
  drawLine(display, left,  top,    left,  bottom, false);
  drawLine(display, right, top,    right, bottom, false);

  drawLine(display, left, top, right, bottom, false);
  drawLine(display, right, top, left, bottom, false);

}

// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

void drawA(SSD1306& display, int8_t left, int8_t top, int8_t right, int8_t bottom, bool jitter) {
  int8_t mid = getMid(top, bottom);
  drawLine(display, left,  top, right, top,    jitter);
  drawLine(display, left,  top, left,  bottom, jitter);
  drawLine(display, right, top, right, bottom, jitter);
  drawLine(display, left,  mid, right, mid,    jitter);
}

// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

void drawD(SSD1306& display, int8_t left, int8_t top, int8_t right, int8_t bottom, bool jitter) {
  int8_t midW = getMid(left, right);
  int8_t midH = getMid(top, bottom);
  drawLine(display, left,  top,    midW,  top,    jitter);
  drawLine(display, left,  top,    left,  bottom, jitter);
  drawLine(display, midW,  top,    right, midH,   jitter);
  drawLine(display, right, midH,   right, bottom, jitter);
  drawLine(display, left,  bottom, right, bottom, jitter);
}

// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

void drawE(SSD1306& display, int8_t left, int8_t top, int8_t right, int8_t bottom, bool jitter) {
  int8_t mid = getMid(top, bottom);
  drawLine(display, left, top,    left,  bottom, jitter);
  drawLine(display, left, top,    right, top,    jitter);
  drawLine(display, left, mid,    right, mid,    jitter);
  drawLine(display, left, bottom, right, bottom, jitter);
}

// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

void drawF(SSD1306& display, int8_t left, int8_t top, int8_t right, int8_t bottom, bool jitter) {
  int8_t mid = getMid(top, bottom);
  drawLine(display, left, top, left,  bottom, jitter);
  drawLine(display, left, top, right, top,    jitter);
  drawLine(display, left, mid, right, mid,    jitter);
}

// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

void drawH(SSD1306& display, int8_t left, int8_t top, int8_t right, int8_t bottom, bool jitter) {
  int8_t mid = getMid(top, bottom);
  drawLine(display, left,  top, left,  bottom, jitter);
  drawLine(display, right, top, right, bottom, jitter);
  drawLine(display, left,  mid, right, mid,    jitter);
}

// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

void drawI(SSD1306& display, int8_t left, int8_t top, int8_t right, int8_t bottom, bool jitter) {
  drawOne(display, left, top, right, bottom, jitter);
}

// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

void drawM(SSD1306& display, int8_t left, int8_t top, int8_t right, int8_t bottom, bool jitter) {
  int8_t mid = getMid(left, right);
  drawLine(display, left,  top, right, top,    jitter);
  drawLine(display, left,  top, left,  bottom, jitter);
  drawLine(display, right, top, right, bottom, jitter);
  drawLine(display, mid,   top, mid,   bottom, jitter);
}

// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

void drawN(SSD1306& display, int8_t left, int8_t top, int8_t right, int8_t bottom, bool jitter) {
  drawLine(display, left,  top, left,  bottom, jitter);
  drawLine(display, right, top, right, bottom, jitter);
  drawLine(display, left,  top, right, bottom, jitter);
}

// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

void drawO(SSD1306& display, int8_t left, int8_t top, int8_t right, int8_t bottom, bool jitter) {
  drawZero(display, left, top, right, bottom, jitter);
}

// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

void drawR(SSD1306& display, int8_t left, int8_t top, int8_t right, int8_t bottom, bool jitter) {
  int8_t mid = getMid(top, bottom);
  drawLine(display, left,  top, left,  bottom, jitter);
  drawLine(display, left,  top, right, top,    jitter);
  drawLine(display, right, top, right, mid,    jitter);
  drawLine(display, left,  mid, right, mid,    jitter);
  drawLine(display, left,  mid, right, bottom, jitter);
}

// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

void drawS(SSD1306& display, int8_t left, int8_t top, int8_t right, int8_t bottom, bool jitter) {
  drawFive(display, left, top, right, bottom, jitter);
}

// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

void drawT(SSD1306& display, int8_t left, int8_t top, int8_t right, int8_t bottom, bool jitter) {
  int8_t mid = getMid(left, right);
  drawLine(display, left, top, right, top,    jitter);
  drawLine(display, mid,  top, mid,   bottom, jitter);
}

// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

void drawU(SSD1306& display, int8_t left, int8_t top, int8_t right, int8_t bottom, bool jitter) {
  drawLine(display, left,  top,    left,  bottom, jitter);
  drawLine(display, right, top,    right, bottom, jitter);
  drawLine(display, left,  bottom, right, bottom, jitter);
}

// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

void drawW(SSD1306& display, int8_t left, int8_t top, int8_t right, int8_t bottom, bool jitter) {
  int8_t mid = getMid(left, right);
  drawLine(display, left,  top,    left,  bottom, jitter);
  drawLine(display, mid,   top,    mid,   bottom, jitter);
  drawLine(display, right, top,    right, bottom, jitter);
  drawLine(display, left,  bottom, right, bottom, jitter);
}

// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
// sun, mon, tue, wed, thu, fri, sat -- sunmotewdhfria

void drawLetter(SSD1306& display, char letter, int8_t left, int8_t top, int8_t right, int8_t bottom, bool jitter = true) {
  switch (letter) {
    case 'a': drawA(display, left, top, right, bottom, jitter);      break;
    case 'd': drawD(display, left, top, right, bottom, jitter);      break;
    case 'e': drawE(display, left, top, right, bottom, jitter);      break;
    case 'f': drawF(display, left, top, right, bottom, jitter);      break;
    case 'h': drawH(display, left, top, right, bottom, jitter);      break;
    case 'i': drawI(display, left, top, right, bottom, jitter);      break;
    case 'm': drawM(display, left, top, right, bottom, jitter);      break;
    case 'n': drawN(display, left, top, right, bottom, jitter);      break;
    case 'o': drawO(display, left, top, right, bottom, jitter);      break;
    case 'r': drawR(display, left, top, right, bottom, jitter);      break;
    case 's': drawS(display, left, top, right, bottom, jitter);      break;
    case 't': drawT(display, left, top, right, bottom, jitter);      break;
    case 'u': drawU(display, left, top, right, bottom, jitter);      break;
    case 'w': drawW(display, left, top, right, bottom, jitter);      break;

    default:  drawDenied(display, left, top, right, bottom, jitter); break;
  }
}

// -------------------------------------------------------------------------------------------------

void drawColon(SSD1306& display, int8_t radius, int8_t left, int8_t top, int8_t right, int8_t bottom) {
  int8_t height33 = (bottom - top) / 3;
  int8_t mid = getMid(left, right);
  int8_t diameter = radius * 2;

  // Horizontal lines.
  drawLine(display, mid - radius, top + height33 - diameter,    mid + radius, top + height33 - diameter);
  drawLine(display, mid - radius, top + height33,               mid + radius, top + height33);
  drawLine(display, mid - radius, bottom - height33,            mid + radius, bottom - height33);
  drawLine(display, mid - radius, bottom - height33 + diameter, mid + radius, bottom - height33 + diameter);

  // Vertical lines.
  drawLine(display, mid - radius, top + height33 - diameter, mid - radius, top + height33);
  drawLine(display, mid + radius, top + height33 - diameter, mid + radius, top + height33);
  drawLine(display, mid - radius, bottom - height33, mid - radius, bottom - height33 + diameter);
  drawLine(display, mid + radius, bottom - height33, mid + radius, bottom - height33 + diameter);
}

// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

void drawPercent(SSD1306& display, int8_t left, int8_t top, int8_t right, int8_t bottom, bool jitter) {
  int8_t dotWidth = (right - left) / 4;
  int8_t dotHeight = (bottom - top) / 4;

  drawLine(display, left, top, left + dotWidth, top + dotHeight, jitter);
  drawLine(display, left + dotWidth, top, left, top + dotHeight, jitter);

  drawLine(display, right, top, left, bottom, jitter);

  drawLine(display, right - dotWidth, bottom - dotHeight, right, bottom, jitter);
  drawLine(display, right, bottom - dotHeight, right - dotWidth, bottom, jitter);
}

// -------------------------------------------------------------------------------------------------

void drawTime(SSD1306& display,
              int8_t left, int8_t top, int8_t right, int8_t bottom,
              int8_t hour, int8_t minute) {
  // Split the space into boxes for the digits, leave a gap between each.
  // 1A:BC - 1 - 15% / A - 25% / : - 10% / B - 25% / C - 25%
  int16_t fullWidth = static_cast<int16_t>(right - left);

  constexpr uint8_t gap = 2;

  int16_t width15 = (fullWidth * 15) / 100;
  if (hour >= 10) {
    // Draw the leading 1.
    drawNum(display, 1, left, top, left + width15 - gap, bottom);
  }

  int16_t width25 = fullWidth / 4;
  drawNum(display, hour   % 10,
          left + width15 + gap, top,
          left + width15 + width25 - gap, bottom);
  drawNum(display, minute / 10,
          right - width25 - width25 + gap, top,
          right - width25 - gap, bottom);
  drawNum(display, minute % 10,
          right - width25 + gap, top,
          right - gap, bottom);

  int16_t width50 = fullWidth / 2;
  drawColon(display, 2, left + width15 + width25 + 1, top, left + width50 - 1, bottom);
}

// -------------------------------------------------------------------------------------------------

void drawAmPm(SSD1306& display, int8_t left, int8_t top, int8_t right, int8_t bottom, bool isAm) {
  int8_t mid = getMid(left, right);
  int8_t ucross = top + ((bottom - top) / 3);
  int8_t lcross = bottom - ((bottom - top) / 3);

  int8_t gap = 3;

  // The A and P both have the left and top lines.
  drawLine(display, left, top, mid - gap, top);
  drawLine(display, left, top, left,      bottom);

  if (isAm) {
    drawLine(display, mid - gap, top,    mid - gap, bottom);
    drawLine(display, left,      lcross, mid - gap, lcross);
  } else {
    drawLine(display, mid - gap, top,    mid - gap, ucross);
    drawLine(display, left,      ucross, mid - gap, ucross);
  }

  int8_t midM = getMid(mid, right);
  drawLine(display, mid,   top, mid,   bottom);
  drawLine(display, right, top, right, bottom);
  drawLine(display, mid,   top, midM,  lcross);
  drawLine(display, right, top, midM,  lcross);
}

// -------------------------------------------------------------------------------------------------

void drawPercentage(SSD1306& display, int8_t left, int8_t top, int8_t right, int8_t bottom, int8_t pc) {
  // Split the space into boxes for the digits, leave a gap between each.
  // 100% - 1 - 10% / 0 25% / % 15%

  uint16_t rand = xorShift();
  int8_t vertAdjust = ((rand >> 0) % 3) - 1;

  int8_t gap = 1;
  int8_t mid = getMid(left, right);

  int8_t width25 = (right - left) / 4;
  if (pc >= 100) {
    int8_t horizAdjust = ((rand >> 2) % 3) - 1;
    drawNum(display, 1,
            left + gap + horizAdjust,
            top + gap + vertAdjust,
            left + width25 - gap + horizAdjust,
            bottom - gap + vertAdjust,
            false);
  }

  if (pc >= 10) {
    int8_t horizAdjust = ((rand >> 4) % 3) - 1;
    drawNum(display, (pc / 10) % 10,
            mid - width25 + gap + horizAdjust,
            top + gap + vertAdjust,
            mid - gap + horizAdjust,
            bottom - gap + vertAdjust,
            false);
  }
  int8_t horizAdjust = ((rand >> 6) % 3) - 1;
  drawNum(display, pc % 10,
          mid + gap + horizAdjust,
          top + gap + vertAdjust,
          mid + width25 - gap + horizAdjust,
          bottom - gap + vertAdjust,
          false);

  horizAdjust = ((rand >> 8) % 3) - 1;
  drawPercent(display,
              right - width25 + gap + horizAdjust,
              top + gap + vertAdjust,
              right - gap + horizAdjust,
              bottom - gap + vertAdjust,
              false);
}

// -------------------------------------------------------------------------------------------------

constexpr char sunDay[3] PROGMEM = { 's', 'u', 'n' };
constexpr char monDay[3] PROGMEM = { 'm', 'o', 'n' };
constexpr char tueDay[3] PROGMEM = { 't', 'u', 'e' };
constexpr char wedDay[3] PROGMEM = { 'w', 'e', 'd' };
constexpr char thuDay[3] PROGMEM = { 't', 'h', 'u' };
constexpr char friDay[3] PROGMEM = { 'f', 'r', 'i' };
constexpr char satDay[3] PROGMEM = { 's', 'a', 't' };

constexpr char* dayNames[7] PROGMEM = {
  sunDay, monDay, tueDay, wedDay, thuDay, friDay, satDay,
};

void drawDate(SSD1306& display,
              int8_t left, int8_t top, int8_t right, int8_t bottom, int8_t month,
              int8_t day, int8_t dayOfWeek) {
  // Split the space into boxes for the digits, leave a gap between each.
  // DAY DD/MM - split into 8, with 1/2 for the space and slash.
  int8_t width8th = (right - left) / 8;
  int8_t gap = 1;

  uint16_t rand = xorShift();
  int8_t vertAdjust = ((rand >> 0) % 3) - 1;

  char* dayNameAddr = pgm_read_word(&(dayNames[dayOfWeek]));

  // Write the day name.
  int8_t posLeft = left + ((rand >> 12) % 3) - 1;
  for (int16_t letterIdx = 0; letterIdx < 3; letterIdx++) {
    char letter = pgm_read_byte(dayNameAddr + letterIdx);
    drawLetter(display, letter, posLeft + gap, top + vertAdjust, posLeft + width8th - gap, bottom + vertAdjust, false);
    posLeft += width8th + ((rand >> (letterIdx * 4 + 0)) % 3) - 1;
    vertAdjust = ((rand >> (letterIdx * 4 + 2)) % 3) - 1;
  }

  // Get a new random.
  rand = xorShift();

  // Start the date aligned from the right.
  posLeft = right - ((right - left) / 2) - (width8th / 2) + ((rand >> 0) % 3) - 1;
  if (day >= 10) {
    drawNum(display, day / 10, posLeft + gap, top + vertAdjust, posLeft + width8th - gap, bottom + vertAdjust, false);
  }
  posLeft += width8th + ((rand >> 2) % 3) - 1;
  vertAdjust = ((rand >> 4) % 3) - 1;
  drawNum(display, day % 10, posLeft + gap, top + vertAdjust, posLeft + width8th - gap, bottom + vertAdjust, false);
  posLeft += width8th + ((rand >> 6) % 3) - 1;
  vertAdjust = 0;

  // The slash.
  drawLine(display, posLeft + (width8th / 2) - gap, top + vertAdjust, posLeft + gap, bottom + vertAdjust, false);
  posLeft += (width8th / 2) + ((rand >> 8) % 3) - 1;
  vertAdjust = ((rand >> 10) % 3) - 1;

  drawNum(display, month / 10, posLeft + gap, top + vertAdjust, posLeft + width8th - gap, bottom + vertAdjust, false);
  posLeft += width8th + ((rand >> 12) % 3) - 1;
  vertAdjust = ((rand >> 14) % 3) - 1;
  drawNum(display, month % 10, posLeft + gap, top + vertAdjust, posLeft + width8th - gap, bottom + vertAdjust, false);
}

// -------------------------------------------------------------------------------------------------
// Draw the time using lines.

void printLinesFace(SSD1306& display,
                    int8_t month, int8_t day, int8_t hour, int8_t minute, int8_t dayOfWeek,
                    int16_t batteryPc) {
  bool isAm = true;
  if (hour > 12) {
    hour -= 12;
    isAm = false;
  }
  if (hour == 0) {
    hour = 12;
  }

  // We need to make sure there's room in our bounding boxes for the random adjustments.  Don't go
  // right to the edges of the display
  display.clear();
  drawTime(display, 4, 4, 96, 46, hour, minute);
  drawTime(display, 5, 5, 95, 45, hour, minute);
  drawAmPm(display, 100, 8, 124, 30, isAm);
  drawDate(display, 4, 54, 96, 62, month, day, dayOfWeek);
  drawPercentage(display, 100, 54, 124, 62, batteryPc);
  display.flush();
}

// -------------------------------------------------------------------------------------------------
