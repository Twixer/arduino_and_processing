 /* Read Jostick
  * ------------
  *
  * Reads two analog pins that are supposed to be
  * connected to a jostick made of two potentiometers
  *
  * We send three bytes back to the comp: one header and two
  * with data as signed bytes, this will take the form:
  *     Jxy\r\n
  *
  * x and y are integers and sent in ASCII
  *
  * http://www.0j0.org | http://arduino.berlios.de
  * copyleft 2005 DojoDave for DojoCorp
  */

#include <Servo.h>


Servo myservo1;  // create servo object to control a servo
Servo myservo2;  // create servo object to control a servo


int potpin1 = 0;  // analog pin used to connect the potentiometer
int potpin2 = 0;  // analog pin used to connect the potentiometer
int val1;    // variable to read the value from the analog pin
int val2;    // variable to read the value from the analog pin


 int ledPin = 13;
 int joyPin1 = 0;                 // slider variable connecetd to analog pin 0
 int joyPin2 = 1;                 // slider variable connecetd to analog pin 1
 int value1 = 0;                  // variable to read the value from the analog pin 0
 int value2 = 0;                  // variable to read the value from the analog pin 1

 void setup() {
  pinMode(ledPin, OUTPUT);              // initializes digital pins 0 to 7 as outputs
  Serial.begin(9600);

   myservo1.attach(9);  // attaches the servo on pin 9 to the servo object
   myservo2.attach(10);  // attaches the servo on pin 9 to the servo object
 }

 int treatValue(int data) {
 // return (data * 9 / 1024) + 48;
  return data;
 }

 void loop() {
  // reads the value of the variable resistor
  value1 = analogRead(joyPin1);  
  // this small pause is needed between reading
  // analog pins, otherwise we get the same value twice
  delay(100);            
  // reads the value of the variable resistor
  value2 = analogRead(joyPin2);  

  digitalWrite(ledPin, HIGH);          
  delay(value1);
  digitalWrite(ledPin, LOW);
  delay(value2);
  //Serial.print('');
  Serial.print(treatValue(value1));
  Serial.print(' ');
  Serial.println(treatValue(value2));

  val1 = map(value1, 80, 1023, 0, 180);     // scale it to use it with the servo (value between 0 and 180) 
  val2 = map(value2, 80, 1023, 0, 180);     // scale it to use it with the servo (value between 0 and 180) 
  myservo1.write(val1);                  // sets the servo position according to the scaled value 
  myservo2.write(val2);                  // sets the servo position according to the scaled value  
 }