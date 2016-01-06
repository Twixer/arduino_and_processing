/*
 * source : http://p.loussouarn.free.fr/arduino/exemple/PortSerieSurATtiny.html
 *
 * Ce sketch montre comment utiliser la librairie <TinyOscCal> pour calibrer l'oscillateur interne des ATtiny84 et ATtiny85.
 * Ceci permet l'utilisation fiable des ports series logiciels <SoftSerial> sur les ATtiny84 et ATtiny85.
 * Par RC Navy 2012: http://p.loussouarn.free.fr
*/
#include <EEPROM.h>        /* Utilisee par la librairie TinyOscCal */
#include <TinyPinChange.h> /* Utilisee par la librairie SoftSerial */
#include <SoftSerial.h>
#include <TinyOscCal.h>

#define SERIAL_RX_PIN	10 /* Physical Pin 2 for an ATtinyX5 and Physical Pin 10 for an ATtinyX4 */
#define SERIAL_TX_PIN	9 /* Physical Pin 3 for an ATtinyX5 and Physical Pin  9 for an ATtinyX4 */

SoftSerial MySerial(SERIAL_RX_PIN, SERIAL_TX_PIN);

void setup()
{
  MySerial.begin(9600); /* IMPORTANT: initialiser le port serie AVANT d'appeler TinyOscCal.init() */
  TinyOscCal.init(&MySerial, 0); /* Calibration de l'oscillateur interne si necessaire: stockage de l'offset en position 0 de l'EEPROM */
  MySerial.print(F("Byte used in EEPROM for <TinyOscCal> library: "));MySerial.println((int)TinyOscCal.getEepromStorageSize());
}

void loop()
{
static uint16_t Sec=0;
static uint32_t StartMs=millis();

  if((millis() - StartMs) >= 1000)
  {
    StartMs = millis();
    Sec++;
    MySerial.print(F("Uptime: "));MySerial.print(Sec);MySerial.println(F(" sec")); /* Affiche le nombre de secondes écoulées depuis le démarrage */
  }
}
