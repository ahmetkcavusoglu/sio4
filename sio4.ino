// =================================================================================================

#include <Wire.h>

#include <avr/power.h>
#include <avr/sleep.h>

#include <SSD1306Ascii.h>
#include <SSD1306AsciiSpi.h>
#include <uRTCLib.h>
#include <YetAnotherPcInt.h>

#define FONT_TO_USE font5x7
#include "font5x7.h"

// -------------------------------------------------------------------------------------------------

// XXX don't forget about PROGMEM either, for static buffers...

constexpr int8_t c_upperLeftButtonPin = 8;
constexpr int8_t c_upperRightButtonPin = 11;
constexpr int8_t c_lowerRightButtonPin = 10;

constexpr int8_t c_leftLedPin = 13;
constexpr int8_t c_rightLedPin = 6;

constexpr int8_t c_batteryReadEnablePin = 4;
constexpr int8_t c_batteryPin = A11;

constexpr int8_t c_chargingPin = 5;

constexpr int8_t c_oledDataCommandPin = A3;
constexpr int8_t c_oledChipSelectPin = A5;
constexpr int8_t c_oledResetPin = A4;

constexpr int32_t c_defaultShowtimeTimeoutMs = 8000;

// -------------------------------------------------------------------------------------------------
// Button state and interrupt handlers.

volatile bool upperLeftButtonPressed = false;
volatile bool upperRightButtonPressed = false;
volatile bool lowerRightButtonPressed = false;

volatile int8_t interruptCount = 0;

void buttonLrbIsr(bool pinState) {
  // We take the inverse reading as they're pulled high by default.
  lowerRightButtonPressed = !pinState;
  interruptCount++;
}

// -------------------------------------------------------------------------------------------------
// Global instances.

SSD1306AsciiSpi g_oled;
uRTCLib g_rtc(URTCLIB_ADDRESS);   // I2C address.

// -------------------------------------------------------------------------------------------------

void setup() {
  Wire.begin();

  // Pull the button pins high.
  pinMode(c_upperLeftButtonPin, INPUT_PULLUP);
  pinMode(c_upperRightButtonPin, INPUT_PULLUP);
  pinMode(c_lowerRightButtonPin, INPUT_PULLUP);

  // Set the LED pins for output.
  pinMode(c_leftLedPin, OUTPUT);
  pinMode(c_rightLedPin, OUTPUT);

  // Set the battery read enable pin for output and the charging pin for input.
  pinMode(c_batteryReadEnablePin, OUTPUT);
  pinMode(c_chargingPin, INPUT_PULLUP);

  // Enable USB VBUS pad so we can read the power state from the USB status register.
  USBCON |= bit(OTGPADE);

  // Install an ISR for LRB.
  PcInt::attachInterrupt(c_lowerRightButtonPin, buttonLrbIsr, CHANGE);

  // Init the RTC.
  g_rtc.set_model(URTCLIB_MODEL_DS3231);

  // Init the display.
  g_oled.begin(&Adafruit128x64, c_oledChipSelectPin, c_oledDataCommandPin, c_oledResetPin);
  g_oled.setFont(FONT_TO_USE);
  g_oled.clear();

  // Read the current date and time from the serial port just once.
  if (getUsbAttached()) {
    readDateTimeFromSerial();
  }
}

// -------------------------------------------------------------------------------------------------

void powerDown() {
  // Power down everything.
  g_oled.ssd1306WriteCmd(SSD1306_DISPLAYOFF);
  power_adc_disable();
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();

  // Sleep.
  sleep_cpu();

  // ... wake.

  // Power up.
  sleep_disable();
  power_adc_enable();
  g_oled.ssd1306WriteCmd(SSD1306_DISPLAYON);
}

// -------------------------------------------------------------------------------------------------

float getRawBattery() {
  digitalWrite(c_batteryReadEnablePin, HIGH);
  delay(50);
  float value = analogRead(c_batteryPin);
  digitalWrite(c_batteryReadEnablePin, LOW);
  return value;
}

bool getUsbAttached() {
  // We can test if a USB data connection is up; UDADDR is the USB address register, and the ADDEN
  // bit is whether the address is enabled.
  return (UDADDR & bit(ADDEN)) != 0;
}

bool getUsbPowered() {
  // We can test if we're seeing power from USB; USBSTA is a USB status register and the VBUS bit
  // tells us there's power.
  return (USBSTA & bit(VBUS)) != 0;
}

bool getCharging() {
  // The charging status pin is low while charging, high once fully charged.
  return getUsbPowered() && digitalRead(c_chargingPin) == LOW;
}

// -------------------------------------------------------------------------------------------------
// Read the date and time values from the serial port.  Useful for booting after reset.  Will prompt
// for the correct values (year, month, day, etc.) one after the other.

bool readDateTimeFromSerial() {
  // Display an info message on the display.
  g_oled.setCursor(0, 3);
  g_oled.print("Waiting for time to");
  g_oled.setCursor(0, 4);
  g_oled.print("be set from serial...");

  Serial.begin(9600);
  Serial.setTimeout(60 * 1000);

  // Since this is happening at boot time we need to allow for a connection to be made before we
  // start prompting.
  for (int countdown = 0; countdown < 10; countdown++) {
    Serial.print("Prompting for date in... "); Serial.print(10 - countdown); Serial.println('s');
    delay(1000);
  }

  auto readValueWithPrompt = [](const char* prompt) -> uint8_t {
    Serial.print(prompt);
    uint8_t value = Serial.parseInt();
    Serial.print("Got: "); Serial.println(value);
    return value;
  };

  // If year is a zero then we probably had a timeout (it's not Y2K) and so we can just abort.
  uint8_t year = readValueWithPrompt("Year? (00-99, 0 to skip) ");
  if (year != 0) {
    uint8_t month = readValueWithPrompt("Month? (1-12) ");
    uint8_t day = readValueWithPrompt("Day? (1-31) ");
    uint8_t dayOfWeek = readValueWithPrompt("Weekday? (S 0, M 1, T 2, W 3, T 4, F 5, S 6) ");
    uint8_t hour = readValueWithPrompt("Hour? (0-23) ");
    uint8_t minute = readValueWithPrompt("Minute? (0-59) ");

    g_rtc.set(0, minute, hour, dayOfWeek, day, month, year);
  }

  Serial.end();
  g_oled.clear();
}

// -------------------------------------------------------------------------------------------------
// This face looks great with the cp437 font, but it's 8x8 and just too wide (128 / 8 == 16 chars
// per row, not enough for the ~20 we need.)
// {{{ DOS FACE

#if 0
void printDOSFace(int8_t month, int8_t day, int8_t hour, int8_t minute, int16_t battery, int16_t temp) {
  auto printTimeFormat = [](int8_t first, int8_t second) {
    g_oled.print(first / 10);
    g_oled.print(first % 10);
    g_oled.print(':');
    g_oled.print(second / 10);
    g_oled.print(second % 10);
  };

  // Use 12 hour time.
  if (hour > 12) {
    hour -= 12;
  }

  // First line: Directory of C:/
  g_oled.setCursor(0, 0);
  g_oled.print("Directory of C:\\");

  // Second line: autoexec.bat with time.
  g_oled.setCursor(0, 2);
  g_oled.print("AUTOEXEC  BAT   ");
  printTimeFormat(hour, minute);

  // Third line: config.sys with date.
  g_oled.setCursor(0, 3);
  g_oled.print("CONFIG    SYS   ");
  printTimeFormat(month, day);

  // Fourth line: games directory.
  g_oled.setCursor(0, 4);
  g_oled.print("GAMES    <DIR>  ");

  // Fifth line: file and bytes counts with temperature.
  g_oled.setCursor(0, 5);
  g_oled.print(" 3 file(s) ");
  g_oled.print(temp);
  g_oled.print(" bytes");
  g_oled.clearToEOL();

  // Sixth line: bytes free with battery.
  g_oled.setCursor(0, 6);
  g_oled.print(' ');
  g_oled.print(battery);
  g_oled.print(" bytes free");

  // Seventh line: command prompt.
  g_oled.setCursor(0, 7);
  g_oled.print("C:\\>_");
}
#endif
// }}}

// -------------------------------------------------------------------------------------------------
// A `top` alternative:
//
// 12:34 up 6:51  1 user
// load avg: 0.5,2.5,0.3
// mem: 123KB swap: 40KB
// PID USER %CPU COMMAND
// 1   root 0.0  init
// 16  root 0.1  sshd
// 23  root 0.0  etc...
// 45  toby  99  somet

// -------------------------------------------------------------------------------------------------
// This face mimics the output of `ps -ax`.

void printPsFace(int8_t month, int8_t day, int8_t hour, int8_t minute, int16_t battery, int16_t temp) {
  // Use 12 hour time.
  //if (hour > 12) {
  //  hour -= 12;
  //}

  g_oled.setCursor(0, 0);
  g_oled.print("$ ps -ax");

  g_oled.setCursor(0, 1);
  g_oled.print("PID TTY TIME  CMD");

  g_oled.setCursor(0, 2);
  g_oled.print("  0  -  ");
  g_oled.print(hour / 10); g_oled.print(hour % 10);
  g_oled.print(':');
  g_oled.print(minute / 10); g_oled.print(minute % 10);
  g_oled.print(" kernel");

  g_oled.setCursor(0, 3);
  g_oled.print("  1  -  ");
  g_oled.print(month / 10); g_oled.print(month % 10);
  g_oled.print(':');
  g_oled.print(day / 10); g_oled.print(day % 10);
  g_oled.print(" init");

  g_oled.setCursor(0, 4);
  g_oled.print(" 12  -  89:47 [idle]");

  g_oled.setCursor(0, 5);
  g_oled.print(" 54  -  02:17 sshd");

  g_oled.setCursor(0, 6);
  g_oled.print(battery);
  g_oled.print("  0  00:42 bash");

  g_oled.setCursor(0, 7);
  g_oled.print("722  0  ");
  g_oled.print(temp / 1000);
  g_oled.print((temp / 100) % 10);
  g_oled.print(':');
  g_oled.print((temp / 10) % 10);
  g_oled.print(temp % 10);
  g_oled.print(" ps");
  g_oled.clearToEOL();
}

// -------------------------------------------------------------------------------------------------

void loop() {
  // Show the time, battery and temp for 4 seconds.
  g_rtc.refresh();
  printPsFace(g_rtc.month(), g_rtc.day(), g_rtc.hour(), g_rtc.minute(),
              static_cast<int16_t>(getRawBattery()), g_rtc.temp());
  delay(c_defaultShowtimeTimeoutMs);

  // Power down
  powerDown();
}

// =================================================================================================
// vim:ft=cpp
