//----------------------------------------------------------------------------------------------------------------------
// TinyTX Simple Tranceive Example
//
//
// Licenced under the Creative Commons Attribution-ShareAlike 3.0 Unported (CC BY-SA 3.0) licence:
// http://creativecommons.org/licenses/by-sa/3.0/
//
// Requires Arduino IDE with arduino-tiny core: http://code.google.com/p/arduino-tiny/
//----------------------------------------------------------------------------------------------------------------------

#include <JeeLib.h>        // https://github.com/jcw/jeelib

#define DEBUG false        // comment this line

#define SLEEP_TIME 1000

#include <SoftSerial.h>
#include <TinyPinChange.h>

#define SERIAL_TX_PIN 0 /* Physical Pin 3 for an ATtinyX5 and Physical Pin  9 for an ATtinyX4 */
#define SERIAL_RX_PIN 3 /* PHYSICAL PIN 2 FOR AN ATTINYX5 AND PHYSICAL PIN 10 FOR AN ATTINYX4 */

SoftSerial mySerial(SERIAL_RX_PIN, SERIAL_TX_PIN);

#define myNodeID 29      // RF12 node ID in the range 1-30
#define network 210      // RF12 Network group
#define freq RF12_433MHZ // Frequency of RFM12B module

#define USE_ACK           // Enable ACKs, comment out to disable
#define RETRY_PERIOD 5    // How soon to retry (in seconds) if ACK didn't come in
#define RETRY_LIMIT 5     // Maximum number of times to retry
#define ACK_TIME 10       // Number of milliseconds to wait for an ack

//########################################################################################################################
//Data Structure to be sent
//########################################################################################################################

typedef struct {
  int valueOne; // Humidity reading
  int valueTwo;  // Supply voltage
  int valueThree; // Temperature reading
} Payload;

Payload tinytx;

// Wait a few milliseconds for proper ACK
#ifdef USE_ACK
static byte waitForAck() {
  MilliTimer ackTimer;
  while (!ackTimer.poll(ACK_TIME)) {
    if (rf12_recvDone() && rf12_crc == 0 &&
        rf12_hdr == (RF12_HDR_DST | RF12_HDR_CTL | myNodeID))
      return 1;
  }
  return 0;
}
#endif

//--------------------------------------------------------------------------------------------------
// Send payload data via RF
//-------------------------------------------------------------------------------------------------
static void rfwrite() {
#ifdef USE_ACK
  for (byte i = 0; i <= RETRY_LIMIT; ++i) {  // tx and wait for ack up to RETRY_LIMIT times
    rf12_sleep(-1);              // Wake up RF module
    while (!rf12_canSend())
      rf12_recvDone();
    rf12_sendStart(RF12_HDR_ACK, &tinytx, sizeof tinytx);
    rf12_sendWait(2);           // Wait for RF to finish sending while in standby mode
    byte acked = waitForAck();  // Wait for ACK
    rf12_sleep(0);              // Put RF module to sleep
    if (acked) {
      return;  // Return if ACK received
    }

    Sleepy::loseSomeTime(RETRY_PERIOD * 1000);     // If no ack received wait and try again
  }
#else
  rf12_sleep(-1);              // Wake up RF module
  while (!rf12_canSend())
    rf12_recvDone();
  rf12_sendStart(0, &tinytx, sizeof tinytx);
  rf12_sendWait(2);           // Wait for RF to finish sending while in standby mode
  rf12_sleep(0);              // Put RF module to sleep
  return;
#endif
}

//########################################################################################################################

void setup() {

  mySerial.begin(9600);
  mySerial.println(F("Transmetteur TinyTX"));
  mySerial.print("NodeID : "); mySerial.println(myNodeID);
  mySerial.print("Frequency : "); mySerial.println(freq);
  mySerial.print("Network : "); mySerial.println(network);

  rf12_initialize(myNodeID, freq, network); // Initialize RFM12 with settings defined above
  rf12_sleep(0);                          // Put the RFM12 to sleep

  PRR = bit(PRTIM1); // only keep timer 0 going

  ADCSRA &= ~ bit(ADEN); bitSet(PRR, PRADC); // Disable the ADC to save power

}

void loop() {

  mySerial.println(F("Begin loop ..."));

  tinytx.valueOne = 1;
  tinytx.valueTwo = 2;
  tinytx.valueThree = 3;

  mySerial.print(F("ValueOne = ")); mySerial.println(tinytx.valueOne);
  mySerial.print(F("valueTwo = ")); mySerial.println(tinytx.valueTwo);
  mySerial.print("valueThree "); mySerial.println(tinytx.valueThree);

  rfwrite(); // Send data via RF

  //Sleepy::loseSomeTime(SLEEP_TIME); //JeeLabs power save function: enter low power mode for 60 seconds (valid range 16-65000 ms)

  mySerial.print(F("End of loop."));
}

