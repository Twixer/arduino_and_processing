#ifndef RcTxSerial_h
#define RcTxSerial_h

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

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <inttypes.h>
#include <Stream.h>

#if F_CPU < 16000000L
#error Please, select a board at 16MHz or 20MHz
#endif

enum {RC_TX_SERIAL_INIT_NOT_SAVED_DEFAULT=0, RC_TX_SERIAL_INIT_CURRENT_EEPROM};

enum {RC_TX_SERIAL_MODE_NORMAL=0, RC_TX_SERIAL_MODE_TEST_MIN, RC_TX_SERIAL_MODE_TEST_MAX, RC_TX_SERIAL_MODE_NB};

class RcTxSerial : public Stream
{
  private:
    // static data
	uint8_t _TxFifoSize;
    uint8_t _TxPwmPin;
    uint8_t _PwmMin;
    uint8_t _PwmMax;
	boolean _TxCharInProgress;
    char   *_TxFifo;
	char    _TxChar;
    uint8_t _TxFifoTail;
    uint8_t _TxFifoHead;
    class   RcTxSerial *next;
    static  RcTxSerial *first;
    uint8_t TxFifoRead(char *TxChar);
  public:
    RcTxSerial(uint8_t PpmPin, uint8_t TxPin, uint8_t TxFifoSize);
    void    setPwmMin(uint8_t PwmMin);
    void    setPwmMax(uint8_t PwmMax);
    uint8_t getPwmMin();
    uint8_t getPwmMax();
    int peek();
    virtual size_t write(uint8_t byte);
    virtual int read();
    virtual int available();
    virtual void flush();
    using Print::write;
    static uint8_t init(uint8_t EepromStartIdx, boolean ForceClear=0);
	static void    setMode(uint8_t Mode);
	static uint8_t getMode();
	static void    saveSetupInEeprom();
    static uint8_t getEepromStorageSize();
    static uint8_t refresh();
};

#endif
