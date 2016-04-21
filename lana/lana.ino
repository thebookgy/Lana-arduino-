

    #include <SPI.h>
    #include <SD.h>
    #include "Wire.h"
    #include <SoftwareSerial.h>

    //SoftwareSerial bluetooth(7, 8); // RX, TX
    File myFile;
    const int chipSelect = 4;
    String sdfilename0;
    String sdfilename; 
    int LED= 12;  
    char input;
    int flaginput = 0;
        //  Variables
    int sumpulse = 0;
    int BPMcount = 0;
    int avgBPM = 0;
    int flagpul = 1;
    String minfirst ;
    int pulsePin = 0;                 // Pulse Sensor purple wire connected to analog pin 0
    int blinkPin = 13;                // pin to blink led at each beat
    int fadePin = 5;                  // pin to do fancy classy fading blink at each beat
    int fadeRate = 0;                 // used to fade LED on with PWM on fadePin
    
    // Volatile Variables, used in the interrupt service routine!
    volatile int BPM;                   // int that holds raw Analog in 0. updated every 2mS
    volatile int Signal;                // holds the incoming raw data
    volatile int Signal2; 
    volatile int IBI = 600;             // int that holds the time interval between beats! Must be seeded! 
    volatile boolean Pulse = false;     // "True" when User's live heartbeat is detected. "False" when not a "live beat". 
    volatile boolean QS = false;        // becomes true when Arduoino finds a beat.
    float voltage;
    float filterFrequency = 5.0; 
    float R = 0.000116343;  // or 0.003234; 0.002804; 0.026342059; 2912.114; 5921.87658; Measurement Noise
    float Q = 1e-8; // Process Noise
    float Pc = 0.0; // Variance of pre estimation state (before sensor got value)
    float G = 0.0;  // Kalman Gain
    float Pp = 1.0;  // Variance of previous step
    float Xp = 0.0; // Previous estimation of true state
    float Zp = 0.0; // Estimation of true state
    float Xe = 0.0; // Estimation from kalman filter (Result)    
    // Regards Serial OutPut  -- Set This Up to your needs
    static boolean serialVisual = true;   // Set to 'false' by Default.  Re-set to 'true' to see Arduino Serial Monitor ASCII Visual Pulse 

    //setting time
    
    String txt_date;
    String txt_dateformat;
    String txt_time;
    String txt_min;

    
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
    
    
    
    void setDateDs1307()
    {
      second =0;
      minute = 20;
      hour = 12;
      dayOfWeek = 4;
      dayOfMonth =12;
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
      txt_date=String(dayOfMonth,DEC)+"-"+String(month, DEC)+"-"+String(year, DEC);
      //txt_dateformat=String(dayOfMonth,DEC)+"\/"+String(month, DEC)+"\/"+String(year, DEC);
      //txt_time=String(hour,DEC)+":"+String(minute, DEC)+":"+String(second, DEC);
      txt_time=String(hour,DEC)+":"+String(minute, DEC);
      txt_min=String(minute, DEC);  
      
      
      //Serial.print(hour, DEC);
      /*
      Serial.print(" Date: "+txt_date);
      Serial.print("|");
      Serial.print(" Time: "+txt_time);
      Serial.println();
      */
      //delay(2000);
    }
    
      
    void setup() 
    {  
      
      Wire.begin();
      pinMode(blinkPin,OUTPUT);         // pin that will blink to your heartbeat!
      pinMode(fadePin,OUTPUT);          // pin that will fade to your heartbeat!
      Serial.begin(9600);             // we agree to talk fast!
      pinMode(LED, OUTPUT);  
      Serial.println(">> START<<");  
      setDateDs1307(); //Set current time;
     
      
      interruptSetup();                 // sets up to read Pulse Sensor signal every 2mS 
       // IF YOU ARE POWERING The Pulse Sensor AT VOLTAGE LESS THAN THE BOARD VOLTAGE, 
       // UN-COMMENT THE NEXT LINE AND APPLY THAT VOLTAGE TO THE A-REF PIN
    //   analogReference(EXTERNAL);   
      
    // bluetooth.begin(9600);

    }  
      
    void loop() 
    {  
      /*
       for(int i=0; i<=30000; i++){
        pul();
        delay(1000);//ms
       }
       */
       
       pul();
       bt(avgBPM);
       getDateDs1307();
       sdfilename0 = txt_date;
       sdfilename = sdfilename0+".txt" ;
       
    }  

/* Pulse Function *****************/ 
    void pul(){
      // Check pulse 1 minute
      if(flagpul == 1){
        minfirst = txt_min;
        flagpul = 0;
      }
      if(minfirst == txt_min){

      serialOutput() ;       
          if (QS == true){     // A Heartbeat Was Found
                         // BPM and IBI have been Determined
                         // Quantified Self "QS" true when arduino finds a heartbeat
          digitalWrite(blinkPin,HIGH);     // Blink LED, we got a beat. 
          fadeRate = 255;         // Makes the LED Fade Effect Happen
                                  // Set 'fadeRate' Variable to 255 to fade LED with pulse
          serialOutputWhenBeatHappens();   // A Beat Happened, Output that to serial.     
          QS = false;                      // reset the Quantified Self flag for next time    
          }else{
       // digitalWrite(blinkPin,LOW);            // There is not beat, turn off pin 13 LED
       // serialOutputWhenBeatHappens();   // A Beat Happened, Output that to serial.     
         //QS = false;                      // reset the Quantified Self flag for next time    
          }
       
      ledFadeToBeat();                      // Makes the LED Fade Effect Happen 
      delay(1000);                             //  take a break
      }else{
      Serial.println("==========off==========");
     // delay(3600000);
      // 1min=60000 / 1 hour = 3600000
      sd(sdfilename,avgBPM);
      flaginput = 0; 
      flagpul = 1;
      sumpulse = 0;
      BPMcount = 0;
      }
    
  }
/* LED Alert(Beat) *****************/ 
  void ledFadeToBeat(){
      fadeRate -= 15;                         //  set LED fade value
      fadeRate = constrain(fadeRate,0,255);   //  keep LED fade value from going into negative numbers!
      analogWrite(fadePin,fadeRate);          //  fade LED
    }

/* Bluetooth *****************/ 
   void bt(int avgBPM){
   // input = bluetooth.read(); 
    // bluetooth.println(String(avgBPM));
    // Keep reading from HC-05 and send to Arduino Serial Monitor
    /*
  if (bluetooth.available()){
    Serial.write(bluetooth.read());
  }

  // Keep reading from Arduino Serial Monitor and send to HC-05
  if (Serial.available()){
    bluetooth.write(Serial.read());
    
  }
  */


    /*
      if(Serial.available()>0)  
      {  
        input= Serial.read(); 
        if(input=='1')  
        {  
          Serial.println("ON BLUETOOTH");  
          Serial.println("=========================");
          sd(sdfilename,avgBPM); 
          digitalWrite(LED, HIGH);  
          delay(2000);  
         Serial.println("BMP "+String(avgBPM));
        }  
        else if(input=='0')  
        {  
          Serial.println("OFF");  
          digitalWrite(LED, LOW);  
          delay(2000);  
          Serial.println(input);
        }  
        else  
        {  
          Serial.println("NO INPUT");  
          Serial.println(input);  
        }  
      } 
      */

     }
     
/* SD Card Function *****************/ 
void sd(String sdfilename,int BPM){
     Serial.print("Initializing SD card...\n");
     pinMode(SS, OUTPUT);
   
      if (!SD.begin(chipSelect)) {
        Serial.println("initialization failed!");
        return;
      }
      Serial.println("initialization done.");
  
       myFile = SD.open(sdfilename, FILE_WRITE);
         Serial.println(String(sdfilename));
       //saveToSdCard(sdfilename);
  
  // if open file has success, write data
  if(flaginput == 0){
    if (myFile) {
   Serial.print("Writing to "+String(sdfilename));
    myFile.println(sdfilename+" - "+txt_time+" - "+String(BPM)); //write data
    myFile.close(); // close file
    Serial.println(" done.");
    } else {
      Serial.println("error opening "+String(sdfilename));
      }

  } else if(flaginput == 1){
        // Open file to read
        myFile = SD.open(sdfilename); // Open file
        if (myFile) {
          Serial.println(String(sdfilename)+":");
          // Read all data
          while (myFile.available()) {
          Serial.write(myFile.read());
          }
         myFile.close();
        } else {
          Serial.println("error opening "+String(sdfilename));
        }
         
  }
 
   Serial.println("=========================");  

}

/* Custom filename to sdcard ***************/
void saveToSdCard(String sdfilename){
String FileName = sdfilename;
myFile = SD.open(FileName, FILE_WRITE);
}

     
