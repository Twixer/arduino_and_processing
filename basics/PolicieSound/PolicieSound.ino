/*
  Motor, lights, and siren code with LED.

  Drives a motor, lights and siren with input of LED light

  circuit:
  *one small motor
  *small speaker
  *six LED's
  *one photodiode

  Created 4-17-12
  by Dalton Bolger and Dulcie Head

*/

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations = 16;

int IR = A0;
int motor = 7;
int blueLed = A5;
int redLed = A4;

// creates the state for the lights and intializes the time
int ledState = LOW;
long previousMillis = 0;

// time in between flashes for the lights
long interval = 100;

void setup() {
  pinMode(IR, INPUT);
  pinMode(motor, OUTPUT);
  pinMode(blueLed, OUTPUT);
  pinMode(redLed, OUTPUT);
  Serial.begin(9600);
}


// run the sirens while flashing lights continuously
void lightsAndSirens() {
  int melody = 550;
  while (melody<1200) {
    // to calculate the note duration, take one second
    // divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000/noteDurations;
    tone(8, melody, noteDuration);
    melody = melody + 7;

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * .5;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(8);
  }
  melody = 1200;
  while (melody > 550) {
    int noteDuration = 1000/noteDurations;
    tone(8, melody, noteDuration);
    melody = melody - 7;
  
    int pauseBetweenNotes = noteDuration * .5;
    delay(pauseBetweenNotes);
    noTone(8);
  }
}


// if the appropriate light shows up, run the motor, sirens, and lights
void loop() {

    lightsAndSirens();

  delay(5);
}
