#include <Stepper.h>

const int stepsPerRevolution = 20; 
const int baudRate = 115200;

//Connection pins
Stepper myStepperX(stepsPerRevolution, 8,9,10,11); 



int speed = 100;
int step = 160; 			//max 250 steps for dvd/cd stepper motors 
int timeInterval = 100;

void setup() {

	// initialize the serial port:
	Serial.begin(baudRate);

	Serial.println("Starting setup()");
	
	// ...

	Serial.println("Ending setup()");
}

void loop() {

	Serial.println("Starting loop()");
	
	Serial.println("Speed 100");
	Serial.println("Step 160");

	myStepperX.setSpeed(speed);
	myStepperX.step(step);
	delay(timeInterval);

	myStepperX.step(-step);
	delay(timeInterval);

	Serial.println("Ending loop()");
}