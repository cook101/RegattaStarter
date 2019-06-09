/*
   Countdown Schedule

   This class is used to wrap the countdown schedule so that it cannot
   become unsynchronized.  Once the constructor has been called, the
   only possible interactions are to get information about the next step
   in the schedule and to move the schedule a step forward.
*/

#include "Schedule.hpp"

Schedule::Schedule(const unsigned long* sch_, const int* h_or_b_,
                   const unsigned int &size_schedule_, const long timer_length_ms, const char* const title_)
  : sch(sch_), h_or_b(h_or_b_), size_schedule(size_schedule_), index(size_schedule_),
    timerLength_ms(timer_length_ms), title(title_)
{
  return;
}
