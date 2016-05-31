#ifndef TinyOscCal_h
#define TinyOscCal_h

/*
 English: by RC Navy (2012)
 =======
 <_TinyOscCal>: a library to easily calibrate the internal Oscillator of the ATtiny for a reliable usage of <SoftSerial>.
 http://p.loussouarn.free.fr

 Francais: par RC Navy (2012)
 ========
 <_TinyOscCal>: une librairie pour calibrer facilement l'oscillateur interne des ATtiny pour un usage fiable des <SoftSerial>.
 http://p.loussouarn.free.fr
*/

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <inttypes.h>
#include <SoftSerial.h>
#include <EEPROM.h>


enum {TINY_OSC_CAL_INIT_CALIBRATION_NOT_DONE=0, TINY_OSC_CAL_INIT_CALIBRATION_DONE, TINY_OSC_CAL_INIT_CURRENT_FROM_EEPROM};

class _TinyOscCal
{
  public:
    _TinyOscCal();
    static uint8_t init(SoftSerial *MySerial, uint8_t EepromStartIdx, boolean ForceClear = 0);
    static uint8_t getEepromStorageSize();
};

extern _TinyOscCal TinyOscCal;                 //Object externalisation

#endif
