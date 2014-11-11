// --- Programme Arduino ---
// Copyright X. HINAULT - Créé le 10/04/2010
// www.mon-club-elec.fr
// http://www.mon-club-elec.fr/pmwiki_mon_club_elec/pmwiki.php?n=MAIN.ArduinoExpertCapteursComplexesDS18B20Test
//  Code sous licence GNU GPL :
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License,
//  or any later version.
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//  

// --- Que fait ce programme ? ---
/* test simple de la mesure de la température à l'aide d'un capteur
de température One-wire de type DS18B20 et affichage du résultat
dans la fenêtre Terminal série.  */

// --- Fonctionnalités utilisées ---
// Utilise la connexion série vers le PC
// Utilise un ou plusieurs capteurs One Wire

// --- Circuit à réaliser ---
// Connexion série entre la carte Arduino et le PC (utilise les broches 0 et 1)  
// Connecter  sur la broche 2 la broche de données du capteur One Wire

//**************** Entête déclarative *******
// A ce niveau sont déclarées les librairies, les constantes, les variables...

// --- Inclusion des librairies utilisées ---

#include <OneWire.h> // librairie pour capteur OneWire

// --- Déclaration des constantes ---

// --- constantes des broches ---

const int broche_OneWire=2; //declaration constante de broche

//---- code d'instruction du capteur
const int modeLecture=0xBE;
const int lancerMesure=0x44;

// --- Déclaration des variables globales ---
byte data[12]; // Tableau de 12 octets pour lecture des 9 registres de RAM et des 3 registres d'EEPROM du capteur One Wire
byte adresse[8]; // Tableau de 8 octets pour stockage du code d'adresse 64 bits du composant One Wire

int tempet=0; // variable pour resultat de la mesure
float tempetf=0.0; // variable pour resultat de la mesure

// --- Déclaration des objets utiles pour les fonctionnalités utilisées ---
OneWire  capteur(broche_OneWire);  // crée un objet One Wire sur la broche voulue


//**************** FONCTION SETUP = Code d'initialisation *****
// La fonction setup() est exécutée en premier et 1 seule fois, au démarrage du programme

void setup()   { // debut de la fonction setup()

// --- ici instructions à exécuter au démarrage ---

Serial.begin(115200); // initialise connexion série à 115200 bauds
// IMPORTANT : régler le terminal côté PC avec la même valeur de transmission


// XXXXXXXXXXXXXXXXXXXXXX Détection du capteur présent sur la broche XXXXXXXXXXXXXXXXXXXXXXX
Serial.println("**** Detection du capteur **** ");

while (capteur.search(adresse)== false) // tant qu'aucun nouveau capteur est détecté
{

  // la fonction search renvoie la valeur FAUX si aucun élément 1-wire est trouvé.

  Serial.println("Aucun capteur 1-wire present sur la broche ! "); // affiche message + saut de ligne
  delay (1000); // pause 1 seconde
}

//la suite est exécutée seulement si un capteur est détecté

  // la fonction search renvoie la valeur VRAI si un élément 1-wire est trouvé.
  // Stocke son code d'adresse 16 bits dans le tableau adresse[8]
  // adresse envoyé à la fonction correspond à l'adresse de début du tableau adresse[8] déclaré ...  

  Serial.print ("1 capteur 1-wire present avec code adresse 64 bits : ");


  //--- affichage des 64 bits d'adresse au format hexadécimal
   for(int i = 0; i < 8; i++) { // l'adresse renvoyée par la fonction search est stockée sur 8 octets

    if (adresse[i]<16) Serial.print('0'); // pour affichage des O poids fort au format hexadécimal
    Serial.print(adresse[i], HEX); // affiche 1 à 1 les 8 octets du tableau adresse au format hexadécimal
    Serial.print(" ");
  }

  Serial.println();

  //---- test du type de capteur ----
  // le type du capteur est donné par le 1er octet du code adresse 64 bits
  // Valeur 0x28 pour capteur type DS18B20, 0x10 pour type DS18S20, 0x22 pour type DS1820
  if (adresse[0]==0x28)
  {
    Serial.println ("Type du capteur present : Capteur temperature DS18B20.");
  }
  else
  {
    Serial.println ("Le capteur present n'est pas un capteur de temperature DS18B20.");
  }


  //----- contrôle du code CRC ----
  // le dernier octet de l'adresse 64bits est un code de contrôle CRC
  // à l'aide de la fonction crc8 on peut vérifier si ce code est valide
  if (capteur.crc8( adresse, 7) == adresse[7]) // vérification validité code CRC de l'adresse 64 bits
  // le code CRC de l'adresse 64 bits est le 8ème octet de l'adresse (index 7 du tableau)
  {
    Serial.println ("Verification du code CRC de l'adresse 64 bits de ce capteur : VALIDE !");
  }
  else
  {
    Serial.println ("Verification du code CRC de l'adresse 64 bits de ce capteur : NON VALIDE !");    
  }

  //------- message final détection ----
  Serial.println("----- fin de la recherche du capteur ----");
  Serial.println("");

  //--- la phase de détection n'est pas obligatoire avec un seul capteur ---
  //---- il est possible d'utiliser uniquement la fonction skip ---
  //---- le résultat semble plus hasardeux cependant...
  // capteur.skip();

  // une fois le capteur détecté, son code adresse 64 bits est stocké dans le tableau adresse[8]
  // on va à présent adresser des instructions au capteur et lire le résultat obtenu
  // successivement il faudra :
  //    * envoyer l'instruction de lancement de la mesure
  //    * attendre 1 seconde
  //    * envoyer l'instruction de lecture du résultat
  //    * envoyer les instructions de lecture



} // fin de la fonction setup()
// ********************************************************************************

//*************** FONCTION LOOP = Boucle sans fin = coeur du programme *************
// la fonction loop() s'exécute sans fin en boucle aussi longtemps que l'Arduino est sous tension

void loop(){ // debut de la fonction loop()

// XXXXXXXXXXXXXXXXXXXXXX Lancement d'une mesure et lecture du résultat XXXXXXXXXXXXXXXXXXXXXXX

//  Serial.println("**** Acquisition d'une mesure de la temperature **** ");

  // avant chaque nouvelle instruction, il faut :
  //    * initialiser le bus 1-wire
  //    * sélectionner le capteur détecté
  //    * envoyer l'instruction

//--------- lancer une mesure --------
  capteur.reset(); // initialise le bus 1-wire avant la communication avec un capteur donné
  capteur.select(adresse); // sélectionne le capteur ayant l'adresse 64 bits contenue dans le tableau envoyé à la fonction
  capteur.write(lancerMesure,1); // lance la mesure et alimente le capteur par la broche de donnée

//-------- pause d'une seconde -----
delay(1000);     // au moins 750 ms
           // il faudrait mettre une instruction capteur.depower ici, mais le reset va le faire


//---------- passer en mode LECTURE -------------
  capteur.reset(); // initialise le bus 1-wire avant la communication avec un capteur donné
  capteur.select(adresse); // sélectionne le capteur ayant l'adresse 64 bits contenue dans le tableau envoyé à la fonction
  capteur.write(modeLecture,1); // passe en mode lecture de la RAM du capteur

// ----------- lire les 9 octets de la RAM (appelé Scratchpad) ----

for ( int i = 0; i < 9; i++) {           // 9 octets de RAM stockés dans 9 octets
    data[i] = capteur.read();             // lecture de l'octet de rang i stocké dans tableau data
  }

// ----- affichage du contenu des différents octets -----------
//Serial.println("");
//Serial.println("---- lecture de la RAM du capteur ---- ");
//Serial.print("Octet 0 (Resultat poids faible)="), Serial.println(data[0],BIN);
//Serial.print("Octet 1 (Resultat poids fort)="), Serial.println(data[1],BIN);
//Serial.print("Octet 2 (Alarme haute)="), Serial.println(data[2],BIN);
//Serial.print("Octet 3 (Alarme basse)="), Serial.println(data[3],BIN);
//Serial.print("Octet 4 (Registre de configuration)="), Serial.println(data[4],BIN);
//Serial.print("Octet 5 (Reserve)="), Serial.println(data[5],BIN);
//Serial.print("Octet 6 (Reserve)="), Serial.println(data[6],BIN);
//Serial.print("Octet 7 (Reserve)="), Serial.println(data[7],BIN);
//Serial.print("Octet 8 (code CRC mesure)="), Serial.println(data[8],BIN);


//----- test de validité des valeurs reçues par contrôle du code CRC  ----

//Serial.println("");
//Serial.println("---- test de controle de validite des donnees recues ---- ");

  // le dernier (9ème) octet de la RAM est un code de contrôle CRC
  // à l'aide de la fonction crc8 on peut vérifier si ce code est valide
  if (capteur.crc8( data, 8) == data[8]) // vérification validité code CRC des valeurs reçues
  {
    //Serial.println ("Verification du code CRC de ce resultat : VALIDE !");
  }
  else
  {
    Serial.println ("Verification du code CRC de ce resultat  : NON VALIDE !");    
  }

//----- caclul de la température mesurée (enfin!) ---------
//Serial.println("");
//Serial.println("---- calcul de la temperature ---- ");

//---- extraction du résultat de la mesure à partir des registres de la RAM ---
data[1]=data[1] & B10000111; // met à 0 les bits de signes inutiles
tempet=data[1]; // bits de poids fort
tempet=tempet<<8;
tempet=tempet+data[0]; // bits de poids faible

//Serial.print ("Mesure brute =");
//Serial.println (tempet);

// --- en mode 12 bits, la résolution est de 0.0625°C - cf datasheet DS18B20
tempetf=float(tempet)*6.25;
tempetf=tempetf/100.0;

Serial.print ("Mesure Finale = ");
Serial.print (tempetf,2);
Serial.println (" Degres Celsius. ");

  delay(1000);
//while(1); // stoppe loop

} // fin de la fonction loop() - le programme recommence au début de la fonction loop sans fin
// ********************************************************************************

//*************** Autres Fonctions du programme *************

// --- Fin programme ---
