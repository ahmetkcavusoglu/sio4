// =================================================================================================

#include <avr/power.h>
#include <avr/sleep.h>

#include <YetAnotherPcInt.h>

// -------------------------------------------------------------------------------------------------

constexpr int8_t c_upperLeftButtonPin = 8;
constexpr int8_t c_upperRightButtonPin = 11;
constexpr int8_t c_lowerRightButtonPin = 10;

constexpr int8_t c_leftLedPin = 13;
constexpr int8_t c_rightLedPin = 6;

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

int count = 0;

void logMessage(const char* msg) {
  Serial.print(count++); Serial.print(": ");
  Serial.println(msg);
}

// -------------------------------------------------------------------------------------------------

void setup() {
  // Pull the button pins high.
  pinMode(c_upperLeftButtonPin, INPUT_PULLUP);
  pinMode(c_upperRightButtonPin, INPUT_PULLUP);
  pinMode(c_lowerRightButtonPin, INPUT_PULLUP);

  // Set the LED pins for output.
  pinMode(c_leftLedPin, OUTPUT);
  pinMode(c_rightLedPin, OUTPUT);

  // Install an ISR for LRB.
  PcInt::attachInterrupt(c_lowerRightButtonPin, buttonLrbIsr, CHANGE);

  Serial.begin(9600);
}

// -------------------------------------------------------------------------------------------------

void loop() {
  // Flash 3 times.
  for (int16_t count = 0; count < 3; count++) {
    delay(950);
    digitalWrite(c_rightLedPin, HIGH);
    delay(50);
    digitalWrite(c_rightLedPin, LOW);
  }

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

  digitalWrite(c_leftLedPin, HIGH);
  delay(1000);
  digitalWrite(c_leftLedPin, LOW);
}

// =================================================================================================
// vim:ft=cpp
