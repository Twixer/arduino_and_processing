/* 
 * //////////////////////////////////////////////////
 * //making sense of the Parallax PIR sensor's output
 * //////////////////////////////////////////////////
 *
 * Switches a LED according to the state of the sensors output pin.
 * Determines the beginning and end of continuous motion sequences.
 *
 * @author: Kristian Gohlke / krigoo (_) gmail (_) com / http://krx.at
 * @date:   3. September 2006 
 * http://www.instructables.com/id/Arduino-Basics-PIR-Sensor/?ALLSTEPS
 * kr1 (cleft) 2006 
 * released under a creative commons "Attribution-NonCommercial-ShareAlike 2.0" license
 * http://creativecommons.org/licenses/by-nc-sa/2.0/de/
 *
 *
 * The Parallax PIR Sensor is an easy to use digital infrared motion sensor module. 
 * (http://www.parallax.com/detail.asp?product_id=555-28027)
 *
 * The sensor's output pin goes to HIGH if motion is present.
 * However, even if motion is present it goes to LOW from time to time, 
 * which might give the impression no motion is present. 
 * This program deals with this issue by ignoring LOW-phases shorter than a given time, 
 * assuming continuous motion is present during these phases.
 *  
 */
#define	C0 16.35
#define	Db0	17.32
#define	D0	18.35
#define	Eb0	19.45
#define	E0	20.60
#define	F0	21.83
#define	Gb0	23.12
#define	G0	24.50
#define	Ab0	25.96
#define	LA0	27.50
#define	Bb0	29.14
#define	B0	30.87
#define	C1	32.70
#define	Db1	34.65
#define	D1	36.71
#define	Eb1	38.89
#define	E1	41.20
#define	F1	43.65
#define	Gb1	46.25
#define	G1	49.00
#define	Ab1	51.91
#define	LA1	55.00
#define	Bb1	58.27
#define	B1	61.74
#define	C2	65.41
#define	Db2	69.30
#define	D2	73.42
#define	Eb2	77.78
#define	E2	82.41
#define	F2	87.31
#define	Gb2	92.50
#define	G2	98.00
#define	Ab2	103.83
#define	LA2	110.00
#define	Bb2	116.54
#define	B2	123.47
#define	C3	130.81
#define	Db3	138.59
#define	D3	146.83
#define	Eb3	155.56
#define	E3	164.81
#define	F3	174.61
#define	Gb3	185.00
#define	G3	196.00
#define	Ab3	207.65
#define	LA3	220.00
#define	Bb3	233.08
#define	B3	246.94
#define	C4	261.63
#define	Db4	277.18
#define	D4	293.66
#define	Eb4	311.13
#define	E4	329.63
#define	F4	349.23
#define	Gb4	369.99
#define	G4	392.00
#define	Ab4	415.30
#define	LA4	440.00
#define	Bb4	466.16
#define	B4	493.88
#define	C5	523.25
#define	Db5	554.37
#define	D5	587.33
#define	Eb5	622.25
#define	E5	659.26
#define	F5	698.46
#define	Gb5	739.99
#define	G5	783.99
#define	Ab5	830.61
#define	LA5	880.00
#define	Bb5	932.33
#define	B5	987.77
#define	C6	1046.50
#define	Db6	1108.73
#define	D6	1174.66
#define	Eb6	1244.51
#define	E6	1318.51
#define	F6	1396.91
#define	Gb6	1479.98
#define	G6	1567.98
#define	Ab6	1661.22
#define	LA6	1760.00
#define	Bb6	1864.66
#define	B6	1975.53
#define	C7	2093.00
#define	Db7	2217.46
#define	D7	2349.32
#define	Eb7	2489.02
#define	E7	2637.02
#define	F7	2793.83
#define	Gb7	2959.96
#define	G7	3135.96
#define	Ab7	3322.44
#define	LA7	3520.01
#define	Bb7	3729.31
#define	B7	3951.07
#define	C8	4186.01
#define	Db8	4434.92
#define	D8	4698.64
#define	Eb8	4978.03
// DURATION OF THE NOTES 
#define BPM 120    //  you can change this value changing all the others
#define H 2*Q //half 2/4
#define Q 60000/BPM //quarter 1/4 
#define E Q/2   //eighth 1/8
#define S Q/4 // sixteenth 1/16
#define W 4*Q // whole 4/4

/////////////////////////////
//VARS
//the time we give the sensor to calibrate (10-60 secs according to the datasheet)
int calibrationTime = 30;        

//the time when the sensor outputs a low impulse
long unsigned int lowIn;         

//the amount of milliseconds the sensor has to be low 
//before we assume all motion has stopped
long unsigned int pause = 5000;  

boolean lockLow = true;
boolean takeLowTime;  

int pirPin = 7;    //the digital pin connected to the PIR sensor's output
int ledPin = 8;
int buzzPin = 9;

void buzz() {
  //tone(pin, note, duration)
  tone(buzzPin,LA3,Q); 
  delay(1+Q); //delay duration should always be 1 ms more than the note in order to separate them.
  tone(buzzPin,LA3,Q);
  delay(1+Q);
  tone(buzzPin,LA3,Q);
  delay(1+Q);
  tone(buzzPin,F3,E+S);
  delay(1+E+S);
  tone(buzzPin,C4,S);
  delay(1+S);

  tone(buzzPin,LA3,Q);
  delay(1+Q);
  tone(buzzPin,F3,E+S);
  delay(1+E+S);
  tone(buzzPin,C4,S);
  delay(1+S);
  tone(buzzPin,LA3,H);
  delay(1+H);

  tone(buzzPin,E4,Q); 
  delay(1+Q); 
  tone(buzzPin,E4,Q);
  delay(1+Q);
  tone(buzzPin,E4,Q);
  delay(1+Q);
  tone(buzzPin,F4,E+S);
  delay(1+E+S);
  tone(buzzPin,C4,S);
  delay(1+S);

  tone(buzzPin,Ab3,Q);
  delay(1+Q);
  tone(buzzPin,F3,E+S);
  delay(1+E+S);
  tone(buzzPin,C4,S);
  delay(1+S);
  tone(buzzPin,LA3,H);
  delay(1+H);

  tone(buzzPin,LA4,Q);
  delay(1+Q);
  tone(buzzPin,LA3,E+S);
  delay(1+E+S);
  tone(buzzPin,LA3,S);
  delay(1+S);
  tone(buzzPin,LA4,Q);
  delay(1+Q);
  tone(buzzPin,Ab4,E+S);
  delay(1+E+S);
  tone(buzzPin,G4,S);
  delay(1+S);

  tone(buzzPin,Gb4,S);
  delay(1+S);
  tone(buzzPin,E4,S);
  delay(1+S);
  tone(buzzPin,F4,E);
  delay(1+E);
  delay(1+E);//PAUSE
  tone(buzzPin,Bb3,E);
  delay(1+E);
  tone(buzzPin,Eb4,Q);
  delay(1+Q);
  tone(buzzPin,D4,E+S);
  delay(1+E+S);
  tone(buzzPin,Db4,S);
  delay(1+S);

  tone(buzzPin,C4,S);
  delay(1+S);
  tone(buzzPin,B3,S);
  delay(1+S);
  tone(buzzPin,C4,E);
  delay(1+E);
  delay(1+E);//PAUSE QUASI FINE RIGA
  tone(buzzPin,F3,E);
  delay(1+E);
  tone(buzzPin,Ab3,Q);
  delay(1+Q);
  tone(buzzPin,F3,E+S);
  delay(1+E+S);
  tone(buzzPin,LA3,S);
  delay(1+S);

  tone(buzzPin,C4,Q);
  delay(1+Q);
  tone(buzzPin,LA3,E+S);
  delay(1+E+S);
  tone(buzzPin,C4,S);
  delay(1+S);
  tone(buzzPin,E4,H);
  delay(1+H);

  tone(buzzPin,LA4,Q);
  delay(1+Q);
  tone(buzzPin,LA3,E+S);
  delay(1+E+S);
  tone(buzzPin,LA3,S);
  delay(1+S);
  tone(buzzPin,LA4,Q);
  delay(1+Q);
  tone(buzzPin,Ab4,E+S);
  delay(1+E+S);
  tone(buzzPin,G4,S);
  delay(1+S);

  tone(buzzPin,Gb4,S);
  delay(1+S);
  tone(buzzPin,E4,S);
  delay(1+S);
  tone(buzzPin,F4,E);
  delay(1+E);
  delay(1+E);//PAUSE
  tone(buzzPin,Bb3,E);
  delay(1+E);
  tone(buzzPin,Eb4,Q);
  delay(1+Q);
  tone(buzzPin,D4,E+S);
  delay(1+E+S);
  tone(buzzPin,Db4,S);
  delay(1+S);

  tone(buzzPin,C4,S);
  delay(1+S);
  tone(buzzPin,B3,S);
  delay(1+S);
  tone(buzzPin,C4,E);
  delay(1+E);
  delay(1+E);//PAUSE QUASI FINE RIGA
  tone(buzzPin,F3,E);
  delay(1+E);
  tone(buzzPin,Ab3,Q);
  delay(1+Q);
  tone(buzzPin,F3,E+S);
  delay(1+E+S);
  tone(buzzPin,C4,S);
  delay(1+S);

  tone(buzzPin,LA3,Q);
  delay(1+Q);
  tone(buzzPin,F3,E+S);
  delay(1+E+S);
  tone(buzzPin,C4,S);
  delay(1+S);
  tone(buzzPin,LA3,H);
  delay(1+H);

  delay(2*H);
}
/////////////////////////////
//SETUP
void setup(){
  Serial.begin(9600);
  pinMode(pirPin, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(buzzPin, OUTPUT);   
  digitalWrite(pirPin, LOW);

  //give the sensor some time to calibrate
  Serial.print("calibrating sensor ");
  for(int i = 0; i < calibrationTime; i++){
    Serial.print(".");
    delay(1000);
  }
  Serial.println(" done");
  Serial.println("SENSOR ACTIVE");
  delay(50);
}

////////////////////////////
//LOOP
void loop(){

  if(digitalRead(pirPin) == HIGH){
    digitalWrite(ledPin, HIGH);   //the led visualizes the sensors output pin state
    if(lockLow){  
      //makes sure we wait for a transition to LOW before any further output is made:
      lockLow = false;            
      Serial.println("---");
      Serial.print("motion detected at ");
      Serial.print(millis()/1000);
      Serial.println(" sec"); 
      delay(50);
      buzz();
    }         
    takeLowTime = true;
  }

  if(digitalRead(pirPin) == LOW){       
    digitalWrite(ledPin, LOW);  //the led visualizes the sensors output pin state

    if(takeLowTime){
      lowIn = millis();          //save the time of the transition from high to LOW
      takeLowTime = false;       //make sure this is only done at the start of a LOW phase
    }

    //if the sensor is low for more than the given pause, 
    //we assume that no more motion is going to happen
    if(!lockLow && millis() - lowIn > pause){  
      //makes sure this block of code is only executed again after 
      //a new motion sequence has been detected
      lockLow = true;                        
      Serial.print("motion ended at ");      //output
      Serial.print((millis() - pause)/1000);
      Serial.println(" sec");
      delay(50);
    }
  }
}



