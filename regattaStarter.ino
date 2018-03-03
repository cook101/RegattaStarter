
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



/***  Constants  ***/

const int NUM_SEQ = 4;  // Number of menu options

// Button enumerations
const int BUTTON_RIGHT  = 0;
const int BUTTON_UP     = 1;
const int BUTTON_DOWN   = 2;
const int BUTTON_LEFT   = 3;
const int BUTTON_SELECT = 4;
const int BUTTON_NONE   = 5;

const int WARNING_BEEP = 0;  // enumeration


// Pins
const int RELAY_BEEP = 2;   // beeper relay pin
const int RELAY_HORN = 11;  // horn relay pin
const int LCD_BUTTON_PIN = 0;

// Time lengths
const int STD_DELAY = 300; // (ms)
const unsigned long len_of_note[] = {500, 400, 800, 1500}; // (ms)

// Messages (LCD is 2x16 characters)
const char* const STARTING_MSG = " ...STARTING... ";
const char* const JASC_5_MSG   = "JASC 1x5 min    ";
const char* const JASC_3_MSG   = "JASC 1x3 min    ";
const char* const DOSC_1x5_MSG = "DOSC 1x5 min    ";
const char* const DOSC_3x5_MSG = "DOSC 3x5 min    ";
const char* const DOSC_3x5_ALT_MSG = "DOSC ROLLING 3x5";
const char* const EMPTY_MSG    = "                ";
const char* const BEG_TIME_MSG = "00:00           ";
const char* const CANCEL_MSG   = "SEQ. CANCELLED  ";
const char* const HORN_ON_MSG = "H";
const char* const BEEP_ON_MSG = "B";
const char* const SOUND_OFF_MSG = " ";


// Horn sequences
// warning long buzz (sb)= 0,   short horn (sh) = 1, long horn (lh) =2  extralong horn 3
// Short seq countdown // 10lb  5sh   5blank  3lh

//************************************* THREE MIN SEQ ********************************
const unsigned long sch_3[] = { 0,   10, 20, 30, 40, 50,
                              100, 
                              190, 200, 
                              280,290,300, 
                              
                              1*600, 600+10, 600+20, 600+30, 600+40, 600+50,
                              900-15-20, 900-15-10, 900-15, 900,
                              2*600-15, 2*600, 2*600+10, 2*600+20, 2*600+30, 2*600+40, 2*600+50,
                              3*600-30, 3*600-15, 3*600,          3*600+60, 3*600+70,3*600+80,3*600+90,      3*600+100,3*600+110, 3*600+120, 3*600+130, 3*600+140, 3*600+150, 3*600+160,3*600+170, 3*600+180, 3*600+190, 3*600+200  };
const int h_or_b3[] = {        3,   1,1,1,1,1,
                               1,
                               1,1,
                               1,1,1,
                            2, 0,0,0,0,0,
                            1,1,1,2,
                            2,2,0,0,0,0,0,
                            2,2,2,   1,1,1,1,1,      0, 0, 0, 0, 0, 0, 0, 0, 0, 0  };
const int index_3 = 6 + 6 + 6 + 4 + 7 + 18 - 1;
const long ctdwn_3 = 205*1000L; // 3*60 + 39;



//************************************* FIVE MIN SEQ ********************************
const unsigned long sch_5[] = {0, 10, 20, 30, 40, 50,
                               1*600, 1*600 + 10, 1*600 + 20, 1*600 + 30, 1*600 + 40, 1*600 + 50,
                               4*600, 4*600 + 10, 4*600 + 20, 4*600 + 30, 4*600 + 40, 4*600 + 50,
                               5*600, 5*600 + 10, 5*600 + 20, 5*600 + 30, 5*600 + 40, 5*600 + 50, 5*600 + 60, 5*600 + 70, 5*600 + 80, 5*600 + 90, 5*600 + 100, 5*600 + 110, 5*600 + 120, 5*600 + 130, 5*600 + 140, 5*600 + 150, 5*600 + 160, 5*600 + 170, 5*600 + 180, 5*600 + 190, 5*600 + 200
                              };

const int h_or_b5[] =      {   3, 0, 0, 0, 0, 0,
                               2, 0, 0, 0, 0, 0,
                               2, 0, 0, 0, 0, 0,
                               2, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
                           };
const int index_5 = 28 + 6 + 5 - 1;
const long ctdwn_5 = (5 * 60 + 25)*1000L;  // (ms)


//************************************* DOSC x1********************************
const unsigned long sch_5british[] = {0, 10, 20, 30, 40, 50,
                                      1*600, 1*600 + 10, 1*600 + 20, 1*600 + 30, 1*600 + 40, 1*600 + 50,
                                      4*600, 4*600 + 10, 4*600 + 20, 4*600 + 30, 4*600 + 40, 4*600 + 50,
                                      5*600, 5*600 + 10, 5*600 + 20, 5*600 + 30, 5*600 + 40, 5*600 + 50
                                     };

const int h_or_b5british[] =      {  3, 0, 0, 0, 0, 0,
                                     2, 0, 0, 0, 0, 0,
                                     2, 0, 0, 0, 0, 0,
                                     2, 0, 0, 0, 0, 0
                                  };
const int index_5british = 4 * 6 - 1;
const long ctdwn_5british = (5 * 60 + 8)*1000L;  // (ms)

//************************************* DOSC ROLLING x3 ********************************
const unsigned long sch__3[] =   {    0,       10,       20,       30,       40,      50,
                                      1*600,  1*600 + 10,  1*600 + 20,  1*600 + 30,  1*600 + 40,  1*600 + 50,
                                      4*600,  4*600 + 10,  4*600 + 20,  4*600 + 30,  4*600 + 40,  4*600 + 50,
                                      5*600,  5*600 + 10,  5*600 + 20,  5*600 + 30,  5*600 + 40,  5*600 + 50,
                                      6*600,  6*600 + 10,  6*600 + 20,  6*600 + 30,  6*600 + 40,  6*600 + 50,
                                      9*600,  9*600 + 10,  9*600 + 20,  9*600 + 30,  9*600 + 40,  9*600 + 50,
                                      10*600, 10*600 + 10, 10*600 + 20, 10*600 + 30, 10*600 + 40, 10*600 + 50,
                                      11*600, 11*600 + 10, 11*600 + 20, 11*600 + 30, 11*600 + 40, 11*600 + 50,
                                      14*600, 14*600 + 10, 14*600 + 20, 14*600 + 30, 14*600 + 40, 14*600 + 50,
                                      15*600, 15*600 + 10, 15*600 + 20, 15*600 + 30, 15*600 + 40, 15*600 + 50
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
const long ctdwn__3 = (15 * 60 + 7)*1000L;  // (ms)



/***  Global Variables  ***/

bool my_start;
bool sound_on;
int button_press_counter;  // button presses
long timer_start_ms;  // sequence start time
long sound_start_ms;  // sound start time
long countdown_ms;
int index;

const unsigned long* sch;
const int* h_or_b;

LiquidCrystal lcd(8, 9, 4, 5, 6, 7); // select the pins used on the LCD panel



/***  Procedures  ***/

void setup() {

  my_start = false;
  sound_on = false;
  button_press_counter = 0;
  timer_start_ms = -1;
  sound_start_ms = -1;
  countdown_ms = -1;
  index = 0;
  sch = NULL;
  h_or_b = NULL;

  Serial.begin(9600);
  //Serial.println("hello");
  pinMode(RELAY_HORN, OUTPUT);
  pinMode(RELAY_BEEP, OUTPUT);
  lcd.begin(16, 2);  
  mymenu();
  
}


int read_LCD_buttons() {
  int adc_key_in = analogRead(LCD_BUTTON_PIN);       // read the value from the sensor

  // My V1.1 buttons when read are centered at these values: 0, 144, 329, 504, 741.
  // We add approx 50 to those values and check to see if we are close.
  // We make "nothing" the first option for speed reasons since it will be the most likely result.

  if (adc_key_in > 1000) {
    return BUTTON_NONE;
  } else if (adc_key_in < 50) {
    return BUTTON_RIGHT;
  } else if (adc_key_in < 250) {
    return BUTTON_UP;
  } else if (adc_key_in < 450) {
    return BUTTON_DOWN;
  } else if (adc_key_in < 650) {
    return BUTTON_LEFT;
  } else if (adc_key_in < 850) {
    return BUTTON_SELECT;
  }

  // when all others fail, return this.
  return BUTTON_NONE;
}


// utility to print on LCD
void lcd_overwrite(const char upper[16], const char lower[16]) {
  lcd.setCursor(0, 0);
  lcd.print(EMPTY_MSG);
  lcd.setCursor(0, 1);
  lcd.print(EMPTY_MSG);
  lcd.setCursor(0, 0);
  lcd.print(upper);
  delay(STD_DELAY);
  lcd.setCursor(0, 1);
  lcd.print(lower);
  delay(STD_DELAY);
}


void mymenu() {
  lcd_overwrite("BY CHRIS LABORDE", " & J BERENGUERES");
  delay(STD_DELAY);
  lcd_overwrite("SELECT 5 OR 3 ", "MINUTE SEQUENCE ");
}
/*
  void vibrate(int n,int b, int c) {
  for(int i=0;i<n;i++){
    digitalWrite(RELAY_PIN, HIGH);
    delay(b);
    digitalWrite(RELAY_PIN, LOW);
    delay(c);

  }
  }
*/


void activate_sound(int sound) {
  // check what instrument to sound
  int what_beep = RELAY_HORN;
  if (sound == WARNING_BEEP) {
    what_beep = RELAY_BEEP;
  }
  digitalWrite(what_beep, HIGH);
  sound_start_ms = millis();
  lcd.setCursor(7, 1);
  if (sound == WARNING_BEEP)  {
    lcd.print(BEEP_ON_MSG);
  } else {
    lcd.print(HORN_ON_MSG);
  }
}


void de_activate_sound(int sound) {
  int what_beep = RELAY_HORN;
  if (sound == WARNING_BEEP) {
    what_beep = RELAY_BEEP;
  }
  digitalWrite(what_beep, LOW);
  lcd.setCursor(7, 1);
  lcd.print(SOUND_OFF_MSG);
}


void horn_or_beep(unsigned long time_ms) {
  if (sound_on) {
    if ( ((millis() - sound_start_ms) > len_of_note[h_or_b[index]] ) ) {
      de_activate_sound(h_or_b[index]);
      sound_on = false;
      index = index - 1;
    }
  } else {
    unsigned long a_time_ms = (sch[index] * 100);
    //Serial.println("long v = (long) (sch[index]*1000);");
    //Serial.println(index);
    //Serial.println(s);
    //Serial.println(v);

    if (time_ms < a_time_ms + 1000) {
      activate_sound(h_or_b[index]);
      sound_on = true;
    }
  }
}


void diplay_timer(long time_ms) {

  if (time_ms > -1) {

    int seconds = (time_ms / 1000) % 60;
    int minutes = (time_ms / 1000) / 60;
    int sec_pos = 3;

    lcd.setCursor(1, 1);
    lcd.print(minutes);

    lcd.setCursor(2, 1);
    lcd.print(":");
    
    if (seconds < 10) {
      sec_pos += 1;
      lcd.setCursor(3, 1);
      lcd.print("0");
    }
    lcd.setCursor(sec_pos, 1);
    lcd.print(seconds);

  } else {

    lcd.setCursor(7, 1);
    lcd.print("START!");
    digitalWrite(RELAY_HORN, LOW);

  }
}


void loop() {

  const char* msg;
  int lcd_key = 0;
  long time_since_start_ms =  millis() - timer_start_ms;
  if (my_start) {
    long timer_now_ms = countdown_ms - time_since_start_ms;
    horn_or_beep(timer_now_ms);
    diplay_timer(timer_now_ms);
  }

  lcd_key = read_LCD_buttons();
  delay(50);

  // depending on which button was pushed, we perform an action
  switch (lcd_key) {
    case BUTTON_LEFT: {
        my_start = !my_start;
        if (my_start) {
          if (button_press_counter == 0) {
            lcd_overwrite(STARTING_MSG, JASC_5_MSG);
            countdown_ms = ctdwn_5;
            sch = sch_5;
            h_or_b = h_or_b5;
            index = index_5;
          }
          if (button_press_counter == 1) {
            lcd_overwrite(STARTING_MSG, JASC_3_MSG);
            countdown_ms = ctdwn_3;
            sch = sch_3;
            h_or_b = h_or_b3;
            index = index_3;
          }
          if (button_press_counter == 2) {
            lcd_overwrite(STARTING_MSG, DOSC_1x5_MSG);
            countdown_ms = ctdwn_5british;
            sch = sch_5british;
            h_or_b = h_or_b5british;
            index = index_5british;
          }
          if (button_press_counter == 3) {
            lcd_overwrite(STARTING_MSG, DOSC_3x5_MSG);
            countdown_ms = ctdwn__3;
            sch = sch__3;
            h_or_b = h_or_b__3;
            index = index__3;
          }
          lcd.setCursor(0, 1);
          lcd.print(BEG_TIME_MSG);
          sound_on = false;
          timer_start_ms = millis();
          break;
        } else {
          // stop
          digitalWrite(RELAY_HORN, LOW);
          digitalWrite(RELAY_BEEP, LOW);
          lcd.setCursor(0, 0);
          lcd.print(CANCEL_MSG);
          my_start = false;
        }
        delay(400);
        break;
      }

    case BUTTON_SELECT: {
        if (!my_start) {
          button_press_counter += 1;
          if (button_press_counter > (NUM_SEQ - 1)) {
            button_press_counter = 0;
          }
          if (button_press_counter == 0) {
            msg = JASC_5_MSG;
          }
          if (button_press_counter == 1) {
            msg = JASC_3_MSG;
          }
          if (button_press_counter == 2) {
            msg = DOSC_1x5_MSG;
          }
          if (button_press_counter == 3) {
            msg = DOSC_3x5_ALT_MSG;
          }
          lcd_overwrite(msg, EMPTY_MSG);
          break;
        }
      }

    case BUTTON_NONE: {
        break;
      }
  }
}

