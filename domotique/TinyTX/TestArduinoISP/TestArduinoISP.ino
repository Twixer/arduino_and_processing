// TestArduinoISP version 04m3
// Copyright (c) 2015-2018 Twixer
// If you require a license, see
//     http://www.opensource.org/licenses/bsd-license.php
//
// This sketch tests a ATTiny84 shield for Arduino UNO.
// This sketch is based on the pins of the ArduinoISP sketch.
//
// pin name all HIGH for testing :       
// slave reset: 10               
// MOSI:        11               
// MISO:        12               
// SCK:         13            
// Heartbeat    9
// Error        8
// Programming  7
//

#include "pins_arduino.h"
#define RESET     SS

#define LED_HB    9
#define LED_ERR   8
#define LED_PMODE 7
#define LED_RESET 10
#define LED_MOSI  11
#define LED_MISO  12
#define LED_SCK   13

void pulse(int pin, int times);

void setup() {
  Serial.begin(19200);
  pinMode(LED_PMODE, OUTPUT);
  pulse(LED_PMODE, 2);
  pinMode(LED_ERR, OUTPUT);
  pulse(LED_ERR, 2);
  pinMode(LED_HB, OUTPUT);
  pulse(LED_HB, 2);

  pinMode(LED_RESET, OUTPUT);
  pinMode(LED_MISO, OUTPUT);
  pinMode(LED_MOSI, OUTPUT);
  pinMode(LED_SCK, OUTPUT);
}

void loop(void) {

  digitalWrite(LED_PMODE, HIGH);
  digitalWrite(LED_ERR, HIGH);
  digitalWrite(LED_HB, HIGH);
  digitalWrite(LED_RESET, HIGH);
  digitalWrite(LED_MISO, HIGH);
  digitalWrite(LED_MOSI, HIGH);
  digitalWrite(LED_SCK, HIGH);
  
}

#define PTIME 30
void pulse(int pin, int times) {
  do {
    digitalWrite(pin, HIGH);
    delay(PTIME);
    digitalWrite(pin, LOW);
    delay(PTIME);
  }
  while (times--);
}



