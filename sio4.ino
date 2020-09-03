// =================================================================================================

#include <Wire.h>

#include <avr/power.h>
#include <avr/sleep.h>

#include <uRTCLib.h>
#include <YetAnotherPcInt.h>

#include "ssd1306.h"
#include "face-lines.h"
#include "lines.h"

// -------------------------------------------------------------------------------------------------

constexpr int8_t c_upperLeftButtonPin = 8;
constexpr int8_t c_upperRightButtonPin = 11;
constexpr int8_t c_lowerRightButtonPin = 10;

constexpr int8_t c_leftLedPin = 13;
constexpr int8_t c_rightLedPin = 6;

constexpr int8_t c_batteryReadEnablePin = 4;
constexpr int8_t c_batteryPin = A11;

constexpr int8_t c_chargingPin = 5;

constexpr int32_t c_defaultShowtimeTimeoutMs = 4000;
constexpr int32_t c_linesFaceAnimDelay = 100;

// -------------------------------------------------------------------------------------------------
// Button state and interrupt handlers.

volatile bool upperLeftButtonPressed = false;
volatile bool upperRightButtonPressed = false;
volatile bool lowerRightButtonPressed = false;

void buttonLrbIsr(bool pinState) {
  // We take the inverse reading as they're pulled high by default.
  lowerRightButtonPressed = !pinState;
}

// -------------------------------------------------------------------------------------------------
// Global instances.

SSD1306  g_display;
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
  g_display.initialise();
  g_display.clear();

  // Read the current date and time from the serial port just once.
  if (getUsbAttached()) {
    readDateTimeFromSerial();
  }
}

// -------------------------------------------------------------------------------------------------

void powerDown() {
  // Power down everything.
  g_display.turnOff();
  power_adc_disable();
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();

  // Sleep.
  sleep_cpu();

  // ... wake.

  // Power up.
  sleep_disable();
  power_adc_enable();
  g_display.turnOn();
}

// -------------------------------------------------------------------------------------------------

float getRawBattery() {
  digitalWrite(c_batteryReadEnablePin, HIGH);
  delay(50);
  float value = analogRead(c_batteryPin);
  digitalWrite(c_batteryReadEnablePin, LOW);
  return value;
}

int16_t getBatteryPc() {
  int16_t rawLevel = static_cast<int16_t>(getRawBattery());
  return max(0, min(100, rawLevel - 534));
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
  Serial.begin(9600);
  Serial.setTimeout(60 * 1000);

  // Since this is happening at boot time we need to allow for a connection to be made before we
  // start prompting.
  for (int countdown = 10; countdown > 0; countdown--) {
    Serial.print("Prompting for date in... "); Serial.print(countdown); Serial.println('s');
    g_display.clear();
    if (countdown >= 10) {
      drawNum(g_display, countdown / 10, 34, 20, 62, 48, true);
      drawNum(g_display, countdown % 10, 66, 20, 94, 48, true);
    } else {
      drawNum(g_display, countdown, 50, 20, 78, 48, true);
    }
    g_display.flush();
    delay(1000);
  }

  // Double check that the USB connection is still there.  This way we can abort setting the time by
  // unplugging the watch just after programming it.
  if (getUsbAttached()) {
    g_display.clear();
    drawLetter(g_display, 'r',  4, 50, 12, 60, false);
    drawLetter(g_display, 'e', 14, 50, 22, 60, false);
    drawLetter(g_display, 'a', 24, 50, 32, 60, false);
    drawLetter(g_display, 'd', 34, 50, 42, 60, false);
    drawLetter(g_display, 't', 54, 50, 62, 60, false);
    drawLetter(g_display, 'i', 64, 50, 72, 60, false);
    drawLetter(g_display, 'm', 74, 50, 82, 60, false);
    drawLetter(g_display, 'e', 84, 50, 92, 60, false);
    g_display.flush();

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
      uint8_t dayOfWeek = readValueWithPrompt("Weekday? (Su 1, Mo 2, Tu 3, We 4, Th 5, Fr 6, Sa 7) ");
      uint8_t hour = readValueWithPrompt("Hour? (0-23) ");
      uint8_t minute = readValueWithPrompt("Minute? (0-59) ");

      g_rtc.set(0, minute, hour, dayOfWeek, day, month, year);
    }
  }

  Serial.end();
}

// -------------------------------------------------------------------------------------------------

void loop() {
  // Show the time and battery for a while.  Get the current time first.
  g_rtc.refresh();

  // Show the Lines face.
  printLinesFace(g_display,
                 g_rtc.month(), g_rtc.day(), g_rtc.hour(), g_rtc.minute(), g_rtc.second(),
                 g_rtc.dayOfWeek(),
                 getBatteryPc());
  delay(c_defaultShowtimeTimeoutMs);

  // Power down if we're not connected (and are mostly likely developing/debugging).
  if (!getUsbAttached()) {
    powerDown();
  }
}

// =================================================================================================
// vim:ft=cpp
