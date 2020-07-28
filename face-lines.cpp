#include <Arduino.h>

#include "face-lines.h"

#include "ssd1306.h"
#include "lines.h"
#include "xorshift.h"

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
    drawNum(display, 1, left, top, left + width15 - gap, bottom, true);
  }

  int16_t width25 = fullWidth / 4;
  drawNum(display, hour   % 10,
          left + width15 + gap, top,
          left + width15 + width25 - gap, bottom, true);
  drawNum(display, minute / 10,
          right - width25 - width25 + gap, top,
          right - width25 - gap, bottom, true);
  drawNum(display, minute % 10,
          right - width25 + gap, top,
          right - gap, bottom, true);

  int16_t width50 = fullWidth / 2;
  drawColon(display, 2, left + width15 + width25 + 1, top, left + width50 - 1, bottom, true);
}

// -------------------------------------------------------------------------------------------------

void drawAmPm(SSD1306& display, int8_t left, int8_t top, int8_t right, int8_t bottom, bool isAm) {
  int8_t mid = getMid(left, right);
  int8_t ucross = top + ((bottom - top) / 3);
  int8_t lcross = bottom - ((bottom - top) / 3);

  int8_t gap = 3;

  // The A and P both have the left and top lines.
  drawLine(display, left, top, mid - gap, top,    true);
  drawLine(display, left, top, left,      bottom, true);

  if (isAm) {
    drawLine(display, mid - gap, top,    mid - gap, bottom, true);
    drawLine(display, left,      lcross, mid - gap, lcross, true);
  } else {
    drawLine(display, mid - gap, top,    mid - gap, ucross, true);
    drawLine(display, left,      ucross, mid - gap, ucross, true);
  }

  int8_t midM = getMid(mid, right);
  drawLine(display, mid,   top, mid,   bottom, true);
  drawLine(display, right, top, right, bottom, true);
  drawLine(display, mid,   top, midM,  lcross, true);
  drawLine(display, right, top, midM,  lcross, true);
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
