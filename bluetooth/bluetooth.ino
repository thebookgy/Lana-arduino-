

    int LED= 12;  
    char input;  
      
    void setup() 
    {  
      Serial.begin(9600);  
      pinMode(LED, OUTPUT);  
      Serial.println(">> START<<");  
    }  
      
    void loop() 
    {  
      if(Serial.available()>0)  
      {  
        input= Serial.read(); 
        if(input=='1')  
        {  
          Serial.println("ON");  
          digitalWrite(LED, HIGH);  
          delay(2000);  
         Serial.println("Communication from Bluetooth");
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
