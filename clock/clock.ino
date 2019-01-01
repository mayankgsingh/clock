#include "Wire.h"
#include "RTClib.h"
#include "DFRobot_LCD.h"
#include <avr/power.h>

//RTC Vars
RTC_DS3231 rtc;
//const char DOW[7][4] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
const char MONTH_SHORT[12][4] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
//LCD Vars
DFRobot_LCD lcd(16,2);

volatile uint8_t tmpMin = 0;

//Pin Configs
int8_t MODE = 4;
int8_t MODE_VAL = 3; //INCREASE VALUE

//logic vars
char LINES[2][17] = {"", ""};
int8_t clockMode = 0;
boolean clockFormat12 = true;

const char TIMEZONE_NAME[] = "Sydney          ";
boolean isDst = true;

const uint8_t modeDelay = 300;
DateTime curr;
uint16_t diff = ((isDst?1:0) * 3600);
boolean refreshDisplay = true;

void setup () {
  if (F_CPU == 16000000) clock_prescale_set(clock_div_1);

  pinMode(MODE, INPUT);
  pinMode(MODE_VAL, INPUT_PULLUP);
  
  //init LCD
    lcd.init();
    //lcd.setRGB(255, 0, 0);//If the module is a monochrome screen, you need to shield it
    lcd.setPWM(REG_ONLY, 120);
  // Print a message to the LCD.
  delay(500);
}

void loop () {
  curr = rtc.now();
  
  boolean modeVal = false;
 /*Connect Trinket using BAT & GND PINS then enable this section.
  * boolean modeVal = digitalRead(MODE);
  if(modeVal) {
    clockFormat12 = !clockFormat12;
    refreshDisplay = true;
  }*/

  boolean chgModeVal = !digitalRead(MODE_VAL);
  if(chgModeVal){
    isDst = !isDst;
    diff = ((isDst?1:0) * 3600);
    refreshDisplay = true;
  }
  displayClock();

  if(refreshDisplay) {
    printLines();
    refreshDisplay = !refreshDisplay;
    if(modeVal || chgModeVal) {
      delay(modeDelay);
    }
  }
}
  
void displayClock() {
  curr = curr.unixtime() + diff;
  if(refreshDisplay || tmpMin != curr.minute()) {
    tmpMin = curr.minute();
    int8_t tmpHour = curr.hour();
    if(clockFormat12) {
      boolean isAM = (curr.hour() < 12);
      if(curr.hour() == 0) {
        tmpHour = 12;
      } 
      else if(curr.hour() > 12) {
        tmpHour = curr.hour() - 12;
      }
      sprintf(LINES[0],"%02d%s %02d:%02d %s   ",curr.day(),MONTH_SHORT[(curr.month()-1)],tmpHour,curr.minute(), (isAM?"AM":"PM"));
    } else {
      sprintf(LINES[0],"%02d%s%d %02d:%02d ",curr.day(),MONTH_SHORT[(curr.month()-1)],curr.year(),tmpHour,curr.minute());
    }
    sprintf(LINES[1],"%s",TIMEZONE_NAME);
    //curr.dayOfTheWeek() -- to be tested
    refreshDisplay = true;
  }
}

void printLines() {
  printLine(0);
  printLine(1);
}

void printLine(uint8_t line) {
  lcd.setCursor(0, line);
  int len = strlen(LINES[line]);
  if (len == 16) {
    //do nothing
  } else if(len < 16) {
    for (int i = len; i < 16; i++)
      strcat(LINES[line], " ");
  } 
  lcd.print(LINES[line]);
}
