/*
   Countdown Schedule

   This class is used to wrap the countdown schedule so that it cannot
   become unsynchronized.  Once the constructor has been called, the
   only possible interactions are to get information about the next step
   in the schedule and to move the schedule a step forward.
*/

class Schedule {
  public:
    Schedule(const unsigned long* sch, const int* h_or_b, const unsigned int &size_schedule, 
             const long timer_length_ms, const char* const title);
    Schedule() {};
    ~Schedule() {};
    inline void incrementIndex() {
      index--;
    };
    inline unsigned long getSch() const {
      return sch[index];
    }
    inline int getSound() const {
      return h_or_b[index];
    }
    inline long getTimerLength_ms() const {
      return timerLength_ms;
    }
    inline const char* getTitle() const {
      return title;
    }
    inline void reset() {
      index = size_schedule;
    }

  private:
    const unsigned long* sch = nullptr;  // Next time to make a sound
    const int* h_or_b = nullptr;  // Next sound to make
    unsigned int size_schedule = 0;
    unsigned int index = 0;  // position in countdown sequence instructions
    long timerLength_ms = -1;  // length of countdown sequence
    const char* title = "";  // Display title of countdown sequence
};
