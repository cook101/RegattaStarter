
/*************************************************************************************
  (cc) Jose Berengueres 4-8th Feb  2015, Dubai
  (LCD sample code by Mark Bramwell, July 2010)

  This is an Arduino based Ollie box implmentation used for regata starts

  Components:
  DF ROBOT LCD PANEL, 2 x RELAY Module v3.2, 1 x Arduino
  connect the relay of the horn to D2 of LCD shield. This relay drives aircompressor (12VDC battery)
  connect the relay that controls buzzer/beeper to D11.
**************************************************************************************/

#include <LiquidCrystal.h>

#include "Schedule.hpp"
#include "SystemState.hpp"


/*
   Interface to the button hardware.
*/
// Pins
const int kLcdButtonPin = 0;

// Button enumerations
enum class Button : uint8_t {
  right  = 0,
  up     = 1,
  down   = 2,
  left   = 3,
  select = 4,
  none   = 5,
  start_stop = left,
};



/*
   Interface to the sound hardware.
   There is a SoundInterface base class that is not intended
   for use but provides the interface functions.  The Horn
   and Bell classes define a private pin number for the
   hardware and are intended for use.
*/
class SoundInterface {
  public:
    SoundInterface() = delete;
    ~SoundInterface();
    static void initialize();
    static void turnOn();
    static void turnOff();
    // static bool isOn();
  protected:
    static const int kRelayPin = -1;
};
void SoundInterface::initialize() {
  pinMode(kRelayPin, OUTPUT);
};
void SoundInterface::turnOn() {
  digitalWrite(kRelayPin, HIGH);
}
void SoundInterface::turnOff() {
  digitalWrite(kRelayPin, LOW);
}

class Horn: public SoundInterface {
  protected:
    static const int kRelayPin = 11;
};

class Beep: public SoundInterface {
  protected:
    static const int kRelayPin = 2;
};



/*
   Interface to the display hardware.
   Provides a simple interface by encapsulating the details of the LiquidCrystal class.
   The LCD display has 2 rows of 16 characters.
*/
namespace display {

const uint8_t kZstringSize = 17;  // numCharPerLine + 1

namespace {
/*
   Everything inside this anonymous namespace is private to the display namespace.
   That is, nothing outside the display namespace can access the lcd object.
*/

// Display characteristics
const uint8_t kNumLines = 2;
const uint8_t kNumCharPerLine = 16;

// Text that displays an empty message
const char kEmptyMessage[kZstringSize] = "                ";

// Hardware pins
const uint8_t kRsPin = 8;  // controls commands
const uint8_t kEnablePin = 9;
const uint8_t kD0Pin = 4;
const uint8_t kD1Pin = 5;
const uint8_t kD2Pin = 6;
const uint8_t kD3Pin = 7;
const uint8_t kBacklightPin = 10;

// Display interface
LiquidCrystal lcd = LiquidCrystal(kRsPin, kEnablePin, kD0Pin, kD1Pin, kD2Pin, kD3Pin);

}  // namespace

void initialize() {
  lcd.begin(kNumCharPerLine, kNumLines);
  pinMode(kBacklightPin, OUTPUT);
}

void backlightOn() {
  digitalWrite(kBacklightPin, HIGH);
}

void backlightOff() {
  digitalWrite(kBacklightPin, LOW);
}

void overwrite(const char* const upper, const char* const lower) {
  lcd.setCursor(0, 0);
  lcd.print(kEmptyMessage);
  lcd.setCursor(0, 1);
  lcd.print(kEmptyMessage);
  lcd.setCursor(0, 0);
  lcd.print(upper);
  lcd.setCursor(0, 1);
  lcd.print(lower);
  return;
}

void clear() {
  overwrite("", "");
}

void overwriteUpper(const char* const upper) {
  lcd.setCursor(0, 0);
  lcd.print(kEmptyMessage);
  lcd.setCursor(0, 0);
  lcd.print(upper);
  return;
}

void overwriteLower(const char* const lower) {
  lcd.setCursor(0, 1);
  lcd.print(kEmptyMessage);
  lcd.setCursor(0, 1);
  lcd.print(lower);
  return;
}

}  // namespace display



// Messages (LCD is 2x16 characters)
const char* const kStartingMessage = " ...STARTING... ";
const char* const kCancelMessage   = "Timer Cancelled ";
const char* const JASC_5_MSG       = "JASC 1x5 min    ";
const char* const JASC_3_MSG       = "JASC 1x3 min    ";
const char* const DOSC_1x5_MSG     = "DOSC 1x5 min    ";
const char* const DOSC_3x5_MSG     = "DOSC 3x5 min    ";


// Horn sequences
// warning long buzz (sb)= 0,   short horn (sh) = 1, long horn (lh) =2  extralong horn 3
// Short seq countdown // 10lb  5sh   5blank  3lh

// Time lengths
const int kStandardDelay = 300; // (ms)
const unsigned long len_of_note[] = {500, 400, 800, 1500}; // (ms)

const int kNumSequences = 4;

// Sound names
const int WARNING_BEEP = 0;
const int RELAY_BEEP = 2;
const int RELAY_HORN = 11;

//************************************* THREE MIN SEQ ********************************
const unsigned long sch_3[] = { 0,   10, 20, 30, 40, 50,
                                100,
                                190, 200,
                                280, 290, 300,

                                1 * 600, 600 + 10, 600 + 20, 600 + 30, 600 + 40, 600 + 50,
                                900 - 15 - 20, 900 - 15 - 10, 900 - 15, 900,
                                2 * 600 - 15, 2 * 600, 2 * 600 + 10, 2 * 600 + 20, 2 * 600 + 30, 2 * 600 + 40, 2 * 600 + 50,
                                3 * 600 - 30, 3 * 600 - 15, 3 * 600,          3 * 600 + 60, 3 * 600 + 70, 3 * 600 + 80, 3 * 600 + 90,      3 * 600 + 100, 3 * 600 + 110, 3 * 600 + 120, 3 * 600 + 130, 3 * 600 + 140, 3 * 600 + 150, 3 * 600 + 160, 3 * 600 + 170, 3 * 600 + 180, 3 * 600 + 190, 3 * 600 + 200
                              };
const int h_or_b3[] = {        3,   1, 1, 1, 1, 1,
                               1,
                               1, 1,
                               1, 1, 1,
                               2, 0, 0, 0, 0, 0,
                               1, 1, 1, 2,
                               2, 2, 0, 0, 0, 0, 0,
                               2, 2, 2,   1, 1, 1, 1, 1,      0, 0, 0, 0, 0, 0, 0, 0, 0, 0
                      };
const unsigned int index_3 = 6 + 6 + 6 + 4 + 7 + 18 - 1;
const long ctdwn_3 = 205 * 1000L; // 3*60 + 39;



//************************************* FIVE MIN SEQ ********************************
const unsigned long sch_5[] = {0, 10, 20, 30, 40, 50,
                               1 * 600, 1 * 600 + 10, 1 * 600 + 20, 1 * 600 + 30, 1 * 600 + 40, 1 * 600 + 50,
                               4 * 600, 4 * 600 + 10, 4 * 600 + 20, 4 * 600 + 30, 4 * 600 + 40, 4 * 600 + 50,
                               5 * 600, 5 * 600 + 10, 5 * 600 + 20, 5 * 600 + 30, 5 * 600 + 40, 5 * 600 + 50, 5 * 600 + 60, 5 * 600 + 70, 5 * 600 + 80, 5 * 600 + 90, 5 * 600 + 100, 5 * 600 + 110, 5 * 600 + 120, 5 * 600 + 130, 5 * 600 + 140, 5 * 600 + 150, 5 * 600 + 160, 5 * 600 + 170, 5 * 600 + 180, 5 * 600 + 190, 5 * 600 + 200
                              };

const int h_or_b5[] =      {   3, 0, 0, 0, 0, 0,
                               2, 0, 0, 0, 0, 0,
                               2, 0, 0, 0, 0, 0,
                               2, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
                           };
const unsigned int index_5 = 28 + 6 + 5 - 1;
const long ctdwn_5 = (5 * 60 + 25) * 1000L; // (ms)


//************************************* DOSC x1********************************
const unsigned long sch_5british[] = {0, 10, 20, 30, 40, 50,
                                      1 * 600, 1 * 600 + 10, 1 * 600 + 20, 1 * 600 + 30, 1 * 600 + 40, 1 * 600 + 50,
                                      4 * 600, 4 * 600 + 10, 4 * 600 + 20, 4 * 600 + 30, 4 * 600 + 40, 4 * 600 + 50,
                                      5 * 600, 5 * 600 + 10, 5 * 600 + 20, 5 * 600 + 30, 5 * 600 + 40, 5 * 600 + 50
                                     };

const int h_or_b5british[] =      {  3, 0, 0, 0, 0, 0,
                                     2, 0, 0, 0, 0, 0,
                                     2, 0, 0, 0, 0, 0,
                                     2, 0, 0, 0, 0, 0
                                  };
const unsigned int index_5british = 4 * 6 - 1;
const long ctdwn_5british = (5 * 60 + 8) * 1000L; // (ms)

//************************************* DOSC ROLLING x3 ********************************
const unsigned long sch__3[] =   {    0,       10,       20,       30,       40,      50,
                                      1 * 600,  1 * 600 + 10,  1 * 600 + 20,  1 * 600 + 30,  1 * 600 + 40,  1 * 600 + 50,
                                      4 * 600,  4 * 600 + 10,  4 * 600 + 20,  4 * 600 + 30,  4 * 600 + 40,  4 * 600 + 50,
                                      5 * 600,  5 * 600 + 10,  5 * 600 + 20,  5 * 600 + 30,  5 * 600 + 40,  5 * 600 + 50,
                                      6 * 600,  6 * 600 + 10,  6 * 600 + 20,  6 * 600 + 30,  6 * 600 + 40,  6 * 600 + 50,
                                      9 * 600,  9 * 600 + 10,  9 * 600 + 20,  9 * 600 + 30,  9 * 600 + 40,  9 * 600 + 50,
                                      10 * 600, 10 * 600 + 10, 10 * 600 + 20, 10 * 600 + 30, 10 * 600 + 40, 10 * 600 + 50,
                                      11 * 600, 11 * 600 + 10, 11 * 600 + 20, 11 * 600 + 30, 11 * 600 + 40, 11 * 600 + 50,
                                      14 * 600, 14 * 600 + 10, 14 * 600 + 20, 14 * 600 + 30, 14 * 600 + 40, 14 * 600 + 50,
                                      15 * 600, 15 * 600 + 10, 15 * 600 + 20, 15 * 600 + 30, 15 * 600 + 40, 15 * 600 + 50
                                 };

const int h_or_b__3[] =          {   3, 0, 0, 0, 0, 0,
                                     2, 0, 0, 0, 0, 0,
                                     2, 0, 0, 0, 0, 0,

                                     3, 0, 0, 0, 0, 0,
                                     2, 0, 0, 0, 0, 0,
                                     2, 0, 0, 0, 0, 0,

                                     3, 0, 0, 0, 0, 0,
                                     2, 0, 0, 0, 0, 0,
                                     2, 0, 0, 0, 0, 0,
                                     2, 0, 0, 0, 0, 0
                                 };
const unsigned int index__3 = 10 * 6 - 1;
const long ctdwn__3 = (15 * 60 + 7) * 1000L; // (ms)



/***  Global Variables  ***/
Schedule schedule_5 = Schedule(sch_5, h_or_b5, index_5, ctdwn_5, JASC_5_MSG);
Schedule schedule_3 = Schedule(sch_3, h_or_b3, index_3, ctdwn_3, JASC_3_MSG);
Schedule schedule_5british = Schedule(sch_5british, h_or_b5british, index_5british, ctdwn_5british, DOSC_1x5_MSG);
Schedule schedule__3 = Schedule(sch__3, h_or_b__3, index__3, ctdwn__3, DOSC_3x5_MSG);


/*  System State */
SystemState state;



/***  Functions  ***/

/*
   Prepares the system for timer functionality.
   Run at power on.
*/
void setup() {

  // Initialize system state
  state.initialize();

  // Initialize sound relays
  Horn::initialize();
  Beep::initialize();

  // Initialize display, turning on backlight
  display::initialize();
  display::backlightOn();

  show_introduction();

  return;

}



/*
   Writes introductory text to the display.
*/
void show_introduction() {
  display::overwrite("BY CHRIS LABORDE", "");
  delay(kStandardDelay);
  display::overwrite("BY CHRIS LABORDE", " & J BERENGUERES");
  delay(2 * kStandardDelay);
  display::overwrite("  Select start", "    sequence");
  delay(kStandardDelay);
  return;
}



/*
   The command loop that runs the timer.
   Runs continuously after setup() completes.

   loop() performs three major, largely independent actions:
     1.) If the timer is running, it displays the timer status and emits sounds
     2.) If the start/stop button is pressed, it starts/stops the timer.
     3.) When the timer is stopped, the selection buttons allows the user to select
         a specific countdown sequence.
*/
void loop() {

  // If timer is running, perform the timer actions.  Specifically,
  // make sounds and display status at the appropriate times.
  if (state.isTimerRunning()) {
    long time_remaining_ms = state.getTimeRemaining_ms();
    horn_or_beep(time_remaining_ms);
    display_timer(time_remaining_ms);
  }

  // Depending on which button was pushed, we perform an action
  switch (read_LCD_buttons()) {
    case Button::start_stop:
      if (state.isTimerRunning()) {

        // Timer is running; stop timer
        display::overwrite(kCancelMessage, "");
        de_activate_sound(RELAY_HORN);
        de_activate_sound(RELAY_BEEP);
        state.stopTimer();
      } else {

        // Timer not running; start timer
        state.startTimer();
        display::overwrite(kStartingMessage, state.getSchedule()->getTitle());
      }
      delay(400);  // TODO: Remove after implementing more sophisticated debouncing
      break;

    case Button::select:
      // If the timer is not running, switch to next countdown sequence
      if (!state.isTimerRunning()) {
        increment_sequence_selection();
      }
      break;

    // Do-nothing scenarios
    case Button::right:
      [[fallthrough]]
    case Button::up:
      [[fallthrough]]
    case Button::down:
      [[fallthrough]]
    case Button::none:
      break;
  }
}


/*
   increment_sequence_selection()
   Switches through the available timer schedules that the
   system can follow.
*/
void increment_sequence_selection() {

  static short selected_sequence = -1;  // countdown sequence selection
  display::clear();

  selected_sequence += 1;
  if (selected_sequence >= kNumSequences) {
    selected_sequence = 0;
  }

  if (selected_sequence == 0) {
    state.setSchedule(schedule_5);
  } else if (selected_sequence == 1) {
    state.setSchedule(schedule_3);
  } else if (selected_sequence == 2) {
    state.setSchedule(schedule_5british);
  } else if (selected_sequence == 3) {
    state.setSchedule(schedule__3);
  }

  display::overwrite(state.getSchedule()->getTitle(), "");

  return;
}


Button read_LCD_buttons() {

  // My V1.1 buttons when read are centered at these values: 0, 144, 329, 504, 741.
  // We add approx 50 to those values and check to see if we are close.
  // We make "nothing" the first option for speed reasons since it will be the most likely result.

  int adc_key_in = analogRead(kLcdButtonPin);  // read the value from the sensor
  Button button = Button::none;

  if (adc_key_in <= 1000) {

    // Button pressed
    if (adc_key_in < 50) {
      button = Button::right;
    } else if (adc_key_in < 250) {
      button = Button::up;
    } else if (adc_key_in < 450) {
      button = Button::down;
    } else if (adc_key_in < 650) {
      button = Button::left;
    } else if (adc_key_in < 850) {
      button = Button::select;
    } else {
      button = Button::none;
    }

    // Add delay to debounce
    // TODO: Debounce better
    delay(200);
  }

  return button;
}



/*
   Turns on a sound.
*/
void activate_sound(const int sound) {
  // check what instrument to sound
  if (sound == WARNING_BEEP) {
    Beep::turnOn();
    state.setBeepOn();
  } else {
    Horn::turnOn();
    state.setHornOn();
  }
  return;
}



/*
   Turns off a sound.
*/
void de_activate_sound(const int sound) {
  if (sound == WARNING_BEEP) {
    Beep::turnOff();
    state.setBeepOff();
  } else {
    Horn::turnOff();
    state.setHornOff();
  }
  return;
}



/*
   Selects sound to turn on/off and causes the change to happen.
*/
void horn_or_beep(const unsigned long time_ms) {
  if (state.isSoundOn()) {
    // A sound is on; check if it should be turned off.
    if ( state.getTimeSinceSoundStart() > static_cast<int>(len_of_note[state.getSchedule()->getSound()]) ) {
      de_activate_sound(state.getSchedule()->getSound());
      state.getSchedule()->incrementIndex();
    }
  } else {
    // No sound is on; check if one should be turned on.
    unsigned long a_time_ms = (state.getSchedule()->getSch() * 100);
    //Serial.println(state.index);
    if (time_ms < a_time_ms + 1000) {
      activate_sound(state.getSchedule()->getSound());
    }
  }
  return;
}


/*
   Displays on the LCD screen the time remaining on the
   countdown timer.  The time remaining is read
   from the state struct.
*/
void display_timer(const long time_ms) {

  // Sound status sub-strings
  const char* const kHornOnMessage = "H";
  const char* const kBeepOnMessage  = "B";
  const char* const kSoundOffMessage = " ";

  // String that will be displayed
  char text[display::kZstringSize];

  if (time_ms > -1) {

    // Timer is running
    // Display time remaining and sounds on/off.
    int seconds = (time_ms / 1000) % 60;
    int minutes = (time_ms / 1000) / 60;

    const char* msg = kSoundOffMessage;
    if (state.isHornOn()) {
      msg = kHornOnMessage;
    } else if (state.isBeepOn()) {
      msg = kBeepOnMessage;
    }

    snprintf(text, display::kZstringSize, " %2d:%02d  %1s        ", minutes, seconds, msg);
    display::overwriteUpper(text);

  } else {
    // Timer has finished
    display::overwriteLower("START!");

  }
  return;
}
