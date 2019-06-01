
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



/*
 * Interface to the button hardware.
 */
// Pins
const int LCD_BUTTON_PIN = 0;

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
 * Interface to the sound hardware.
 * There is a SoundInterface base class that is not intended
 * for use but provides the interface functions.  The Horn
 * and Bell classes define a private pin number for the 
 * hardware and are intended for use.
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
    static const int relay_pin = -1;
};
void SoundInterface::initialize() {
  pinMode(relay_pin, OUTPUT);
};
void SoundInterface::turnOn() {
  digitalWrite(relay_pin, HIGH);
}
void SoundInterface::turnOff() {
  digitalWrite(relay_pin, LOW);
}

class Horn: public SoundInterface {
  protected:
    static const int relay_pin = 11;
};

class Beep: public SoundInterface {
  protected:
    static const int relay_pin = 2;
};



/*
 * Interface to the display hardware.
 * Provides a simple interface by encapsulating
 * the details of the LiquidCrystal class.
 */
namespace display {
  
  const uint8_t zstringSize = 17;  // numCharPerLine + 1
  
  namespace {
    // Everything inside this anonymous namespace is private to the display namespace.
    // That is, nothing outside the display namespace can access the lcd object.
    const uint8_t rsPin = 8;  // controls commands
    const uint8_t enablePin = 9;
    const uint8_t d0Pin = 4;
    const uint8_t d1Pin = 5;
    const uint8_t d2Pin = 6;
    const uint8_t d3Pin = 7;
    const uint8_t backlightPin = 10;
    const uint8_t numLines = 2;
    const uint8_t numCharPerLine = 16;  
    const char EMPTY_MSG[zstringSize] = "                ";
    LiquidCrystal lcd = LiquidCrystal(rsPin, enablePin, d0Pin, d1Pin, d2Pin, d3Pin);
  }  // namespace
  
  void initialize() {
    lcd.begin(numCharPerLine, numLines);
    pinMode(backlightPin, OUTPUT);
  }
  
  void backlightOn() {
    digitalWrite(backlightPin, HIGH);
  }
  
  void backlightOff() {
    digitalWrite(backlightPin, LOW);
  }

  void overwrite(const char* upper, const char* lower) {
    lcd.setCursor(0, 0);
    lcd.print(EMPTY_MSG);
    lcd.setCursor(0, 1);
    lcd.print(EMPTY_MSG);
    lcd.setCursor(0, 0);
    lcd.print(upper);
    lcd.setCursor(0, 1);
    lcd.print(lower);
    return;
  }

  void clear() {
    overwrite("", "");
  }

  void overwriteUpper(const char* upper) {
    lcd.setCursor(0, 0);
    lcd.print(EMPTY_MSG);
    lcd.setCursor(0, 0);
    lcd.print(upper);
    return;
  }
  
  void overwriteLower(const char* lower) {
    lcd.setCursor(0, 1);
    lcd.print(EMPTY_MSG);
    lcd.setCursor(0, 1);
    lcd.print(lower);
    return;
  }

}  // namespace display



// Messages (LCD is 2x16 characters)
const char* const STARTING_MSG = " ...STARTING... ";
const char* const JASC_5_MSG   = "JASC 1x5 min    ";
const char* const JASC_3_MSG   = "JASC 1x3 min    ";
const char* const DOSC_1x5_MSG = "DOSC 1x5 min    ";
const char* const DOSC_3x5_MSG = "DOSC 3x5 min    ";
const char* const BEG_TIME_MSG = "00:00           ";
const char* const CANCEL_MSG   = "SEQ. CANCELLED  ";
const char* const HORN_ON_MSG = "H";
const char* const BEEP_ON_MSG = "B";
const char* const SOUND_OFF_MSG = " ";


// Horn sequences
// warning long buzz (sb)= 0,   short horn (sh) = 1, long horn (lh) =2  extralong horn 3
// Short seq countdown // 10lb  5sh   5blank  3lh

// Time lengths
const int STD_DELAY = 300; // (ms)
const unsigned long len_of_note[] = {500, 400, 800, 1500}; // (ms)

const int NUMBER_OF_SEQUENCES = 4;

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
const int index_3 = 6 + 6 + 6 + 4 + 7 + 18 - 1;
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
const int index_5 = 28 + 6 + 5 - 1;
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
const int index_5british = 4 * 6 - 1;
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
const int index__3 = 10 * 6 - 1;
const long ctdwn__3 = (15 * 60 + 7) * 1000L; // (ms)



/***  Global Variables  ***/

/*  System State */
struct SystemState_t {
  bool is_timer_running;
  bool is_horn_on;  // horn to signal to racers
  bool is_beep_on;  // race committee warning beep
  short selected_sequence;  // countdown sequence selection
  long timer_start_ms;  // system time at sequence start
  long sound_start_ms;  // system time at sound start
  long timer_length_ms; // length of countdown sequence
  int index;  // position in countdown sequence instructions
};
SystemState_t state;

/* Countdown Procedure */
const unsigned long* sch;
const int* h_or_b;


/***  Functions  ***/

void setup() {

  // Initialize system
  state.is_timer_running = false;
  state.is_horn_on = false;
  state.is_beep_on = false;
  state.selected_sequence = 0;
  state.timer_start_ms = -1;
  state.sound_start_ms = -1;
  state.timer_length_ms = -1;
  state.index = -1;
  sch = nullptr;
  h_or_b = nullptr;

  // Initialize sound relays
  Horn::initialize();
  Beep::initialize();

  // Initialize display, turning on backlight
  display::initialize();
  display::backlightOn();

  show_introduction();

  return;

}



void loop() {

  long time_since_start_ms =  millis() - state.timer_start_ms;
  if (state.is_timer_running) {
    long time_remaining_ms = state.timer_length_ms - time_since_start_ms;
    horn_or_beep(time_remaining_ms);
    display_timer(time_remaining_ms);
  }

  // depending on which button was pushed, we perform an action
  switch (read_LCD_buttons()) {
    case Button::start_stop: {
        if (state.is_timer_running) {
          // Stop timer
          de_activate_sound(RELAY_HORN);
          de_activate_sound(RELAY_BEEP);
          display::overwrite(CANCEL_MSG, "");
          state.is_timer_running = false;
        } else {
          // Start timer
          if (state.selected_sequence == 0) {
            display::overwrite(STARTING_MSG, JASC_5_MSG);
            state.timer_length_ms = ctdwn_5;
            sch = sch_5;
            h_or_b = h_or_b5;
            state.index = index_5;
          } else if (state.selected_sequence == 1) {
            display::overwrite(STARTING_MSG, JASC_3_MSG);
            state.timer_length_ms = ctdwn_3;
            sch = sch_3;
            h_or_b = h_or_b3;
            state.index = index_3;
          } else if (state.selected_sequence == 2) {
            display::overwrite(STARTING_MSG, DOSC_1x5_MSG);
            state.timer_length_ms = ctdwn_5british;
            sch = sch_5british;
            h_or_b = h_or_b5british;
            state.index = index_5british;
          } else if (state.selected_sequence == 3) {
            display::overwrite(STARTING_MSG, DOSC_3x5_MSG);
            state.timer_length_ms = ctdwn__3;
            sch = sch__3;
            h_or_b = h_or_b__3;
            state.index = index__3;
          }
          state.is_horn_on = false;
          state.is_beep_on = false;
          state.timer_start_ms = millis();
          state.is_timer_running = true;
        }
        delay(400);  // TODO: Remove after implementing more sophisticated debouncing
        break;
      }

    case Button::select: {
        if (!state.is_timer_running) {
          increment_sequence_selection();
        }
        break;
      }

    // Do-nothing scenarios
    case Button::right:
      [[fallthrough]]
    case Button::up:
      [[fallthrough]]
    case Button::down:
      [[fallthrough]]
    case Button::none: {
        break;
      }
  }
}



void increment_sequence_selection() {

  const char* msg = "";
  display::clear();

  state.selected_sequence += 1;
  if (state.selected_sequence >= NUMBER_OF_SEQUENCES) {
    state.selected_sequence = 0;
  }

  if (state.selected_sequence == 0) {
    msg = JASC_5_MSG;
  } else if (state.selected_sequence == 1) {
    msg = JASC_3_MSG;
  } else if (state.selected_sequence == 2) {
    msg = DOSC_1x5_MSG;
  } else if (state.selected_sequence == 3) {
    msg = DOSC_3x5_MSG;
  }

  display::overwrite(msg, "");

  return;
}


Button read_LCD_buttons() {
  int adc_key_in = analogRead(LCD_BUTTON_PIN);  // read the value from the sensor
  Button button = Button::none;
  // My V1.1 buttons when read are centered at these values: 0, 144, 329, 504, 741.
  // We add approx 50 to those values and check to see if we are close.
  // We make "nothing" the first option for speed reasons since it will be the most likely result.

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



void show_introduction() {
  display::overwrite("BY CHRIS LABORDE", "");
  delay(STD_DELAY);
  display::overwrite("BY CHRIS LABORDE", " & J BERENGUERES");
  delay(3 * STD_DELAY);
  display::overwrite("  Select start", "    sequence");
  delay(STD_DELAY);
  return;
}



void activate_sound(int sound) {
  // check what instrument to sound
  if (sound == WARNING_BEEP) {
    Beep::turnOn();
  } else {
    Horn::turnOn();
  }
  state.sound_start_ms = millis();
  if (sound == WARNING_BEEP)  {
    state.is_beep_on = true;
  } else {
    state.is_horn_on = true;
  }
  return;
}



void de_activate_sound(int sound) {
  if (sound == WARNING_BEEP) {
    Beep::turnOff();
    state.is_beep_on = false;
  } else {
    Horn::turnOff();
    state.is_horn_on = false;
  }
  return;
}



void horn_or_beep(unsigned long time_ms) {
  if (state.is_horn_on || state.is_beep_on) {
    if ( ((millis() - state.sound_start_ms) > len_of_note[h_or_b[state.index]] ) ) {
      de_activate_sound(h_or_b[state.index]);
      state.index -= 1;
    }
  } else {
    unsigned long a_time_ms = (sch[state.index] * 100);
    //Serial.println("long v = (long) (sch[state.index]*1000);");
    //Serial.println(state.index);
    //Serial.println(s);
    //Serial.println(v);

    if (time_ms < a_time_ms + 1000) {
      activate_sound(h_or_b[state.index]);
    }
  }
  return;
}



void display_timer(long time_ms) {

  char text[display::zstringSize];

  if (time_ms > -1) {

    int seconds = (time_ms / 1000) % 60;
    int minutes = (time_ms / 1000) / 60;

    const char* msg;
    if (state.is_horn_on) {
      msg = HORN_ON_MSG;
    } else if (state.is_beep_on) {
      msg = BEEP_ON_MSG;

    } else {
      msg = SOUND_OFF_MSG;
    }

    snprintf(text, display::zstringSize, " %1d:%02d  %1s        ", minutes, seconds, msg);
    display::overwriteUpper(text);

  } else {

    display::overwriteLower("START!");
    Horn::turnOff();  // TODO:  Remove sound side-effect from display function

  }
  return;
}
