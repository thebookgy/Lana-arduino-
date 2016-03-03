
   // #define SD_CS_PIN SS
    #include <SPI.h>
    #include <SD.h>
    //#include <SdFat.h>
    //SdFat SD;
    
    File myFile;
    const int chipSelect = 4;
    String sdfilename0;
    String sdfilename; 
    int LED= 12;  
    char input;  
        //  Variables
    int pulsePin = 0;                 // Pulse Sensor purple wire connected to analog pin 0
    int blinkPin = 13;                // pin to blink led at each beat
    int fadePin = 5;                  // pin to do fancy classy fading blink at each beat
    int fadeRate = 0;                 // used to fade LED on with PWM on fadePin
    
    // Volatile Variables, used in the interrupt service routine!
    volatile int BPM;                   // int that holds raw Analog in 0. updated every 2mS
    volatile int Signal;                // holds the incoming raw data
    volatile int IBI = 600;             // int that holds the time interval between beats! Must be seeded! 
    volatile boolean Pulse = false;     // "True" when User's live heartbeat is detected. "False" when not a "live beat". 
    volatile boolean QS = false;        // becomes true when Arduoino finds a beat.
    
    // Regards Serial OutPut  -- Set This Up to your needs
    static boolean serialVisual = true;   // Set to 'false' by Default.  Re-set to 'true' to see Arduino Serial Monitor ASCII Visual Pulse 
      
    void setup() 
    {  

      pinMode(blinkPin,OUTPUT);         // pin that will blink to your heartbeat!
      pinMode(fadePin,OUTPUT);          // pin that will fade to your heartbeat!
      Serial.begin(9600);             // we agree to talk fast!
      pinMode(LED, OUTPUT);  
      Serial.println(">> START<<");  
      sdfilename1 = "test01";
      sdfilename = sdfilename0+".txt" ;
      
      interruptSetup();                 // sets up to read Pulse Sensor signal every 2mS 
       // IF YOU ARE POWERING The Pulse Sensor AT VOLTAGE LESS THAN THE BOARD VOLTAGE, 
       // UN-COMMENT THE NEXT LINE AND APPLY THAT VOLTAGE TO THE A-REF PIN
    //   analogReference(EXTERNAL);   
      
      
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
       bt(BPM);
       
    }  

/* Pulse Function *****************/ 
    void pul(){
    serialOutput() ;       
    
    if (QS == true){     // A Heartbeat Was Found
                         // BPM and IBI have been Determined
                         // Quantified Self "QS" true when arduino finds a heartbeat
          digitalWrite(blinkPin,HIGH);     // Blink LED, we got a beat. 
          fadeRate = 255;         // Makes the LED Fade Effect Happen
                                  // Set 'fadeRate' Variable to 255 to fade LED with pulse
          serialOutputWhenBeatHappens();   // A Beat Happened, Output that to serial.     
          QS = false;                      // reset the Quantified Self flag for next time    
    }
       
    ledFadeToBeat();                      // Makes the LED Fade Effect Happen 
    delay(20);                             //  take a break
  }
/* LED Alert(Beat) *****************/ 
  void ledFadeToBeat(){
      fadeRate -= 15;                         //  set LED fade value
      fadeRate = constrain(fadeRate,0,255);   //  keep LED fade value from going into negative numbers!
      analogWrite(fadePin,fadeRate);          //  fade LED
    }

/* Bluetooth *****************/ 
   void bt(int BPM){
      if(Serial.available()>0)  
      {  
        input= Serial.read(); 
        if(input=='1')  
        {  
          Serial.println("ON BLUETOOTH");  
          Serial.println("=========================");
          sd(sdfilename,BPM); 
          digitalWrite(LED, HIGH);  
          delay(2000);  
         Serial.println("BMP "+String(BPM));
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
  
  // if open file has success, write data
  if (myFile) {
    Serial.print("Writing to "+String(sdfilename));
    myFile.println("data : "+String(BPM)); //write data
    myFile.close(); // close file
    Serial.println(" done.");
  } else {
    Serial.println("error opening"+String(sdfilename));
  }
  
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
    Serial.println("=========================");
}

/* Custom filename to sdcard ***************/
void saveToSdCard(String sdfilename){
String FileName = sdfilename;
myFile = SD.open(FileName, FILE_WRITE);
}

     
