// SERIAL LOOP SKETCH FOR COMMUNICATING WITH A BLUETOOTH SPP MODULE

// BASE HC-05 Config Commands

// AT+ORGL （Restore the factory default state）
// AT+UART=115200,0,0 （Set transmission rate to 115200 bps, one stop bit and no parity bit）
// AT+NAME=Smoothie
// AT+PSWD=0000 Set pairing code to 0000

#include <SoftwareSerial.h>
#define rxPin 10
#define txPin 11
SoftwareSerial mySerial(rxPin, txPin); // RX, TX
char myChar ;
void setup() {
  Serial.begin(9600);   
  Serial.println("AT");
  mySerial.begin(38400);
  mySerial.println("AT");
}
void loop() {
  while (mySerial.available()) {
    myChar = mySerial.read();
    Serial.print(myChar);
  }
 while (Serial.available()) {
    myChar = Serial.read();
    Serial.print(myChar); //echo
    mySerial.print(myChar);
  }
}
