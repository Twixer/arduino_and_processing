// Source : https://github.com/shadwork/Arduino-PC-Gameport-HID/blob/master/Analog_Joystick/PC_Gameport_Analog_Joystick.ino

// Settings
#define ENABLE_DEBUG;

// Pins
const int button0 = 7;
const int button1 = 6;
const int button2 = 5;
const int button3 = 4;

const int button4 = 8;
const int button5 = 9;
const int button6 = 10;
const int button7 = 11;

typedef struct JoyState     // Pretty self explanitory. Simple state to store all the joystick parameters
{
  uint8_t   xAxis;
  uint8_t   yAxis;
  uint8_t   zAxis;

  uint8_t   xRotAxis;
  uint8_t   yRotAxis;
  uint8_t   zRotAxis;

  uint8_t   throttle;
  uint8_t   rudder;

  uint8_t   hatSw1;
  uint8_t   hatSw2;

  uint32_t  buttons;    // 32 general buttons

} JoyState_t;

// Joestick
JoyState_t joyStick;

// Ugly vars
int buttonState0 = 0;
int buttonState1 = 0;
int buttonState2 = 0;
int buttonState3 = 0;
int buttonState4 = 0;
int buttonState5 = 0;
int buttonState6 = 0;
int buttonState7 = 0;

int oldButtonState0 = 0;
int oldButtonState1 = 0;
int oldButtonState2 = 0;
int oldButtonState3 = 0;
int oldButtonState4 = 0;
int oldButtonState5 = 0;
int oldButtonState6 = 0;
int oldButtonState7 = 0;

boolean state = false;

// Setup
void setup() {
  Serial.begin(57600);
  // pull up
  pinMode(button0, INPUT_PULLUP);
  pinMode(button1, INPUT_PULLUP);
  pinMode(button2, INPUT_PULLUP);
  pinMode(button3, INPUT_PULLUP);
  pinMode(button4, INPUT_PULLUP);
  pinMode(button5, INPUT_PULLUP);
  pinMode(button6, INPUT_PULLUP);
  pinMode(button7, INPUT_PULLUP);
}

boolean stateChanged() {
  state = false;

  state |= (buttonState0 != oldButtonState0);
  state |= (buttonState1 != oldButtonState1);
  state |= (buttonState2 != oldButtonState2);
  state |= (buttonState3 != oldButtonState3);

  state |= (buttonState4 != oldButtonState4);
  state |= (buttonState5 != oldButtonState5);
  state |= (buttonState6 != oldButtonState6);
  state |= (buttonState7 != oldButtonState7);

  return state;
}

void printState() {
  Serial.print("B0:");
  Serial.print(buttonState0);
  Serial.print(", B1:");
  Serial.print(buttonState1);
  Serial.print(", B2:");
  Serial.print(buttonState2);
  Serial.print(", B3:");
  Serial.print(buttonState3);
  Serial.print(", Axe 0:");
  Serial.print(buttonState4);
  Serial.print(", Axe 1:");
  Serial.print(buttonState5);
  Serial.print(", Axe 2:");
  Serial.print(buttonState6);
  Serial.print(", Axe 3:");
  Serial.println(buttonState7);
}


void loop() {
  // read data
  buttonState0 = !digitalRead(button0);
  buttonState1 = !digitalRead(button1);
  buttonState2 = !digitalRead(button2);
  buttonState3 = !digitalRead(button3);

  buttonState4 = !digitalRead(button4);
  buttonState5 = !digitalRead(button5);
  buttonState6 = !digitalRead(button6);
  buttonState7 = !digitalRead(button7);


#ifdef ENABLE_DEBUG
  //printState();
#endif

  if (stateChanged()) {
    printState();
  }


  delay(1);
}


