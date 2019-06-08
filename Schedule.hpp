/*
   Countdown Schedule

   This class is used to wrap the countdown schedule so that it cannot
   become unsynchronized.  Once the constructor has been called, the
   only possible interactions are to get information about the next step
   in the schedule and to move the schedule a step forward.
*/

class Schedule {
  public:
    Schedule(const unsigned long* sch, const int* h_or_b, unsigned int size_schedule, 
             long timer_length_ms, const char* title);
    Schedule() {};
    ~Schedule() {};
    inline void incrementIndex() {
      index--;
    };
    inline unsigned long getSch() {
      return sch[index];
    }
    inline int getSound() {
      return h_or_b[index];
    }
    inline long getTimerLength_ms() {
      return timerLength_ms;
    }
    inline const char* getTitle() {
      return title;
    }

  private:
    const unsigned long* sch = nullptr;  // Next time to make a sound
    const int* h_or_b = nullptr;  // Next sound to make
    unsigned int index = -1;  // position in countdown sequence instructions
    long timerLength_ms = -1;  // length of countdown sequence
    const char* title = "";  // Display title of countdown sequence
};
