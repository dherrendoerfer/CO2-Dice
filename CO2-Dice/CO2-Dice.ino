/*
 * Simple tool to measure the CO2 concentration in a room to 
 *                               know when it's time to open the Windows
 * Copyright (C) 2020 D.Herrendoerfer 
 *                    <d.herrendoerfer@herrendoerfer.name>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 ************************************************************************/

#include <FastLED.h>
#include <Wire.h>
#include "SparkFun_SCD30_Arduino_Library.h"

#define NUM_LEDS 9
#define DATA_PIN 2

SCD30 airSensor;

CRGB leds[NUM_LEDS];

//                    LEDs 0 to 9 (intensity 1-3), and blink   
uint8_t dice[8][10] = {0,0,0,0,0,0,0,0,0,0, // No LEDs on
                      0,0,0,0,0,0,0,0,1,0,  // 1
                      0,0,1,0,0,0,1,0,0,0,  // 2
                      0,0,1,0,0,0,1,0,1,0,  // 3
                      2,0,2,0,2,0,2,0,0,0,  // 4
                      2,0,2,0,2,0,2,0,2,1,  // 5 + blink
                      2,2,2,0,2,2,2,0,0,1,  // 6 + blink
                      3,3,3,3,3,3,3,3,3,1 };
uint8_t co2_now = 0;
uint8_t blink = 0;
uint8_t count = 0;
uint8_t error = 0;
uint32_t next_ct = 0;

uint8_t diceLED(uint8_t num)
{
  if (num>7)
    num=7;
    
  for( int i; i<9 ;i++)
    if (dice[num][i])
      leds[i]=CRGB(0,64 * dice[num][i],0);
    else
      leds[i]=CRGB(0,0,0);

  return dice[num][9];
}

void setup()
{
  Serial.begin(115200);
  Wire.begin();

  Serial.println();

  if (!airSensor.begin()) {
    Serial.println("No SCD30 found ....");
    error = 1;
  }

  FastLED.addLeds<WS2812B, DATA_PIN, RGB>(leds, NUM_LEDS);
   
  Serial.println("CO2,TEMP,HUMIDITY");

  diceLED(0);
  FastLED.show();
}

void loop()
{

  // Simple Error display (just flash the center LED slow)
  while (error)
  {
     diceLED(error);
     FastLED.show();
     delay(1000);
     diceLED(0);
     FastLED.show();
     delay(1000);
  }

  if (airSensor.dataAvailable())
  {
    //Serial log
    Serial.print((float)airSensor.getCO2(), 2);
    Serial.print(",");
    Serial.print(airSensor.getTemperature(), 1);
    Serial.print(",");
    Serial.print(airSensor.getHumidity(), 1);
    Serial.println();

    co2_now = airSensor.getCO2()/200.0;
  }


  if (micros() > next_ct) {
    next_ct = micros() + 250000;
    if (count++ % 2 == 0) {
        blink = diceLED(co2_now);
        FastLED.show();
    }
    else {
      if (blink) {
        diceLED(0);
        FastLED.show();
      }
    }
  }
}
