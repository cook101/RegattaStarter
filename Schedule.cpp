/*
   Countdown Schedule

   This class is used to wrap the countdown schedule so that it cannot
   become unsynchronized.  Once the constructor has been called, the
   only possible interactions are to get information about the next step
   in the schedule and to move the schedule a step forward.
*/

#include "Schedule.hpp"

Schedule::Schedule(const unsigned long* sch_, const int* h_or_b_, 
                   unsigned int size_schedule, long timer_length_ms, const char* title_)
  : sch(sch_), h_or_b(h_or_b_), index(size_schedule), timerLength_ms(timer_length_ms), title(title_)
{
  return;
}
