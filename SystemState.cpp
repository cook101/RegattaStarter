/*
   Countdown Schedule

   This class is used to wrap the countdown schedule so that it cannot
   become unsynchronized.  Once the constructor has been called, the
   only possible interactions are to get information about the next step
   in the schedule and to move the schedule a step forward.
*/

#include "SystemState.hpp"

#include <Arduino.h>  // for millis()



void SystemState::initialize() {
  is_timer_running = false;
  is_horn_on = false;
  is_beep_on = false;
  timer_start_ms = -1;
  sound_start_ms = -1;
}

void SystemState::startTimer() {
  schedule->reset();
  is_horn_on = false;
  is_beep_on = false;
  timer_start_ms = millis();
  is_timer_running = true;
  return;
}

void SystemState::stopTimer() {
  is_timer_running = false;
}

long SystemState::getTimeRemaining_ms() const {
  long time_since_start_ms =  millis() - timer_start_ms;
  return schedule->getTimerLength_ms() - time_since_start_ms;
}

bool SystemState::isTimerRunning() const {
  return is_timer_running;
}

bool SystemState::isSoundOn() const {
  return is_horn_on || is_beep_on;
}

bool SystemState::isHornOn() const {
  return is_horn_on;
}

bool SystemState::isBeepOn() const {
  return is_beep_on;
}

void SystemState::setHornOn() {
  is_horn_on = true;
  sound_start_ms = millis();
}

void SystemState::setBeepOn() {
  is_beep_on = true;
  sound_start_ms = millis();
}

void SystemState::setHornOff() {
  is_horn_on = false;
}

void SystemState::setBeepOff() {
  is_beep_on = false;
}

long SystemState::getTimeSinceSoundStart() const {
  return millis() - sound_start_ms;
}

void SystemState::setSchedule(Schedule& sched) {
  schedule = &sched;
}

Schedule* SystemState::getSchedule() {
  return schedule;
}
