// --- Programme Arduino ---
// par X. HINAULT - 01/2010

// --- Que fait ce programme ? ---
/* Affiche le résultat brut d'une conversion analogique numérique sur la fenêtre terminal du PC*/

// --- Fonctionnalités utilisées ---
// Utilise la connexion série vers le PC
// Utilise la conversion analogique numérique 10bits sur les voies analogiques  analog 0,

// --- Circuit à réaliser ---
// Connexion série entre la carte Arduino et le PC (utilise les broches 0 et 1)  

// Broche Analog 0 (=broche 14) en entrée Analogique  : connecter la sortie d'une résistance Variable 10K elle-même connectée entre 0 et 5V

//**************** Entête déclarative *******
// A ce niveau sont déclarées les librairies, les constantes, les variables...

// --- Inclusion des librairies utilisées ---

// --- Déclaration des constantes ---

// --- constantes des broches ---

const int Voie_0=0; //declaration constante de broche analogique

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

// acquisition conversion analogique-numerique (CAN) sur la voie analogique
mesure_brute=analogRead(Voie_0);

// affiche valeur numerique entière ou à virgule au format décimal
Serial.println(mesure_brute);

delay(10); // fixe une pause entre 2 mesures
// plus le délai est court, plus la trace est rapide

} // fin de la fonction loop() - le programme recommence au début de la fonction loop sans fin
// ********************************************************************************

// --- Fin programme ---
