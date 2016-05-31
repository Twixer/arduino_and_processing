//----------------------------------------------------------------------------------------------------------------------//----------------------------------------------------------------------------------------------------------------------
// TinyTX Simple Receive Example
// By Nathan Chantrell.
//
// Licenced under the Creative Commons Attribution-ShareAlike 3.0 Unported (CC BY-SA 3.0) licence:
// http://creativecommons.org/licenses/by-sa/3.0/
//----------------------------------------------------------------------------------------------------------------------

#include <JeeLib.h> // https://github.com/jcw/jeelib
//#include <SoftwareSerial.h>
#include <SoftSerial.h>
#include <TinyPinChange.h>

// Set this to 1 to build a version which works with RFM69 radios in “RF12 compatibility mode”.
#define RF69_COMPAT 0

// Fixed RF12 settings
#define MYNODE 30            //node ID of the receiever
#define freq RF12_433MHZ     //frequency
#define group 210            //network group

#define SERIAL_RX_PIN 3 /* Physical Pin 2 for an ATtinyX5 and Physical Pin 10 for an ATtinyX4 */
#define SERIAL_TX_PIN 0 /* Physical Pin 3 for an ATtinyX5 and Physical Pin  9 for an ATtinyX4 */

//SoftwareSerial Serial(SERIAL_RX_PIN, SERIAL_TX_PIN);
SoftSerial mySerial(SERIAL_RX_PIN, SERIAL_TX_PIN);

typedef struct {
  int rxD;              // sensor value
  int supplyV;          // tx voltage
} Payload;
Payload rx;

int nodeID;    //node ID of tx, extracted from RF datapacket. Not transmitted as part of structure

void setup () {
  //Serial.begin(96000);
  mySerial.begin(9600);
  rf12_initialize(MYNODE, freq, group); // Initialise the RFM12B

  mySerial.println("TinyTX Simple Receive Example");
//  mySerial.print("Node Group : ");mySerial.println(group);
//  mySerial.print("Node ID : ");mySerial.println(MYNODE);
//  mySerial.print("Node Frequence : ");mySerial.println(freq);
  mySerial.println("-----------------------------");
  mySerial.println("Waiting for data");
  mySerial.println(" ");
}

void loop() {
  mySerial.print(".");
  
  if (rf12_recvDone() && rf12_crc == 0 && (rf12_hdr & RF12_HDR_CTL) == 0) {
    nodeID = rf12_hdr & 0x1F;  // get node ID
    rx = *(Payload*) rf12_data;
    int value = rx.rxD;
    int millivolts = rx.supplyV;

    if (RF12_WANTS_ACK) {                  // Send ACK if requested
      rf12_sendStart(RF12_ACK_REPLY, 0, 0);
      mySerial.println("-> ack sent");
    }

    mySerial.println("");
    mySerial.println("Received a packet:");
    mySerial.print("From Node: ");
    mySerial.println(nodeID);
    mySerial.print("Value: ");
    mySerial.println(value);
    mySerial.print("TX Millivolts: ");
    mySerial.println(millivolts);
    mySerial.println("--------------------");
  }
  
  mySerial.print("-");
}
