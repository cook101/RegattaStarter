/*
   Regatta Starter System State

   This class is used to keep coherent the internal state of the system.
   The class does not perform any actions with the hardware; it provides
   solely a means to track the state machine.
*/

#pragma once

#include "Schedule.hpp"



class SystemState {
  public:
    SystemState() {
      initialize();
    };
    ~SystemState() = default;
    void initialize();
    void startTimer();
    void stopTimer();
    long getTimeRemaining_ms() const;
    bool isTimerRunning() const;
    bool isSoundOn() const;
    bool isHornOn() const;
    bool isBeepOn() const;
    void setHornOn();
    void setBeepOn();
    void setHornOff();
    void setBeepOff();
    long getTimeSinceSoundStart() const;
    void setSchedule(Schedule& sched);
    Schedule* getSchedule();

  private:
    Schedule* schedule;   // countdown timer schedule
    bool is_horn_on;  // horn to signal to racers
    bool is_beep_on;  // race committee warning beep
    long sound_start_ms;  // system time at sound start
    bool is_timer_running;
    long timer_start_ms;  // system time at sequence start
};
