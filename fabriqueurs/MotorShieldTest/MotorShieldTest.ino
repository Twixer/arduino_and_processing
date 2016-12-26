// Adafruit Motor shield library
// copyright Adafruit Industries LLC, 2009
// this code is public domain, enjoy!

#include <AFMotor.h>


AF_Stepper motorX(20, 1);
AF_Stepper motorY(20, 2);

void setup() {
  Serial.begin(9600);           // set up Serial library at 9600 bps
  Serial.println("Stepper test!");

  motorX.setSpeed(100);  // 10 rpm   
  motorY.setSpeed(100);  // 10 rpm   

  motorX.step(20, FORWARD, SINGLE); 
  motorY.step(20, FORWARD, SINGLE); 
  
  motorX.release();
  motorY.release();
  
  delay(1000);
}

void loop() {
  motorX.step(200, FORWARD, MICROSTEP); 
  motorX.step(200, BACKWARD, MICROSTEP); 
  
  motorY.step(200, FORWARD, MICROSTEP); 
  motorY.step(200, BACKWARD, MICROSTEP); 
  
//  motorY.step(100, FORWARD, SINGLE); 
//  motorY.step(100, BACKWARD, SINGLE); 
//
//  motorY.step(100, FORWARD, DOUBLE); 
//  motorY.step(100, BACKWARD, DOUBLE);
//
//  motorY.step(100, FORWARD, INTERLEAVE); 
//  motorY.step(100, BACKWARD, INTERLEAVE); 
//
//  motorY.step(100, FORWARD, MICROSTEP); 
//  motorY.step(100, BACKWARD, MICROSTEP);
}

