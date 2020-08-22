/*
Arduino-MAX30100 oximetry / heart rate integrated sensor library
Copyright (C) 2016  OXullo Intersecans <x@brainrapers.org>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <Adafruit_MLX90614.h>
#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#include <DS3231.h>

DS3231  rtc(SDA, SCL);

Adafruit_MLX90614 mlx = Adafruit_MLX90614();

#define REPORTING_PERIOD_MS     1000
#define MEASURING_PERIOD_MS     5000

// PulseOximeter is the higher level interface to the sensor
// it offers:
//  * beat detection reporting
//  * heart rate calculation
//  * SpO2 (oxidation level) calculation
PulseOximeter pox;

uint32_t tsLastReport = 0, t_first_val=0;
int LED=13;
int LEDG=12;
// Callback (registered below) fired when a pulse is detected

String date_str,time_str;

Time t;

void onBeatDetected()
{
    //Serial.println("beat");
    digitalWrite(LED,!digitalRead(LED));
    }

void setup()
{
    Serial.begin(9600);
    rtc.begin();
     
    mlx.begin();  
    //Serial.print("Initializing pulse oximeter..");
    

    //rtc.setDOW(WEDNESDAY);     // Set Day-of-Week to SUNDAY
    //rtc.setTime(11, 54, 0);     // Set the time to 12:00:00 (24hr format)
    //rtc.setDate(5,8, 2020);   // Set the date to January 1st, 2014


    // Initialize the PulseOximeter instance
    // Failures are generally due to an improper I2C wiring, missing power supply
    // or wrong target chip
    if (!pox.begin()) {
        //Serial.println("FAILED");

        for(;;);
    } else {
        //Serial.println("SUCCESS");
    }
    pinMode(LEDG,OUTPUT);
    // The default current for the IR LED is 50mA and it could be changed
    //   by uncommenting the following line. Check MAX30100_Registers.h for all the
    //   available options.
    pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);

    // Register a callback for the beat detection
    pox.setOnBeatDetectedCallback(onBeatDetected);
    
}
int f=0,g=0;
float tempc;
int ldr;
int sp,bpm;
int c;
String rasp="";

void loop()
{

    
    
    ldr=analogRead(A1);
    // Make sure to call update as fast as possible
    if(ldr<40){
      //digitalWrite(LED,HIGH);
    }
    else{
      //digitalWrite(LED,LOW);
    }
    //Serial.print(".");
    pox.update();

    // Asynchronously dump heart rate and oxidation levels to the serial
    // For both, a value of 0 means "invalid"
    if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
      //Serial.println(pox.getSpO2());
      if(pox.getSpO2()>60 && pox.getSpO2()<101)
      {
        t = rtc.getTime();
        time_str= rtc.getTimeStr();
        
        if(g==1)
        {
        c=c+1;
        }
        //Serial.println(c);
        if(c==3)
        {f=1;}

        if(f==1 && g==1){
          bpm=pox.getHeartRate();
        sp=pox.getSpO2();
//        Serial.print("date:");
//        Serial.print(date_str);
//        Serial.print(" time:");
//        Serial.print(time_str);
//        Serial.print(" Heart rate:");
//        Serial.print(bpm);
//        Serial.print("bpm / SpO2:");
        
//        Serial.print(sp);
//        Serial.print("%");
//        Serial.print("  TEMP:");
//        Serial.print(tempc);
//        Serial.println("°C");
           Serial.print("Ambient = "); Serial.print(mlx.readAmbientTempC()); 
  Serial.print("*C\tObject = "); Serial.print(mlx.readObjectTempC()); Serial.println("*C");
  Serial.print("Ambient = "); Serial.print(mlx.readAmbientTempF()); 
  Serial.print("*F\tObject = "); Serial.print(mlx.readObjectTempF()); Serial.println("*F");
        //Serial.println(String(t));
        char date_str[30];
        sprintf(date_str,"%04u-%02u-%02u",t.year,t.mon,t.date);
        rasp="<" + String(date_str) + "," + time_str + "," + sp + "," + String(mlx.readObjectTempC()) +">";
        Serial.println(rasp);
        digitalWrite(LEDG,HIGH);
        c=0; 
        f=0;  
        g=0;
        }
        tsLastReport = millis();
    }
    else{g=1; c=0; digitalWrite(LEDG,LOW);}
    }
}
