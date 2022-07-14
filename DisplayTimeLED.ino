/** @file DisplayTimeLED
 *  @brief Arduino LED Backpack time display
 */
#include <Wire.h>
#include <DS1307RTC.h>
#include <Time.h>
#include "Adafruit_LEDBackpack.h"

#define SERIALOUT 1

//Object Instantiation
Adafruit_AlphaNum4 alpha4 = Adafruit_AlphaNum4();

// Global Data
// Data Time Data
const char *monthName[12] = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Now","Dec"};
tmElements_t tm;
char displaybuffer[10] = {0};


/** @brief Arduino main set up
 *  @author Wenyuan Yu
 *  @date 10_July_2022
 *  @param None
 *  @return void
 */
void setup() {
  bool parse=false;
  bool config=false;

  if(getDate(__DATE__) && getTime(__TIME__)){
    parse = true;

    if(RTC.write(tm)){
    config=true;
    }
  }

  Serial.begin(9600);
  while(!Serial);
  if(parse && config){
    Serial.print("Current Time:");
    Serial.print(__TIME__);
    Serial.print(", Date:");
    Serial.println(__DATE__);
  }else if(parse){
    Serial.println("Communication Error");
  }else{
    Serial.print("Could not parse info from the compiler, Time = \"");
    Serial.print(__TIME__);
    Serial.print("\",Date=\"");
    Serial.print(__DATE__);
    Serial.println("\"");
  }

  alpha4.begin(0x70);
  alpha4.clear();
  alpha4.writeDisplay();
}

/** @brief Arduino main control loop
 *  @author Paul Moggach
 *  @date 25MAR2019
 *  @param None
 *  @return void
 */
void loop() {
  RTC.read(tm);
  sprintf(displaybuffer,"%2d%02d%02d",tm.Hour,tm.Minute,tm.Second);
#if SERIALOUT 
  Serial.println();
  Serial.print(displaybuffer);
#endif
  alpha4.writeDigitAscii(0,displaybuffer[0]);
  alpha4.writeDigitAscii(1, displaybuffer[1]);
  alpha4.writeDigitAscii(2, displaybuffer[2]);
  alpha4.writeDigitAscii(3, displaybuffer[3]);  
  // write it out!
  alpha4.writeDisplay();
  delay(1000);
}

bool getTime(const char *str){
  int Hour, Min, Sec;

  if (sscanf(str, "%d:%d:%d", &Hour, &Min, &Sec) != 3) return false;
  tm.Hour = Hour;
  tm.Minute = Min;
  tm.Second = Sec;
  return true;
}

bool getDate(const char *str)
{
  char Month[12];
  int Day, Year;
  uint8_t monthIndex;

  if (sscanf(str, "%s %d %d", Month, &Day, &Year) != 3){ return false; }
  for (monthIndex = 0; monthIndex < 12; monthIndex++) 
  {
    if (strcmp(Month, monthName[monthIndex]) == 0) break;
  }
  if (monthIndex >= 12) return false;
  tm.Day = Day;
  tm.Month = monthIndex + 1;
  tm.Year = CalendarYrToTm(Year);
  return true;
}
