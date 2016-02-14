#include <EEPROM.h>
#include <RcPpmFrame.h>     /* Librairie utilisee pour generer le train PPM pour fournir une horloge de synchronisation a RcTxSerial */
#include <RcTxSerial.h>     /* Librairie utilisee pour creer le port serie via ensemble RC (Cote Emetteur) */
#include <SoftRcPulseOut.h> /* Librairie utilisee pour simuler une voie du recepteur RC */
#include <TinyPinChange.h>  /* Librairie utilisee par <SoftRcPulseIn> */
#include <SoftRcPulseIn.h>  /* Librairie utilisee par <RcRxSerial> */
#include <RcRxSerial.h>     /* Librairie utilisee pour creer le port serie via ensemble RC (Cote Recepteur) */

/*
Ce sketch permet de tester les librairies <RcTxSerial> et <RcRxSerial> directement sur un arduino UNO sans Emetteur ni Recepteur RC.
L'emetteur RC est simule par un generateur de train d'impulsion PPM (librairie <RcPpmFrame>) et une lecture de tension analogique (analogRead()).
Le recepteur RC est simule par un objet de type SoftRcPulseOut (librairie <SoftRcPulseOut>).
Le sketch envoie via un objet RcTxSerial le nombre de secondes ecoulees depuis le demarrage du sketch.
Le message est recupere via un objet RcRxSerial et est affiche sur la Serial Console de l'EDI arduino via Serial (ATTENTION: configurer la Serial Console a 115200 bauds).

Par défaut, avec l'IDE arduino, la fréquence fixe de PWM de l'Arduino est de quelques 488 Hz (976 Hz pour les pins 5 et 6 de l'UNO), insuffisant pour générer une tension continue.
L'auteur a modifie le "core" arduino pour autoriser le PWM a 62.5KHz sur les broches 5 et 6 de l'UNO.
Cette modification a un impact sur les fonctions micros() et delay(): ces fonctions ont ete modifiees en consequence.
Les fichiers Arduino.h et wiring.c ont ete modifies pour qu'en definissant TIMER0_TICK_EVERY_X_CYCLES a 1 dans le fichier Arduino.h, la frequence du PWM a ete porte a 62.5KHz sur les broches 5 et 6.
Le fichier SoftRcPulseOut.c de la librairie <SoftRcPulseOut> a ete mis a jour en consequence car la modification a un impact sur le calcul de micros().

Le calcul du filtre RC du PWM est le suivant: (source: http://jlectronique.org/PWM.htm)
Nous decidons de filtrer avec un rapport de 20mV (ripple) pour 5V ce qui donne K=250.
La frequence de 'coupure' est donc de 62500/250 = 250 Hz
Les éléments du filtres sont donnés par l'équation : RC = 1 / (2 * pi * frequence de coupure), ce qui donne en theorie R = 12,4 K pour C = 47 nF. R = 10K sera suffisante.


                STRUCTURE DU SKETCH DE TEST
          DES LIBRAIRIES <RcTxSerial> ET <RcRxSerial>
           A L'AIDE DE LA LIBRAIRIE <RcPpmFrame>

                ,---------------------,
       RCTX_PPM |    <RcTxSerial>     | RCTX_PWM
  ,------------->                     >----###----+---,
  |Train PPM    |TxSerial.print(x sec)|    10K    |   |
  |             '---------------------'     47nF ===  |
  #                                               |   |
  # 1K          SIMULATEUR ENSEMBLE RC           GND  |
  #           ,-------------------------,\            |
  |           | ,---------------------, | |           | Tension Continue Variable
  |    PPM_GEN| |     <RcPpmFrame>    | | |           | (Simulation Potentiometre)
  '-------------<                     | | / Simulation|
              | |RcPpmFrame_Generate()| | >  Emetteur |
              | '---------------------' | \           |
              |                         | |           |
              | ,---------------------, | |           |
              | |                     | |/ A0         |
              | |     analogRead()    <---------------'
              | |   Largeur_us=map()  | |\
              | '----------V----------' | |
              |            |            | |
              | ,----------V----------, | / Simulation
    SIMURX_PPM| |  <SoftRcPulseOut>   | | > Recepteur
  ,-------------<                     | | \
  |           | | SimuRxCh.write_us() | | |
  #           | '---------------------' | |
  # 1K        '-------------------------'/   
  #             ,---------------------,
  |    RCRX_PPM |    <RcRxSerial>     |
  '------------->                     |
                |   RxSerial.read()   |
                '----------V----------'
                           |
                ,----------V----------,
                |       <Serial>      |
                |                     |
                |    Serial.write()   |
                '----------V----------'
                           |
                ,----------V----------,
                |     Console EDI     |
                |       Arduino       |
                |  Affichage: x sec   |
                '---------------------'

Ce sketch inclut un exemple d'interpreteur de commande qui permet de calibrer tres facilement le PWM Min et Max du RcTxSerial.

Liste des commande supportees: (/!\ Le port de la Serial Console doit etre configure a 19200 bauds avec "Retour chariot" /!\)

C=m: Passe en mode calibration min
m?: Renvoie la valeur courante de min
+: PwmMin=PwmMin+1
-: PwmMin=PwmMin-1

C=M: Passe en mode calibration Max
M?  Renvoie la valeur courante de Max
+: PwmMax=PwmMax+1
-: PwmMax=PwmMax-1

N: Passe en mode Normal (Sort du mode calibration)
S: Sauve la configuration courante (PwmMin et PwmMax) en EEPROM
Z: Zeroise l'EEPROM (re-calibration necessaire a la prochaine mise sous tension)

*/

#define RCTX_PWM_PIN    5 /* This Pin shall support PWM */
#define RCTX_PPM_PIN    2 /* PPM may be positive or negative: automatically detected */

#define PPM_GEN_PIN     4

#define RCRX_PPM_PIN    6
#define SIMURX_PPM_PIN  7

#define SCOPE_PIN       8 /* Used to check PPM clock is properly extracted */

RcTxSerial TxSerial(RCTX_PPM_PIN, RCTX_PWM_PIN, 32); /* data rate is # 200 bauds (40 ms per character) -> 25 characters per second max */

RcRxSerial RxSerial(RCRX_PPM_PIN);

SoftRcPulseOut SimuRxCh;

/* Debut declaration pour afficher la table des largeurs d'impulsion (pas indispensable) */
/*
1000                                                                 2000
  |---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
    0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F   I
*/
enum {NIBBLE_0=0, NIBBLE_1, NIBBLE_2, NIBBLE_3, NIBBLE_4, NIBBLE_5, NIBBLE_6, NIBBLE_7, NIBBLE_8, NIBBLE_9, NIBBLE_A, NIBBLE_B, NIBBLE_C, NIBBLE_D, NIBBLE_E, NIBBLE_F, NIBBLE_I, NIBBLE_NB};
#define PULSE_MIN_VAL 1000
#define PULSE_MAX_VAL 2000

#define PULSE_MIN(NibbleIdx)              (PULSE_MIN_VAL+(((NibbleIdx)*(PULSE_MAX_VAL-PULSE_MIN_VAL))/NIBBLE_NB))
#define PULSE_MAX(NibbleIdx)               PULSE_MIN((NibbleIdx)+1)
#define PULSE_MIN_MAX(NibbleIdx)          {PULSE_MIN(NibbleIdx),PULSE_MAX(NibbleIdx)}

typedef struct {
  uint16_t Min;
  uint16_t Max;
}NibblePulseSt_t;

NibblePulseSt_t NibblePulseTbl[NIBBLE_NB] PROGMEM ={
												PULSE_MIN_MAX(NIBBLE_0),
												PULSE_MIN_MAX(NIBBLE_1),
												PULSE_MIN_MAX(NIBBLE_2),
												PULSE_MIN_MAX(NIBBLE_3),
												PULSE_MIN_MAX(NIBBLE_4),
												PULSE_MIN_MAX(NIBBLE_5),
												PULSE_MIN_MAX(NIBBLE_6),
												PULSE_MIN_MAX(NIBBLE_7),
												PULSE_MIN_MAX(NIBBLE_8),
												PULSE_MIN_MAX(NIBBLE_9),
												PULSE_MIN_MAX(NIBBLE_A),
												PULSE_MIN_MAX(NIBBLE_B),
												PULSE_MIN_MAX(NIBBLE_C),
												PULSE_MIN_MAX(NIBBLE_D),
												PULSE_MIN_MAX(NIBBLE_E),
												PULSE_MIN_MAX(NIBBLE_F),
												PULSE_MIN_MAX(NIBBLE_I)
												};
#define TABLE_ITEM_NBR(Tbl)  (sizeof(Tbl)/sizeof(Tbl[0]))
/* Fin declaration pour afficher la table des largeurs d'impulsion (pas indispensable) */

#define LONGUEUR_MAX  50 /* Message le plus long */
static char Message[LONGUEUR_MAX+1];/* + 1 pour fin de chaine */

static uint8_t  PulseCpt;
static uint32_t Pulse_usCumul=0;

void setup()
{
  Serial.begin(115200);
  SimuRxCh.attach(SIMURX_PPM_PIN);
  RcPpmFrame_Init(PPM_GEN_PIN,4); /* 4 voies dans le train PPM */
  RcTxSerial::init(0); /* La zone EEPROM pour la classe RcTxSerial au debut de l'EEPROM */
  pinMode(SCOPE_PIN,OUTPUT);
  Serial.println("RcTxRxSerial demo: an unidirectional serial port at 200 bauds via RC");
  DisplayNibbleTbl();
}

void loop()
{
static uint32_t StartMs=millis(), StartUs;
static uint16_t CptSec=0;
static boolean PulseToGenerate=0;
char Char;
uint8_t PpmSynchro;
uint16_t AnaVal, RcRxPpm=2000;

  RcPpmFrame_Generate(); /* Genere un train d'impulsion PPM pour fournir une horloge a TxSerial: sert juste a demontrer que RcTxSerial est capabale d'extraire l'horloge PPM de periode 20 ms */
  if((millis()-StartMs)>=1000)
  {
    StartMs=millis();
    CptSec++;
    if(RcTxSerial::getMode()==RC_TX_SERIAL_MODE_NORMAL) /* N'envoyer les message que si l'on n'est pas en mode calibration */
    {
      TxSerial.print("Uptime: ");TxSerial.print(CptSec),TxSerial.println(" sec"); /* Envoi les secondes ecoulees via le port serie TxSerial depuis le demarrage */
    }
  }
  PpmSynchro=RcTxSerial::refresh(); /* Permet l'emission du caractere suivant en etant synchronise avec le train d'impulsion PPM de l'emetteur: renvoie 1 si demi-caractere envoye (maj PPM) */
  /* ---- Debut simulation de l'ensemble RC ---- */
  if(PpmSynchro)
  {
    StartUs=micros();
    static boolean State=HIGH;
    PulseToGenerate=1;
    digitalWrite(SCOPE_PIN,State);State=!State; /* juste pour verifier a l'oscilloscope (pas indispensable) */
//    Serial.println(millis()%100); /* Pour verifier que la synchro est detectee toutes les 20 ms environ */
  }
  if(PulseToGenerate && ((micros()-StartUs)>=2000)) /* Attend que le PWM soit bien etabli avant de lire l'entree analogique */
  {
    AnaVal=analogRead(A0);
    AnaVal=analogRead(A0);
    AnaVal=analogRead(A0); /* 3 mesures necessaires, sinon mesure pas assez precise */
    RcRxPpm=map(AnaVal,0,1023,900,2100); /* ici un petit map() pour etre sur d'avoir des impulsions de 900 à 2100 us */
//    Serial.print('A');Serial.print(AnaVal);Serial.print(' ');Serial.println(RcRxPpm);
    RcPpmFrame_SetChannelPulseWidth(1, RcRxPpm); /* Positionne la largeur d'impulsion de la voie 1 dans le train PPM (pour le fun parce que non utilisee) */
    SimuRxCh.write_us(RcRxPpm); /* Genere une impulsion RC comme le ferait un recepteur RC */
    SoftRcPulseOut::refresh(1); /* Generation immediate */
    PulseToGenerate=0;
  }
  /* ---- Fin simulation de l'ensemble RC ---- */

  /* ---- Debut affichage caracteres recus sur la console Arduino ---- */
  if(RcTxSerial::getMode()!=RC_TX_SERIAL_MODE_NORMAL)
  {
    if(RxSerial.nibbleAvailable())
    {
      PulseCpt++;
      Pulse_usCumul+=RxSerial.lastWidth_us();
      if(PulseCpt>=50)
      {
        Serial.print("P=");Serial.print(Pulse_usCumul/50);Serial.print(" N=");Serial.println(PulseWithToNibbleIdx(Pulse_usCumul/50));
        PulseCpt=0;
        Pulse_usCumul=0;
      }
    }
  }
  if(RxSerial.available())
  {
    Char=RxSerial.read(); /* Lecture par RxSerial des caracteres recus de TxSerial */
    if(RcTxSerial::getMode()==RC_TX_SERIAL_MODE_NORMAL)
    {
      Serial.write(Char);
    }
  }
  /* ---- Fin affichage caracteres recus sur la console Arduino ---- */
  
  /* -------- Debut Gestion des messages par le port serie --------- */
  TerminalClock(); /* Utilise pour la calibration */
  /* --------- Fin Gestion des messages par le port serie ---------- */
}

static void TerminalClock()
{
    if(LitMessage()>0)
    {
        InterpreteEtExecute();  
    }
}

#define RETOUR_CHARIOT		    '\r' /* '\r' = 0x0D (code ASCII) */
#define EFFACE_CARACTERE_PRECEDENT  8

static char LitMessage()
{
char Ret=-1;
char RxChar;
static uint8_t Idx=0;

    if(Serial.available() > 0)
    {
        RxChar=Serial.read();
        switch(RxChar)
        {
            case RETOUR_CHARIOT: /* Si retour chariot: fin de message */
            Message[Idx]=0;/* Remplace CR character par fin de chaine */
            Ret=Idx;
            Idx=0; /* Re-positionne index pour prochain message */
            break;
            
            case EFFACE_CARACTERE_PRECEDENT:
            if(Idx) Idx--;
            break;
            
            default:
            if(Idx<LONGUEUR_MAX)
            {
                Message[Idx]=RxChar;
                Idx++;
            }
            else Idx=0; /* Re-positionne index pour prochain message */
            break;
        }
    }
    return(Ret); 
}

#define Cal        'C'
#define Mini       'm'
#define Maxi       'M'
#define Ppm        'P'
#define Normal     'N'
#define Zero       'Z'
#define Save       'S'
#define COMMANDE   (Message[0])
#define ACTION     (Message[1])
#define MODE       (Message[2])
#define DEMANDE    '?'
#define ORDRE      '='
#define DECREMENTE '-'
#define INCREMENTE '+'
enum {ACTION_RIEN=0, ACTION_REPOND_AVEC_LA_REPONSE, ACTION_REPOND_COMMANDE_INCONNUE, ACTION_REPOND_HORS_PLAGE, ACTION_PAS_POSSIBLE_DANS_CE_CONTEXTE, ACTION_REPOND_ERREUR};
/*
C=m: Passe en mode calibration min
m?: Renvoie la valeur courante de min
+: PwmMin=PwmMin+1
-: PwmMin=PwmMin-1

C=M: Passe en mode calibration Max
M?  Renvoie la valeur courante de Max
+: PwmMax=PwmMax+1
-: PwmMax=PwmMax-1

N: Passe en mode Normal
S: Sauve la configuration courante en EEPROM
Z: Zeroise l'EEPROM (re-calibration necessaire a la prochaine mise sous tension)
*/

static void InterpreteEtExecute()
{
char Action=ACTION_REPOND_AVEC_LA_REPONSE;

    switch(COMMANDE) /* Test du premier caractère du message = Commande */
    {
        case Cal:
        switch(ACTION) /* Test du second caractère du message = Action */
        {
            case DEMANDE:
            ACTION='=';
            Message[2]=RcTxSerial::getMode()==RC_TX_SERIAL_MODE_TEST_MIN?'m':RcTxSerial::getMode()==RC_TX_SERIAL_MODE_TEST_MAX?'M':'N';
            Message[3]=0; /* Fin de chaine */
            break;
            
            case ORDRE:
            {
              switch(MODE)
              {
                case Mini: /* min */
                RcTxSerial::setMode(RC_TX_SERIAL_MODE_TEST_MIN);
                PulseCpt=0;
    	        strcpy_P(Message, PSTR("m: -/+ -> RX: 1030 us"));Serial.println(Message);
                Action=ACTION_RIEN;
                break;
                
                case Maxi: /* Max */
                RcTxSerial::setMode(RC_TX_SERIAL_MODE_TEST_MAX);
                PulseCpt=0;
    	        strcpy_P(Message, PSTR("M: -/+ -> RX: 1970 us"));Serial.println(Message);
                Action=ACTION_RIEN;
                break;
    
                case Normal: /* Mode Normal */
                RcTxSerial::setMode(RC_TX_SERIAL_MODE_NORMAL);
                break;
                
                default:
                Action=ACTION_REPOND_ERREUR;
                break;
              }
            }
            break;
            
            default:
            Action=ACTION_REPOND_ERREUR;
            break;
        }
        break;
        
        case Mini: /* min */
        switch(ACTION) /* Test du second caractère du message = Action */
        {           
            case DEMANDE:
            ACTION='=';
            itoa(TxSerial.getPwmMin(),Message+2,10);
            break;
            
            default:
            Action=ACTION_REPOND_ERREUR;
            break;
        }
        break;
        
        case Maxi: /* Max */
        switch(ACTION) /* Test du second caractère du message = Action */
        {            
            case DEMANDE:
            ACTION='=';
            itoa(TxSerial.getPwmMax(),Message+2,10);
            break;
            
            default:
            Action=ACTION_REPOND_ERREUR;
            break;
        }
        break;

        case DECREMENTE:
        switch(RcTxSerial::getMode())
        {
          case RC_TX_SERIAL_MODE_NORMAL:
          Action=ACTION_PAS_POSSIBLE_DANS_CE_CONTEXTE;
          break;
          
          case RC_TX_SERIAL_MODE_TEST_MIN:
          if(TxSerial.getPwmMin()) TxSerial.setPwmMin(TxSerial.getPwmMin()-1);
          else Action=ACTION_REPOND_HORS_PLAGE;
          break;
            
          case RC_TX_SERIAL_MODE_TEST_MAX:
          if(TxSerial.getPwmMax()) TxSerial.setPwmMax(TxSerial.getPwmMax()-1);
          else Action=ACTION_REPOND_HORS_PLAGE;
          break;
        }
        break;
        
        case INCREMENTE:
        switch(RcTxSerial::getMode())
        {
          case RC_TX_SERIAL_MODE_NORMAL:
          Action=ACTION_PAS_POSSIBLE_DANS_CE_CONTEXTE;
          break;
          
          case RC_TX_SERIAL_MODE_TEST_MIN:
          if(TxSerial.getPwmMin()<255) TxSerial.setPwmMin(TxSerial.getPwmMin()+1);
          else Action=ACTION_REPOND_HORS_PLAGE;
          break;
            
          case RC_TX_SERIAL_MODE_TEST_MAX:
          if(TxSerial.getPwmMax()<255) TxSerial.setPwmMax(TxSerial.getPwmMax()+1);
          else Action=ACTION_REPOND_HORS_PLAGE;
          break;
        }
	break;

        case Zero: /* Zeroise l'EEPROM: re-calibration necessaire a la prochaine remise sous tension */
        RcTxSerial::init(0, 1);
	break;

        case Save: /* Sauve la config dans l'EEPROM */
        RcTxSerial::saveSetupInEeprom();
	break;

        default:
        Action=ACTION_REPOND_COMMANDE_INCONNUE;
        break;
    }
    switch(Action)
    {
        case ACTION_RIEN: /* Reste muet */
        break;
        
        case ACTION_REPOND_AVEC_LA_REPONSE:
        strcat(Message,"\r");/* Juste pour avoir le retour a la ligne */
        Serial.println(Message);
        break;
        
        case ACTION_REPOND_COMMANDE_INCONNUE:
	strcpy_P(Message, PSTR("COMMANDE INCONNUE!"));Serial.println(Message);
        break;
        
        case ACTION_REPOND_HORS_PLAGE:
	strcpy_P(Message, PSTR("VALEUR HORS PLAGE!"));Serial.println(Message);
        break;
        
        case ACTION_PAS_POSSIBLE_DANS_CE_CONTEXTE:
	strcpy_P(Message, PSTR("PAS POSSIBLE DANS CE CONTEXTE!"));Serial.println(Message);
        break;
        
        case ACTION_REPOND_ERREUR:
	strcpy_P(Message, PSTR("ERREUR!"));Serial.println(Message);
        break;
    }
}

/* Les fonctions suivantes sont juste necessaires pour afficher la table des impulsions (pas indispensable) */
static void DisplayNibbleTbl()
{
uint8_t Idx;
uint16_t Moy;

  Serial.print("Idx");Serial.print("\t");Serial.print("Min");Serial.print("\t");Serial.print("Moy");Serial.print("\t");Serial.println("Max");
  for(Idx=0;Idx<TABLE_ITEM_NBR(NibblePulseTbl);Idx++)
  {
    Moy=(pgm_read_word(&NibblePulseTbl[Idx].Min)+pgm_read_word(&NibblePulseTbl[Idx].Max))/2;
    Serial.print(Idx);Serial.print("\t");Serial.print(pgm_read_word(&NibblePulseTbl[Idx].Min));Serial.print("\t");Serial.print(Moy);Serial.print("\t");Serial.println(pgm_read_word(&NibblePulseTbl[Idx].Max));
  }  
}

static int8_t PulseWithToNibbleIdx(uint16_t PulseWidth)
{
int8_t Ret=-1, Idx;
  for(Idx=0;Idx<TABLE_ITEM_NBR(NibblePulseTbl);Idx++)
  {
    if( (PulseWidth>=pgm_read_word(&NibblePulseTbl[Idx].Min)) && (PulseWidth<=pgm_read_word(&NibblePulseTbl[Idx].Max)) )
    {
      Ret=Idx;
      break;
    }
  }
  return(Ret);
}

