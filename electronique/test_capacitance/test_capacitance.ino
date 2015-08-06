// http://wordpress.codewrite.co.uk/pic/2014/01/25/capacitance-meter-mk-ii/
// http://wordpress.codewrite.co.uk/pic/2014/01/21/cap-meter-with-arduino-uno/


const int OUT_PIN = A2;
const int IN_PIN = A0;

//Capacitance between IN_PIN and Ground
//Stray capacitance value will vary from board to board.
//Calibrate this value using known capacitor.
const float IN_STRAY_CAP_TO_GND = 24.48;
const float IN_CAP_TO_GND  = IN_STRAY_CAP_TO_GND;
//Pullup resistance will vary depending on board.
//Calibrate this with known capacitor.
const float R_PULLUP = 34.8;  //in k ohms
const int MAX_ADC_VALUE = 1023;

void setup()
{
  pinMode(OUT_PIN, OUTPUT);
  //digitalWrite(OUT_PIN, LOW);  //This is the default state for outputs
  pinMode(IN_PIN, OUTPUT);
  //digitalWrite(IN_PIN, LOW);

  Serial.begin(115200);
}

void loop()
{
    //Capacitor under test between OUT_PIN and IN_PIN
    //Rising high edge on OUT_PIN
    pinMode(IN_PIN, INPUT);
    digitalWrite(OUT_PIN, HIGH);
    int val = analogRead(IN_PIN);
    digitalWrite(OUT_PIN, LOW);

    if (val < 1000)
    {
      //Low value capacitor
      //Clear everything for next measurement
      pinMode(IN_PIN, OUTPUT);

      //Calculate and print result

      float capacitance = (float)val * IN_CAP_TO_GND / (float)(MAX_ADC_VALUE - val);

      Serial.print(F("Capacitance Value = "));
      Serial.print(capacitance, 3);
      Serial.print(F(" pF ("));
      Serial.print(val);
      Serial.println(F(") "));
    }
    else
    {
      //Big capacitor - so use RC charging method

      //discharge the capacitor (from low capacitance test)
      pinMode(IN_PIN, OUTPUT);
      delay(1);

      //Start charging the capacitor with the internal pullup
      pinMode(OUT_PIN, INPUT_PULLUP);
      unsigned long u1 = micros();
      unsigned long t;
      int digVal;

      //Charge to a fairly arbitrary level mid way between 0 and 5V
      //Best not to use analogRead() here because it's not really quick enough
      do
      {
        digVal = digitalRead(OUT_PIN);
        unsigned long u2 = micros();
        t = u2 > u1 ? u2 - u1 : u1 - u2;
      } while ((digVal < 1) && (t < 400000L));

      pinMode(OUT_PIN, INPUT);  //Stop charging
      //Now we can read the level the capacitor has charged up to
      val = analogRead(OUT_PIN);

      //Discharge capacitor for next measurement
      digitalWrite(IN_PIN, HIGH);
      int dischargeTime = (int)(t / 1000L) * 5;
      delay(dischargeTime);    //discharge slowly to start with
      pinMode(OUT_PIN, OUTPUT);  //discharge remainder quickly
      digitalWrite(OUT_PIN, LOW);
      digitalWrite(IN_PIN, LOW);

      //Calculate and print result
      float capacitance = -(float)t / R_PULLUP
                              / log(1.0 - (float)val / (float)MAX_ADC_VALUE);

      Serial.print(F("Capacitance Value = "));
      if (capacitance > 1000.0)
      {
        Serial.print(capacitance / 1000.0, 2);
        Serial.print(F(" uF"));
      }
      else
      {
        Serial.print(capacitance, 2);
        Serial.print(F(" nF"));
      }

      Serial.print(F(" ("));
      Serial.print(digVal == 1 ? F("Normal") : F("HighVal"));
      Serial.print(F(", t= "));
      Serial.print(t);
      Serial.print(F(" us, ADC= "));
      Serial.print(val);
      Serial.println(F(")"));
    }
    while (millis() % 1000 != 0)
      ;    
}
