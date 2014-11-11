
// --- Programme Arduino ---
// can_Graph_pc_v3
// par X. HINAULT - 03/2011
// wwww.mon-club-elec.fr

// --- Que fait ce programme ? ---
/* Envoie la valeur brute de la conversion analogique sur 6 voies, sur le port série
sous la forme "CAN=:val0:val1:val2:val3:val4:val5:=finCAN"
*/

// --- Fonctionnalités utilisées ---
// Utilise la connexion série vers le PC
// Utilise la conversion analogique numérique 10bits sur les 6 voies analogiques

// --- Circuit à réaliser ---
// Connexion série entre la carte Arduino et le PC (utilise les broches 0 et 1)  

// Broches analogiques : connecter les tension à mesurer sur les broches

//**************** Entête déclarative *******
// A ce niveau sont déclarées les librairies, les constantes, les variables...

// --- Inclusion des librairies utilisées ---

// --- Déclaration des constantes ---

// --- constantes des broches ---

const int Voie[6]={0,1,2,3,4,5}; //declaration constante de broche analogique

// --- Déclaration des variables globales ---
int mesure_brute=0;// Variable pour acquisition résultat brut de conversion analogique numérique


// --- Initialisation des fonctionnalités utilisées ---

//**************** FONCTION SETUP = Code d'initialisation *****
// La fonction setup() est exécutée en premier et 1 seule fois, au démarrage du programme

void setup()   { // debut de la fonction setup()

// --- ici instructions à exécuter au démarrage ---

Serial.begin(115200); // initialise connexion série à 115200 bauds
// IMPORTANT : régler le terminal côté PC avec la même valeur de transmission




} // fin de la fonction setup()
// ********************************************************************************

//*************** FONCTION LOOP = Boucle sans fin = coeur du programme *************
// la fonction loop() s'exécute sans fin en boucle aussi longtemps que l'Arduino est sous tension

void loop(){ // debut de la fonction loop()

// --- ici instructions à exécuter par le programme principal ---

//---- envoie sur le port série les 6 mesures sous la forme CAN=:val0:val1:val2:val3:val4:val5:=finCAN
//--- les : sont utilisés comme séparateur par le programme Processing cté PC

Serial.print("CAN=:"); // chaine de début

for (int i=0; i<6; i++) { // défile les vois analogiques

  // acquisition conversion analogique-numerique (CAN) sur la voie analogique
  mesure_brute=analogRead(Voie[i]);

  // affiche valeur numerique entière ou à virgule au format décimal
  Serial.print(mesure_brute);
  Serial.print(":"); // séparateur entre les valeurs

} // fin for i

  Serial.println("=finCAN"); // fin de la chaine + saut de ligne

  delay(50); // fixe une pause entre 2 mesures
  // plus le délai est court, plus la trace est rapide

} // fin de la fonction loop() - le programme recommence au début de la fonction loop sans fin
// ********************************************************************************

// --- Fin programme ---
