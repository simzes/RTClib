/**
 * Sets up a timer, and waits for its interrupt signal to trigger a
 * wake pin.
 *
 * Requires a pin to be set (MONITOR_PIN) and wired to the RTC's INT/SWQ line
 * Otherwise, the wait will wait forever (pulseIn)
 *
 * The timer is programmed once in setup, and will repeat. The program waits
 * repeatedly in the loop for the wake pin.
 *
 * The wait function (pulseIn) could be replaced with a deep sleep call that
 * wakes when a pin changes. These are not portable, and so are not used here.
 */
#include <Wire.h>
#include "RTClib.h"

#define MONITOR_PIN 2 // pin wired from INT/SSW on RTC
#define TIMER_USED PCF8523_Timer_Countdown_B // which timer

RTC_PCF8523 rtc;
Pcf8523TimerState state;

void setup () {
  Serial.begin(9600);
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    abort();
  }

  // setup monitor pin for waking on
  pinMode(MONITOR_PIN, INPUT_PULLUP);

  // set timer to run
  state.enabled = true;
  state.value = 10;
  state.freq = PCF8523_FrequencySecond;
  state.irupt_state.irupt_flag = false;
  state.irupt_state.irupt_enabled = true;

  rtc.write_timer(TIMER_USED, &state);

  // dump timer info, for debug purposes
  rtc.read_timer(TIMER_USED, &state);
  Serial.println("Timer Set Up:");
  Serial.print("timer value: ");
  Serial.print(state.value, DEC);
  Serial.print(", enabled: ");
  Serial.print(state.enabled ? "yes": "no");
  Serial.print(", freq: ");
  Serial.print(state.freq, DEC);
  Serial.println();
  Serial.print("irupt flag: ");
  Serial.print(state.irupt_state.irupt_flag, DEC);
  Serial.print(", enabled: ");
  Serial.print(state.irupt_state.irupt_enabled, DEC);
  Serial.println();
  Serial.print("Monitor pin reading: ");
  Serial.println(digitalRead(MONITOR_PIN) ? "HIGH": "LOW");
  Serial.println();
}

void do_some_wake_action() {
  Serial.println("Woke up like this");
  // your wake action code here...
}

void loop () {
  // await wake pin changing with interrupt firing
  // (pulseIn returns after the MONITOR_PIN reads LOW)
  Serial.println("Waiting for the interrupt to fire...");
  pulseIn(MONITOR_PIN, LOW);

  // woke up, do something
  do_some_wake_action();

  // reset the interrupt for the next wait
  Serial.println("Resetting the interrupt flag for the next wait round");
  Pcf8523IruptState irupt;
  rtc.read_irupt(TIMER_USED, &irupt);

  irupt.irupt_flag = false;
  rtc.write_irupt(TIMER_USED, &irupt);
}
