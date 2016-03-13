#include "RcTxSerial.h"
#include "Arduino.h"
#include <avr/pgmspace.h>
#include "../EEPROM/EEPROM.h"

#if (TIMER0_TICK_EVERY_X_CYCLES > 1)
#error RcTxSerial libray needs TIMER0_TICK_EVERY_X_CYCLES to be defined to 1 in /arduino-1.0.x/hardware/arduino/cores/arduino/Arduino.h
#endif

/*
 English: by RC Navy (2012)
 =======
 <RcTxSerial>: a library to build an unidirectionnal serial port through RC Transmitter/Receiver.
 http://p.loussouarn.free.fr

 Francais: par RC Navy (2012)
 ========
 <RcTxSerial>: une librairie pour construire un port serie a travers un Emetteur/Eecepteur RC.
 http://p.loussouarn.free.fr
*/

/*
  0                                                                  255
  |---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
    0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F   I
*/
#define NIBBLE_DEFAULT_PWM_MIN        0
#define NIBBLE_DEFAULT_PWM_MAX        255

#define EEPROM_SIZE_PER_RC_TX_SERIAL  2 /* Bytes to save in EEPROM for each instance of RcTxSerial */

#define PPM_SYNCHRO_MIN_US            4000UL
#define PPM_PERIOD_MIN_US             15000UL

enum {NIBBLE_0=0, NIBBLE_1, NIBBLE_2, NIBBLE_3, NIBBLE_4, NIBBLE_5, NIBBLE_6, NIBBLE_7, NIBBLE_8, NIBBLE_9, NIBBLE_A, NIBBLE_B, NIBBLE_C, NIBBLE_D, NIBBLE_E, NIBBLE_F, NIBBLE_I, NIBBLE_NB};


RcTxSerial *RcTxSerial::first;

/*************************************************************************
							GLOBAL VARIABLES
*************************************************************************/
static uint8_t _EepromStartIdx=0;
static uint8_t _PpmPin; /* PpmPin is common to all RcTxSerial Objects */
static uint8_t _Mode=RC_TX_SERIAL_MODE_NORMAL;

static boolean IsPpmSynchro();
static uint8_t NibbleToPwm(uint8_t NibbleIdx, uint8_t PwmMin, uint8_t PwmMax);

/* Constructor */
RcTxSerial::RcTxSerial(uint8_t PpmPin, uint8_t TxPwmPin, uint8_t TxFifoSize)
{
	if(TxFifoSize>128) TxFifoSize=128; /* Must fit in a 8 bits  */
	_TxFifoSize=1;
	do
	{
		_TxFifoSize<<=1;
	}while(_TxFifoSize<TxFifoSize); /* Search for the _TxFifoSize in power of 2 just greater or equal to requested size */
	_TxFifo=(char *)malloc(_TxFifoSize);
	if(_TxFifo!=NULL)
	{
		_TxPwmPin=TxPwmPin;
		_PpmPin=PpmPin;
		pinMode(_PpmPin,INPUT);
		_PwmMin=NIBBLE_DEFAULT_PWM_MIN;
		_PwmMax=NIBBLE_DEFAULT_PWM_MAX;
		_TxCharInProgress=0;
		_TxFifoTail=0;
		_TxFifoHead=0;
		next = first;
		first = this;
	}
}

uint8_t RcTxSerial::init(uint8_t EepromStartIdx, boolean ForceClear /* =0 */)
{
RcTxSerial *t;
uint8_t InstanceIdx=0, EepromMin, EepromMax;
uint8_t Ret;

  _EepromStartIdx=EepromStartIdx; /* Set start of EEPROM storage area */
  for ( t = first; t != 0; t = t->next )
  {
	if(ForceClear)
	{
	  EEPROM.write(_EepromStartIdx+(InstanceIdx*EEPROM_SIZE_PER_RC_TX_SERIAL)+0,0xFF);
	  EEPROM.write(_EepromStartIdx+(InstanceIdx*EEPROM_SIZE_PER_RC_TX_SERIAL)+1,0xFF);
	}
	EepromMin=EEPROM.read(_EepromStartIdx+(InstanceIdx*EEPROM_SIZE_PER_RC_TX_SERIAL)+0);
	EepromMax=EEPROM.read(_EepromStartIdx+(InstanceIdx*EEPROM_SIZE_PER_RC_TX_SERIAL)+1);
	if( EepromMin < EepromMax )
	{
	  t->_PwmMin=EepromMin;
	  t->_PwmMax=EepromMax;
	  Ret=RC_TX_SERIAL_INIT_CURRENT_EEPROM;
	}
	else
	{
	  Ret=RC_TX_SERIAL_INIT_NOT_SAVED_DEFAULT;
	}
	InstanceIdx++;
  }
  return(Ret);
}

void RcTxSerial::setPwmMin(uint8_t PwmMin)
{
  _PwmMin=PwmMin;
}

void RcTxSerial::setPwmMax(uint8_t PwmMax)
{
  _PwmMax=PwmMax;
}

uint8_t RcTxSerial::getPwmMin()
{
  return(_PwmMin);
}

uint8_t RcTxSerial::getPwmMax()
{
  return(_PwmMax);  
}

size_t RcTxSerial::write(uint8_t b)
{
size_t Ret=0;

    // if buffer full, discard the character and return
    if ((_TxFifoTail + 1) % _TxFifoSize != _TxFifoHead) 
    {
      // save new data in buffer: tail points to where byte goes
      _TxFifo[_TxFifoTail] = b; // save new byte
      _TxFifoTail = (_TxFifoTail + 1) % _TxFifoSize;
      Ret=1;
    }
    return(Ret);
}

int RcTxSerial::read()
{
    return -1;
}

int RcTxSerial::available()
{
    return 0;
}

void RcTxSerial::flush()
{
  _TxFifoHead = _TxFifoTail = 0;
}

int RcTxSerial::peek()
{
  // Empty buffer?
  if (_TxFifoHead == _TxFifoTail)
    return -1;

  // Read from "head"
  return _TxFifo[_TxFifoHead];
}

uint8_t RcTxSerial::refresh()
{
uint8_t Ret=0;
RcTxSerial *t;

	if(IsPpmSynchro()) /* PPM Synchro Period is 15 to 25 ms */
	{
		for ( t = first; t != 0; t = t->next )
		{
			if(t->_TxCharInProgress)
			{
				analogWrite(t->_TxPwmPin,NibbleToPwm(t->_TxChar&0x0F,t->_PwmMin,t->_PwmMax)); /* LSN */
				t->_TxCharInProgress=0;
			}
			else
			{
				switch(_Mode)
				{
					case RC_TX_SERIAL_MODE_NORMAL:
					if(t->TxFifoRead(&t->_TxChar))
					{
						analogWrite(t->_TxPwmPin,NibbleToPwm((t->_TxChar&0xF0)>>4,t->_PwmMin,t->_PwmMax)); /* MSN first */
						t->_TxCharInProgress=1;
					}
					else
					{
						analogWrite(t->_TxPwmPin,NibbleToPwm(NIBBLE_I,t->_PwmMin,t->_PwmMax)); /* MSN first */
					}
					break;
				  
					case RC_TX_SERIAL_MODE_TEST_MIN:
					analogWrite(t->_TxPwmPin,NibbleToPwm(NIBBLE_0,t->_PwmMin,t->_PwmMax)); /* Continuous 0 for calibration */
					break;
					
					case RC_TX_SERIAL_MODE_TEST_MAX:
					analogWrite(t->_TxPwmPin,NibbleToPwm(NIBBLE_I,t->_PwmMin,t->_PwmMax)); /* Continuous I for calibration */
					break;
				}
			}
		}
		Ret=1;
	}
	return(Ret);
}

uint8_t RcTxSerial::getEepromStorageSize()
{
uint8_t EepromStorageSize=0;
RcTxSerial *t;

  for ( t = first; t != 0; t = t->next )
  {
	EepromStorageSize+=EEPROM_SIZE_PER_RC_TX_SERIAL;
  }
  return(EepromStorageSize);
}

void RcTxSerial::saveSetupInEeprom()
{
RcTxSerial *t;
uint8_t InstanceIdx=0;

  for ( t = first; t != 0; t = t->next )
  {
	EEPROM.write(_EepromStartIdx+(InstanceIdx*EEPROM_SIZE_PER_RC_TX_SERIAL)+0,t->_PwmMin);
	EEPROM.write(_EepromStartIdx+(InstanceIdx*EEPROM_SIZE_PER_RC_TX_SERIAL)+1,t->_PwmMax);
	InstanceIdx++;
  }
  
}

void RcTxSerial::setMode(uint8_t Mode)
{
  if(Mode<RC_TX_SERIAL_MODE_NB)
  {
	_Mode=Mode;
  }
}

uint8_t RcTxSerial::getMode()
{
	return(_Mode);
}

//========================================================================================================================
// PRIVATE FUNCTIONS
//========================================================================================================================
static boolean IsPpmSynchro()
{
static uint32_t HighStartChronoUs=micros(), LowStartChronoUs=micros(), PpmStartChronoUs=millis();
static boolean LastStatus=0;
boolean Ret=0;

   if(digitalRead(_PpmPin)==HIGH)
   {
     if(LastStatus)
     {
       if(((micros()-HighStartChronoUs)>=PPM_SYNCHRO_MIN_US) && ((micros()-PpmStartChronoUs)>=PPM_PERIOD_MIN_US))
       {
         PpmStartChronoUs=micros();
         Ret=1;
       }
     }
     else
     {
       HighStartChronoUs=micros();
       LastStatus=1;
     }
   }
   else
   {
     if(!LastStatus)
     {
       if(((micros()-LowStartChronoUs)>=PPM_SYNCHRO_MIN_US) && ((micros()-PpmStartChronoUs)>=PPM_PERIOD_MIN_US))
       {
         PpmStartChronoUs=micros();
         Ret=1;
       }
     }
     else
     {
       LowStartChronoUs=micros();
       LastStatus=0;
     }
   }
   return(Ret);
}

uint8_t RcTxSerial::TxFifoRead(char *TxChar)
{
uint8_t Ret=0;
  // Empty buffer?
  if (_TxFifoHead != _TxFifoTail)
  {
    // Read from "head"
    *TxChar = _TxFifo[_TxFifoHead]; // grab next byte
    _TxFifoHead = (_TxFifoHead + 1) % _TxFifoSize;
    Ret=1;
  }
  return(Ret);
}

static uint8_t NibbleToPwm(uint8_t NibbleIdx, uint8_t PwmMin, uint8_t PwmMax)
{
  return( PwmMin + (((2*NibbleIdx + 1)*(PwmMax-PwmMin))/(2*NIBBLE_NB)) );
}
