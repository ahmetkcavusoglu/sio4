#include <Arduino.h>

#include "lines.h"

#include "ssd1306.h"
#include "xorshift.h"

// -------------------------------------------------------------------------------------------------

void drawLine(SSD1306& display, int8_t ax, int8_t ay, int8_t bx, int8_t by, bool jitter) {
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

void drawNum(SSD1306& display, int8_t digit, int8_t left, int8_t top, int8_t right, int8_t bottom, bool jitter) {
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

void drawLetter(SSD1306& display, char letter, int8_t left, int8_t top, int8_t right, int8_t bottom, bool jitter) {
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

void drawColon(SSD1306& display, int8_t radius, int8_t left, int8_t top, int8_t right, int8_t bottom, bool jitter) {
  int8_t height33 = (bottom - top) / 3;
  int8_t mid = getMid(left, right);
  int8_t diameter = radius * 2;

  // Horizontal lines.
  drawLine(display, mid - radius, top + height33 - diameter,    mid + radius, top + height33 - diameter,    jitter);
  drawLine(display, mid - radius, top + height33,               mid + radius, top + height33,               jitter);
  drawLine(display, mid - radius, bottom - height33,            mid + radius, bottom - height33,            jitter);
  drawLine(display, mid - radius, bottom - height33 + diameter, mid + radius, bottom - height33 + diameter, jitter);

  // Vertical lines.
  drawLine(display, mid - radius, top + height33 - diameter, mid - radius, top + height33,       jitter);
  drawLine(display, mid + radius, top + height33 - diameter, mid + radius, top + height33,       jitter);
  drawLine(display, mid - radius, bottom - height33, mid - radius, bottom - height33 + diameter, jitter);
  drawLine(display, mid + radius, bottom - height33, mid + radius, bottom - height33 + diameter, jitter);
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
