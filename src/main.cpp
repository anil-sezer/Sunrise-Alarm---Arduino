#include <Arduino.h>
#include <TM1637.h>
#include "Wire.h"


#define DS3231_I2C_ADDRESS 0x68

int TEST_DELAY = 5000;
// DISPLAY

// Pin 3 - > DIO
// Pin 2 - > CLK
TM1637 tm(2, 3);
// 1 2 3 4 5 6 7 8 9 10(a) 11(b) 12(c) 13(d) 14(e) 15(f)
// ******************************

// RELAY
int relay = 8;
volatile byte relayState = LOW;

// ******************************

byte decToBcd(byte val);
byte bcdToDec(byte val);
void setDS3231time(byte hour, byte minute);
void readDS3231time(byte *second, byte *minute, byte *hour);
void timeToSerialMonitor(byte second, byte minute, byte hour);
void timeToLedDisplay(byte hr, byte min);

void setup()
{
  Wire.begin();
  Serial.begin(9600);
  tm.init();                              // Initializes the display
  tm.setBrightness(1);                    // Set brightness to level 3
  tm.display("0000", false, false);


  // set the initial time here:
  // seconds, minutes, hours
  setDS3231time(1,8);
}

void loop()
{
  byte second, minute, hour;
  // retrieve data from DS3231
  readDS3231time(&second, &minute, &hour);

  timeToSerialMonitor(second, minute, hour); // display the real-time clock data on the Serial Monitor
  timeToLedDisplay(hour, minute);
  delay(1000);
}

// Convert normal decimal numbers to binary coded decimal
byte decToBcd(byte val)
{
  return( (val/10*16) + (val%10) );
}
// Convert binary coded decimal to normal decimal numbers
byte bcdToDec(byte val)
{
  return( (val/16*10) + (val%16) );
}

void setDS3231time(byte hour, byte minute)
{
  // sets time and date data to DS3231
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0); // set next input to start at the seconds register
  Wire.write(decToBcd(00)); // set seconds
  Wire.write(decToBcd(minute)); // set minutes
  Wire.write(decToBcd(hour)); // set hours

  // Wire.write(decToBcd(dayOfWeek)); // set day of week (1=Sunday, 7=Saturday)
  // Wire.write(decToBcd(dayOfMonth)); // set date (1 to 31)
  // Wire.write(decToBcd(month)); // set month
  // Wire.write(decToBcd(year)); // set year (0 to 99)
  Wire.endTransmission();
}

void readDS3231time(byte *second, byte *minute, byte *hour)
{
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0); // set DS3231 register pointer to 00h
  Wire.endTransmission();
  Wire.requestFrom(DS3231_I2C_ADDRESS, 7);
  // request seven bytes of data from DS3231 starting from register 00h
  *second = bcdToDec(Wire.read() & 0x7f);
  *minute = bcdToDec(Wire.read());
  *hour = bcdToDec(Wire.read() & 0x3f);
}

void timeToSerialMonitor(byte second, byte minute, byte hour)
{
  // send it to the serial monitor
  Serial.print(hour, DEC);
  // convert the byte variable to a decimal number when displayed
  Serial.print(":");
  if (minute<10)
  {
    Serial.print("0");
  }
  Serial.print(minute, DEC);
  Serial.print(":");
  if (second<10)
  {
    Serial.print("0");
  }
  Serial.println(second, DEC);
}

// 1 2 3 4 5 6 7 8 9 10(a) 11(b) 12(c) 13(d) 14(e) 15(f)
void timeToLedDisplay(byte hr, byte min)
{
  // Serial.println(hr, DEC);
  char hrStr[2];
  String(hr, DEC).toCharArray(hrStr, 2);
  if (hrStr[0] == 0)
  {
    hrStr[1] = '0';
  }
  if (hrStr[1] == NULL)
  {
    char c = hrStr[0];
    hrStr[0] = '0';
    hrStr[1] = c;
  }


  char minStr[2];
  String(min, DEC).toCharArray(minStr, 2);
  if (minStr[0] == 0)
  {
    minStr[1] = '0';
  }
  if (minStr[1] == NULL)
  {
    char c = minStr[0];
    minStr[0] = '0';
    minStr[1] = c;
  }

  char hrWithMin[4] = {hrStr[0], hrStr[1], minStr[0], minStr[1]};


  tm.display((String)hrWithMin, false);
}