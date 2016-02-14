/*
Ce sketch demontre qu'en remplacant un potentiometre d'emetteur RC par un ATtiny (ou UNO), il est possible de construire un port serie unidirectionnel a 200 bauds.
Il faut 2 impulsions PPM pour transmettre un caractere soit environ 40ms. Il est donc possible d'atteindre un debit de 200 bauds (25 caracteres par seconde).
Cote emetteur RC, sur la voie correspondante, il est necessaire de connecter un arduino avec RcTxSerial (Voir Exemple->RcTxSerial->RcTxSerialDemo)
Dans cet exemple, le nombre de secondes ecoulees depuis le demarrage du sketch de l'emetteur est recupere dans un port serie RcRxSerial connecte sur la voie du recepteur
et est affiche dans la Serial Console de l'EDI arduino sur le PC.


    Recepteur RC                    Arduino UNO     
,-----------------,              ,--------------,
|                 |             6|              |
|            Voie +-------------->              |              PC
|                 |              |              |     ,----------------,
|                 |              |              |     |                |
|                 |              | ,---,        |     | Serial Console |
|                 |              | |   |        |     |                |
'-----------------'              '-'-+-'--------'     +----------------+
                                     |       USB      |                |
                                     '----------------+                |
                                                      |                |
                                                      '----------------'
*/
#include <TinyPinChange.h>  /* Librairie utilisee par <SoftRcPulseIn> */
#include <SoftRcPulseIn.h>  /* Librairie utilisee par <RcRxSerial> */
#include <RcRxSerial.h>     /* Librairie utilisee pour creer le port serie via ensemble RC (Cote Recepteur) */

#define RCRX_PPM_PIN    6

RcRxSerial RxSerial(RCRX_PPM_PIN);

void setup()
{
  Serial.begin(9600); /* Initialise le port serie pour la Console Arduino du PC */
}

void loop()
{
char Char;

  if(RxSerial.available())
  {
    Char=RxSerial.read(); /* Lecture par RxSerial des caracteres recus de TxSerial */
    Serial.write(Char);   /* Affichage des caracteres recus dans la Serial Console de l'EDI arduino */
  }
  
}

