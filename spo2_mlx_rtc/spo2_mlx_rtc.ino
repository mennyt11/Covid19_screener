#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#include <Adafruit_MLX90614.h>
#include <DS3231.h>

DS3231  rtc(SDA, SCL);

Adafruit_MLX90614 mlx = Adafruit_MLX90614();

#define REPORTING_PERIOD_MS     1000

PulseOximeter pox;

uint32_t tsLastReport = 0;
int LED=13;
int LEDG=11;
int LEDUV=10;
Time t;
String time_str;
String rasp;

void onBeatDetected()
{
    //Serial.println("beat");
    digitalWrite(LED,!digitalRead(LED));
}

void setup()
{
    Serial.begin(9600);

   // Serial.print("Initializing pulse oximeter..");
    pinMode(LEDG,OUTPUT);
    
    if (!pox.begin()) {
        //Serial.println("FAILED");
        for(;;);
    } else {
        //Serial.println("SUCCESS");
    }
    pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);

    
    pox.setOnBeatDetectedCallback(onBeatDetected);
  //  Serial.println("Adafruit MLX90614 test");  
  rtc.begin();
  mlx.begin();  
}

int count,g;
int ldr;

void loop()
{
    
    
    pox.update();

    ldr=analogRead(A1);
    // Make sure to call update as fast as possible
    
    if(ldr<40){
      //digitalWrite(LED,HIGH);
    }
    else{
      //digitalWrite(LED,LOW);
    }
    
    if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
if(pox.getSpO2()>60 && pox.getSpO2()<101)
      {
        if(g==1)
        {
        count=count+1;
        }
        
        
        
        if(count==3 && g==1)
        {
        t = rtc.getTime();
        time_str= rtc.getTimeStr();
        char date_str[30];
        sprintf(date_str,"%04u-%02u-%02u",t.year,t.mon,t.date);

        rasp="<" + String(date_str) + "," + time_str + "," + String(pox.getSpO2()) + "," + String(mlx.readObjectTempC()) +">";
        Serial.println(rasp);
        digitalWrite(LEDG,HIGH);
        
        count=0;   
        g=0;
        }
        tsLastReport = millis();
      
      }
      else {g=1; count=0; digitalWrite(LEDG,LOW);}
    }
}
