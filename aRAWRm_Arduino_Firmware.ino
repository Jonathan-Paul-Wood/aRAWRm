/*
 * a-Rawrm
 * HackPrinceton Fall 2016
 * Team Members: Mark Seda, Sarah Strobel, Jonathan Wood
 * 
 * Within a plush toy is contained an Arduino 101 and supporting hardware.
 * This plush is an alarm clock (sound) as well as an RGB led.
 * In the future vibration alarm and sleep tracking could be added.
 * 
 * It communicates to an Android app using Bluetooth LE. 
 * 
 * 
 */
#include <CurieBLE.h>

#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978

BLEPeripheral blePeripheral;  // BLE Peripheral Device (the board you're programming)
BLEService alarmService("19B10000-E8F2-537E-4F6C-D104768A1214"); // BLE Buzzer Service

// BLE LED Switch Characteristic - custom 128-bit UUID, read and writable by central
BLEUnsignedCharCharacteristic alarmCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);

const int buzzerPin = 6; // pins to use for the RGB LED and buzzer
const int blueLedPin = 9;
const int redLedPin = 3;
const int greenLedPin = 5;

int melody[] = {
  NOTE_E7, NOTE_E7, 0, NOTE_E7,
  0, NOTE_C7, NOTE_E7, 0,
  NOTE_G7, 0, 0,  0,
  NOTE_G6, 0, 0, 0,
 
  NOTE_C7, 0, 0, NOTE_G6,
  0, 0, NOTE_E6, 0,
  0, NOTE_A6, 0, NOTE_B6,
  0, NOTE_AS6, NOTE_A6, 0,
 
  NOTE_G6, NOTE_E7, NOTE_G7,
  NOTE_A7, 0, NOTE_F7, NOTE_G7,
  0, NOTE_E7, 0, NOTE_C7,
  NOTE_D7, NOTE_B6, 0, 0,
 
  NOTE_C7, 0, 0, NOTE_G6,
  0, 0, NOTE_E6, 0,
  0, NOTE_A6, 0, NOTE_B6,
  0, NOTE_AS6, NOTE_A6, 0,
 
  NOTE_G6, NOTE_E7, NOTE_G7,
  NOTE_A7, 0, NOTE_F7, NOTE_G7,
  0, NOTE_E7, 0, NOTE_C7,
  NOTE_D7, NOTE_B6, 0, 0
};

int noteDurations[] = {
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
 
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
 
  9, 9, 9,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
 
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
 
  9, 9, 9,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
};


void setup() {
  Serial.begin(9600);
  

  // set buzzer and LED pin to output mode
  pinMode(buzzerPin, OUTPUT);
  
  pinMode(greenLedPin, OUTPUT);
  pinMode(blueLedPin, OUTPUT);
  pinMode(redLedPin, OUTPUT);

  // set advertised local name and service UUID:
  blePeripheral.setLocalName("Plush666");
  blePeripheral.setAdvertisedServiceUuid(alarmService.uuid());

  // add service and characteristic:
  blePeripheral.addAttribute(alarmService);
  blePeripheral.addAttribute(alarmCharacteristic);

  // set the initial value for the characeristic:
  alarmCharacteristic.setValue(0);

  // begin advertising BLE service:
  blePeripheral.begin();

  Serial.println("Demon Plush Activated");
}

void loop() {
  // listen for BLE peripherals to connect:
  BLECentral central = blePeripheral.central();

  // if a central is connected to peripheral:
  if (central) {
    Serial.print("Connected to central: ");
    // print the central's MAC address:
    Serial.println(central.address());

    // while the central is still connected to peripheral:
    while (central.connected()) {
      // if the remote device wrote to the AlarmCharacteristic, act accordingly
      if (alarmCharacteristic.written()) {
        Serial.println(alarmCharacteristic.written());
        if (alarmCharacteristic.value() == 1) {   // 1 means start alarm    
          while(alarmCharacteristic.value() == 1){
            digitalWrite(blueLedPin, HIGH);
            digitalWrite(greenLedPin, LOW);
            tone(6, 440, 500);
            delay(500);
            digitalWrite(blueLedPin, LOW);
            digitalWrite(greenLedPin, HIGH);
            delay(500);
          }
        }
        if (alarmCharacteristic.value() == 2) {   // 2 means start soothing lights   
          while(alarmCharacteristic.value() == 2){
              for (int fadeValue = 0 ; fadeValue <= 255; fadeValue += 5) {
                // sets the value (range from 0 to 255):
                analogWrite(greenLedPin, fadeValue);
                // wait for 30 milliseconds to see the dimming effect
                delay(10);
              }
              for (int fadeValue = 0 ; fadeValue <= 255; fadeValue += 5) {
                // sets the value (range from 0 to 255):
                analogWrite(blueLedPin, fadeValue);
                // wait for 30 milliseconds to see the dimming effect
                delay(10);
              }
            
              // fade out from max to min in increments of 5 points:
              for (int fadeValue = 255 ; fadeValue >= 0; fadeValue -= 5) {
                // sets the value (range from 0 to 255):
                analogWrite(greenLedPin, fadeValue);
                // wait for 30 milliseconds to see the dimming effect
                delay(10);
              }
              for (int fadeValue = 255 ; fadeValue >= 0; fadeValue -= 5) {
                // sets the value (range from 0 to 255):
                analogWrite(redLedPin, fadeValue);
                // wait for 30 milliseconds to see the dimming effect
                delay(10);
                
              }
          }
          
        }
        if (alarmCharacteristic.value() == 3) {   // 2 means dance party  
          int thisNote = 0;
          while(alarmCharacteristic.value() == 3 and thisNote < 75){
 
                if(thisNote % 2){
                  digitalWrite(blueLedPin, HIGH);
                  digitalWrite(greenLedPin, LOW);
                }
                else{
                  digitalWrite(blueLedPin, LOW);
                  digitalWrite(greenLedPin, HIGH);
                }
                // to calculate the note duration, take one second
                // divided by the note type.
                //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
                int noteDuration = 1300 / noteDurations[thisNote];
                tone(6, melody[thisNote], noteDuration);
            
                // to distinguish the notes, set a minimum time between them.
                // the note's duration + 30% seems to work well:
                int pauseBetweenNotes = noteDuration * 1.40;
                delay(pauseBetweenNotes);
                // stop the tone playing:
                noTone(8);
                thisNote++;
                if(thisNote == 75){
                  thisNote = 0;
                }
              }
              
          }
        
        else {                              // a 0 value
          Serial.println(F("Alarm off"));
          digitalWrite(buzzerPin, LOW);          // will turn the LED off
          digitalWrite(redLedPin, LOW);
          digitalWrite(greenLedPin, LOW);
          digitalWrite(blueLedPin, LOW);
        
        }
      }
    }

    // when the central disconnects, print it out:
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
  }
}

/*
   Copyright (c) 2016 Intel Corporation.  All rights reserved.
   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.
   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.
   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/
