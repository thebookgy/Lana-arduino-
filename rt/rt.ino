#include "Wire.h"
#define DS1307_I2C_ADDRESS 0x68 // the I2C address of Tiny RTC
byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
// Convert normal decimal numbers to binary coded decimal
byte decToBcd(byte val)
{
return ( (val/10*16) + (val%10) );
}
// Convert binary coded decimal to normal decimal numbers
byte bcdToDec(byte val)
{
return ( (val/16*10) + (val%16) );
}
// Function to set the currnt time, change the second&minute&hour to the right time

String txt_date,txt_time;

void setDateDs1307()
{
second =0;
minute = 15;
hour = 20;
dayOfWeek = 2;
dayOfMonth =17;
month =3;
year= 16;
Wire.beginTransmission(DS1307_I2C_ADDRESS);
Wire.write(decToBcd(0));
Wire.write(decToBcd(second)); // 0 to bit 7 starts the clock
Wire.write(decToBcd(minute));
Wire.write(decToBcd(hour)); // If you want 12 hour am/pm you need to set
// bit 6 (also need to change readDateDs1307)
Wire.write(decToBcd(dayOfWeek));
Wire.write(decToBcd(dayOfMonth));
Wire.write(decToBcd(month));
Wire.write(decToBcd(year));
Wire.endTransmission();
}
// Function to gets the date and time from the ds1307 and prints result
void getDateDs1307()
{
// Reset the register pointer
Wire.beginTransmission(DS1307_I2C_ADDRESS);
Wire.write(decToBcd(0));
Wire.endTransmission();
Wire.requestFrom(DS1307_I2C_ADDRESS, 7);
second = bcdToDec(Wire.read() & 0x7f);
minute = bcdToDec(Wire.read());
hour = bcdToDec(Wire.read() & 0x3f); // Need to change this if 12 hour am/pm
dayOfWeek = bcdToDec(Wire.read());
dayOfMonth = bcdToDec(Wire.read());
month = bcdToDec(Wire.read());
year = bcdToDec(Wire.read());
txt_date=String(dayOfMonth,DEC)+"/"+String(month, DEC)+"/"+String(year, DEC);
txt_time=String(hour,DEC)+":"+String(minute, DEC)+":"+String(second, DEC);
Serial.println();

//Serial.print(hour, DEC);
Serial.print(" Date: "+txt_date);
Serial.print("|");
Serial.print(" Time: "+txt_time);
Serial.println();
/*
Serial.println();
Serial.print(hour,DEC);
Serial.print(":");
Serial.print(minute, DEC);
Serial.print(":");
Serial.print(second, DEC);
Serial.print("|");
Serial.print(dayOfMonth, DEC);
Serial.print("/");
Serial.print(month, DEC);
Serial.print("/");
Serial.print(year,DEC);
Serial.print(" ");
Serial.println();
*/
Serial.println();
delay(2000);
//Serial.print("Day of week:");
}
void setup() {
Wire.begin();
Serial.begin(9600);
setDateDs1307(); //Set current time;
}
void loop()
{
delay(2000);
getDateDs1307();//get the time data from tiny RTC
}
