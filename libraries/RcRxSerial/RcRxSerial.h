#ifndef RcRxSerial_h
#define RcRxSerial_h

/*
 English: by RC Navy (2012)
 =======
 <RcRxSerial>: a library to build an unidirectionnal serial port through RC Transmitter/Receiver.
 http://p.loussouarn.free.fr

 Francais: par RC Navy (2012)
 ========
 <RcRxSerial>: une librairie pour construire un port serie a travers un Emetteur/Recepteur RC.
 http://p.loussouarn.free.fr
*/

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <inttypes.h>
#include "../SoftRcPulseIn/SoftRcPulseIn.h"
#include "../TinyPinChange/TinyPinChange.h"

class RcRxSerial
{
  private:
	// static data
	SoftRcPulseIn *_RxChannel;
	char     _Char;
	uint16_t _LastWidth_us;
	boolean  _available;
	boolean  _Nibble;
	boolean  _NibbleAvailable;
  public:
    RcRxSerial(uint8_t RxPin);
    uint8_t  available();
    uint8_t  read();
	uint16_t lastWidth_us();     /* Only for calibration purpose */
    uint8_t  nibbleAvailable();  /* Only for calibration purpose */
};

#endif
