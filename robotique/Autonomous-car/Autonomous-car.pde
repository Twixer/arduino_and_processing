// ************************************************************************************
//
// An arduino scketch in order to play with the adafruit motor shield v1 and a
// ping sensor. An RC car has been modified, the RC controller has been
// removed and replaced by an arduino.
//
// ************************************************************************************

#include <AFMotor.h>


AF_DCMotor motorForwBack(2, MOTOR12_64KHZ);
AF_DCMotor motorLeftRight(3, MOTOR12_64KHZ);

#define echoPin 14
#define trigPin 15
#define buzzer 16
#define BAUDS 9600
#define DISTANCE 15
#define MAX_SPEED 200
#define MID_SPEED 200

int sound = 250;

void setup() {
	Serial.begin(BAUDS);
	Serial.println("---- Initialization ----");

  // turn on motorForwBack
  motorForwBack.setSpeed(MAX_SPEED);
  motorForwBack.run(RELEASE);

  // turn on motorLeftRight
  motorLeftRight.setSpeed(MAX_SPEED);
  motorLeftRight.run(RELEASE);


  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  // pinMode(buzzer, OUTPUT);

  // tone(buzzer, 220);
  // delay(100);
  // noTone(buzzer);

  randomSeed(analogRead(5));
  Serial.println("Ending setup.");
}

void loop() {

	Serial.println("Starting the main loop ...");
  bool isAvoiding = false;
  bool isGoingForward = true;

  // Comment or uncomment this line in order to test the motor.
  // testMotor();

  // if objects are less than 30cm away the car goes back
  if (isObstacle()) {

  	Serial.println("Obstacle to avoid ...");

    isGoingForward = false;

    if (!isAvoiding) {
      //brake drive motor and pause 1/10 second      
      motorForwBack.run(RELEASE);
      delay(100);
    }

    //
    // Going back
    //

    // set turn motor direction randomly to LEFT or RIGHT
    if (!isAvoiding) {      
      motorLeftRight.setSpeed(MAX_SPEED);
      int left_or_right = random(2);
      left_or_right = 1;
      motorLeftRight.run((left_or_right == 1 ? FORWARD : BACKWARD));
    }

    motorForwBack.setSpeed(MID_SPEED);
    motorForwBack.run(BACKWARD); 

    delay(1000);

    //
    // Stopping
    //

    //brake both motors
    motorForwBack.run(RELEASE);

    isAvoiding = true;
    } else {
     Serial.println("Nothing on the road, keep going ...");

     // No more turning.
     motorLeftRight.run(RELEASE);

    // Going forward more and more quickly
    motorForwBack.run(FORWARD);
    
    // If the car starts to go forward, the speed is increased step by step
    if (!isGoingForward) {
      for (uint8_t i = 0; i < MAX_SPEED; i++) {
        motorForwBack.setSpeed(i);
        delay(10);
      }
    }

  }

  //delay(100);

  Serial.println("Ending the main loop.");
}


bool isObstacle() {
	bool tooShort = false;

  // Variables for ping calculation
  long duration, distance;
  
  // Is there something in front of the car ?
  digitalWrite(trigPin, LOW); 
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = microsecondsToCentimeters(duration);

  Serial.print("Distance : ");
  Serial.println(distance);

  if (distance <= DISTANCE) {
   tooShort = true;
 }

 if (distance <= 30) {
   sound = 250;
 }

 if (distance < 25) {
   sound = 260;
 }

 if (distance < 20) {
   sound = 270;
 } 

 if (distance < 15) {
   sound = 280;  	
 }

 if (distance < 10) {
   sound = 290;
 }

 if (distance < 5) {
   sound = 300;
 }

 // tone(buzzer, sound);
 // delay(10);
 // noTone(buzzer);

 return tooShort;
}

long microsecondsToCentimeters(long microseconds)
{
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the
  // object we take half of the distance travelled.
  return microseconds / 29 / 2;
}

void testMotor() {
  motorForwBack.run(FORWARD);
  for (int i = 0; i < 255; i++) {
    motorForwBack.setSpeed(i);
    delay(10);
  }
  
  motorLeftRight.run(BACKWARD);
  
  delay(20000);

  motorForwBack.run(RELEASE);
  motorLeftRight.run(RELEASE);
  delay(1000);
}