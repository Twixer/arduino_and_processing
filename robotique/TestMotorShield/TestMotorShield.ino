// Adafruit Motor shield library
// copyright Adafruit Industries LLC, 2009
// this code is public domain, enjoy!

#include <AFMotor.h>

AF_DCMotor motorForwBack(2, MOTOR12_64KHZ);
AF_DCMotor motorLeftRight(3, MOTOR12_64KHZ);


void setup() {
  Serial.begin(9600);           // set up Serial library at 9600 bps
  Serial.println("---- Motor test ----");

  // turn on motorForwBack
  motorForwBack.setSpeed(255);
  motorForwBack.run(RELEASE);

  // turn on motorLeftRight
  motorLeftRight.setSpeed(255);
  motorLeftRight.run(RELEASE);

  randomSeed(analogRead(5));
}

void loop() {
  uint8_t i;

  Serial.println("starting ...");

  // motorForwBack.run(FORWARD);
  // for (i = 0; i < 255; i++) {
  //   motorForwBack.setSpeed(i);
  //   delay(10);
  // }
  
  int left_or_right = random(2);
  Serial.println(left_or_right);
  motorLeftRight.run((left_or_right == 1 ? FORWARD : BACKWARD));
  // motorLeftRight.run(BACKWARD);
  
  delay(1000);

  //  motorForwBack.run(FORWARD);
  //  for (i=0; i<255; i++) {
  //    motorForwBack.setSpeed(i);
  //    delay(10);
  // }
  //
  //  motorLeftRight.run(FORWARD);
  //  delay(2000);
  //  motorLeftRight.run(RELEASE);
  //
  //  for (i=255; i!=0; i--) {
  //    motorForwBack.setSpeed(i);
  //    delay(10);
  // }
  //
  //  Serial.print("tock");
  //
  //  motorLeftRight.run(BACKWARD);
  //  delay(2000);
  //  motorLeftRight.run(RELEASE);
  //
  //  //motorForwBack.run(BACKWARD);
  //  for (i=0; i<255; i++) {
  //    motorForwBack.setSpeed(i);
  //    delay(10);
  // }
  //
  //  for (i=255; i!=0; i--) {
  //    motorForwBack.setSpeed(i);
  //    delay(10);
  // }
  //

  Serial.println("ending.");
  motorForwBack.run(RELEASE);
  motorLeftRight.run(RELEASE);
  delay(1000);
}
