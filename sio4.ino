// =================================================================================================

#include <Wire.h>

#include <avr/power.h>
#include <avr/sleep.h>

#include <uRTCLib.h>
#include <YetAnotherPcInt.h>

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

uRTCLib rtc(URTCLIB_ADDRESS);   // I2C address.

// -------------------------------------------------------------------------------------------------

void setup() {
  Serial.begin(9600);
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
  rtc.set_model(URTCLIB_MODEL_DS3231);
  // XXX set it to bogus datetime
  rtc.set(0, 0, 16, 4, 25, 6, 20);  // Thu 25/6/20 4:00:00pm.
}

// -------------------------------------------------------------------------------------------------

void powerDown() {
  // Power down everything.
  power_adc_disable();
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();

  // Sleep.
  sleep_cpu();

  // ... wake.

  // Power up.
  sleep_disable();
  power_adc_enable();
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

void temp_sleepAndFlash() {
  // Flash 3 times.
  for (int16_t count = 0; count < 3; count++) {
    delay(950);
    digitalWrite(c_rightLedPin, HIGH);
    delay(50);
    digitalWrite(c_rightLedPin, LOW);
  }

  powerDown();

  digitalWrite(c_leftLedPin, HIGH);
  delay(1000);
  digitalWrite(c_leftLedPin, LOW);
}

void loop() {
#if 1
  digitalWrite(c_leftLedPin, HIGH);
  delay(10000);
  digitalWrite(c_leftLedPin, LOW);

  temp_sleepAndFlash();
#else

  delay(2000);

  Serial.print("battery: ");  Serial.println(getRawBattery());
  Serial.print("time: ");

  rtc.refresh();
  Serial.print(rtc.day()); Serial.print('/'); Serial.print(rtc.month()); Serial.print('/'); Serial.print(rtc.year()); Serial.print(' ');
  Serial.print(rtc.hour()); Serial.print(':'); Serial.print(rtc.minute()); Serial.print(':'); Serial.println(rtc.second());

  Serial.print("temp: "); Serial.println(rtc.temp());

  if (getUsbPowered()) {
    digitalWrite(c_leftLedPin, HIGH);
  }
  if (getCharging()) {
    digitalWrite(c_rightLedPin, HIGH);
  }
  delay(100);
  digitalWrite(c_leftLedPin, LOW);
  digitalWrite(c_rightLedPin, LOW);
#endif
}

// =================================================================================================
// vim:ft=cpp
