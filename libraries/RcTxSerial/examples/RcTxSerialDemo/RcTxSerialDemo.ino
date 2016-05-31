/*
Ce sketch demontre qu'en remplacant un potentiometre d'emetteur RC par un ATtiny, il est possible de construire un port serie unidirectionnel.
Afin de synchroniser l'emission des caracteres avec le train d'impulsion PPM, il est necessaire de connecter le signal PPM de l'emetteur sur l'ATtiny.
Il faut 2 impulsions PPM pour transmettre un caractere soit environ 40ms. Il est donc possible d'atteindre un debit de 200 bauds (25 caracteres par seconde).
Dans cet exemple, le nombre de secondes ecoulees depuis le demarrage du sketch est emis toutes les secondes.
Sur un ATiny85, il restera 3 I/O de libre pour l'utilisateur.
Cote recepteur RC, sur la voie correspondante, il est necessaire de connecter un arduino avec RcRxSerial (Voir Exemple->RcRxSerial->RcRxSerialDemo)

                                     ATtiny85
                                  ,-------------.
      ,-------- +5V         I/O---+         VDD +-----------< +5V
      |                           |             |
      #                           |             |0  10K
  POT #<------- C      ==>  I/O---+         PWM +---###--+--> C
      #           remplacé par    |             |       === 47nF
      |                           |             |       GND
      '-------- -           I/O---+         GND +-----------< -
                                  '------+------'
                                         |1
                                         '---------------< PPM (Ecolage/Trainer)

*/
#include <EEPROM.h>
#include <RcTxSerial.h>

#define RCTX_PPM_PIN  1
#define RCTX_PWM_PIN  0

RcTxSerial TxSerial(RCTX_PPM_PIN, RCTX_PWM_PIN, 32); /* Taille minimale desiree pour la FIFO d'emission: 32 octets */

void setup()
{
  RcTxSerial::init(0); /* La zone EEPROM pour la classe RcTxSerial commence a l'index 0 */
  RcTxSerial::setMode(RC_TX_SERIAL_MODE_TEST_MIN); /* RC_TX_SERIAL_MODE_TEST_MIN pour calibrer le min, RC_TX_SERIAL_MODE_TEST_MAX pour calibrer le max, RC_TX_SERIAL_MODE_NORMAL quand calibration finie */
  TxSerial.setPwmMin(20); /* Valeur du PWM a l'emission pour avoir 1029 us a la reception (Calibration de l'impulsion Min) */
  TxSerial.setPwmMax(240);/* Valeur du PWM a l'emission pour avoir 1970 us a la reception (Calibration de l'impulsion Max)*/
}

void loop()
{
static uint32_t StartMs=millis();
uint16_t CptSec=0;
  
  if((millis()-StartMs)>=1000) /* il faut cadencer l'emission: pas plus de 25 caracteres par seconde (200 bauds) */
  {
      StartMs=millis();
      CptSec++;
      TxSerial.print("Uptime: ");TxSerial.print(CptSec),TxSerial.println(" sec"); /* Envoi les secondes ecoulees depuis le demarrage via le port serie TxSerial */
  }
}

