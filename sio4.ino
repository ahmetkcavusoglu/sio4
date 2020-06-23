// =================================================================================================

#include <YetAnotherPcInt.h>

// -------------------------------------------------------------------------------------------------

constexpr int8_t c_upperLeftButtonPin = 8;
constexpr int8_t c_upperRightButtonPin = 11;
constexpr int8_t c_lowerRightButtonPin = 10;

// -------------------------------------------------------------------------------------------------

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

void logMessage() {
  Serial.begin(9600);
  Serial.print("This is message "); Serial.println(count++);
  Serial.end();
}

// -------------------------------------------------------------------------------------------------

void setup() {
  pinMode(c_upperLeftButtonPin, INPUT_PULLUP);
  pinMode(c_upperRightButtonPin, INPUT_PULLUP);
  pinMode(c_lowerRightButtonPin, INPUT_PULLUP);

  PcInt::attachInterrupt(c_lowerRightButtonPin, buttonLrbIsr, CHANGE);
}

// -------------------------------------------------------------------------------------------------

void loop() {
  Serial.begin(9600);
  Serial.print("LRB is "); Serial.println(lowerRightButtonPressed ? "ON" : "OFF");
  Serial.print("interrupt count: "); Serial.println(interruptCount);
  Serial.end();

  delay(1000);
}

#if 0
void loop_sleep() {
  // Disable ADC System via PRR Register Bit 0 (PRADC)
  power_adc_disable();

  // Setup SleepMode , see https://www.gammon.com.au/forum/?id=11497
  set_sleep_mode (SLEEP_MODE_PWR_DOWN);

  // Enable Sleep Mode by setting SMCR Register Bit 0 and send the CPU to sleep
  sleep_enable();

  attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(BUTTON3), wakeFromBUTTON, FALLING);

  // Put CPU to sleep.
  sleep_cpu();

  // Wake.

  // Disable sleep
  sleep_disable();

  // Disable 32u4 MPU interrupt(s)
  detachPinChangeInterrupt(digitalPinToPinChangeInterrupt(BUTTON3));

  // Enable ADC via PRR Register Bit 0 (PRADC)
  power_adc_enable();

  // Log message.
  logMessage();
}
#endif

// =================================================================================================
// vim:ft=cpp
