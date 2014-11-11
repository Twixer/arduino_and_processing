/* Processing code for this example

 // Graphing sketch


 // Ce programme reçoit des chaine de caractère ASCII
 // depuis le port Série à 115200 bauds et les affiche sous forme de courbe
 // dans une fourchette de 0 to 1023,
 //lorsque les données sont suivies d'un retour de ligne

 // Created 20 Apr 2005
 // Updated 18 Jan 2008
 // by Tom Igoe
 //
 // modifié par X. HINAULT - January 2010
 // www.mon-club-elec.fr
 */

 import processing.serial.*;

 Serial myPort;        // Variable Port Série
 int xPos = 1;         // variable abscisse  - x
 int xPos0=1;       // variable mémorisation xPos n-1

float yPos=1;  // variable yPos - ordonnée
float yPos0=1; // variable yPos n-1

 void setup () {

 // initialise la fenêtre
 size(int(displayWidth*0.9), int(displayHeight*0.9));        
// 90% de l'écran par défaut - régler au besoin -
// viser taille maxi en hauteur - taille fenêtre (x,y)


 // Liste tous les ports disponible et affiche le résultat
 println(Serial.list());

 // Le port COM3 est listé avec l'indice 1
 // donc j'ouvre le port Serial.list()[1].
 // A adapter si votre port est différent - cf liste qui s'affiche à l'exécution
 myPort = new Serial(this, Serial.list()[1], 115200);
 // ne génère aucun évènement Série tant qu'aucun caractère saut de ligne n'est reçu
 myPort.bufferUntil('\n');
 // initialise le fond de la fenêtre
 background(255);// 0 = noir - 255 = blanc
 }
 void draw () {
 // tout se passe dans la fonction SerialEvent car le programme reçoit des données
 }

 void serialEvent (Serial myPort) {
 // mémorise la chaîne de caractères reçue
 String inString = myPort.readStringUntil('\n');

 if (inString != null) {
 // enlève les espaces
 inString = trim(inString);
 // convertit la chaîne en valeur numérique
 float inByte = float(inString);
 // ré-échelonne la valeur pour affichage
 inByte = map(inByte, 0, 1023, 0, height);

 yPos=inByte; // l'ordonnée est la valeur reçue par le port série

 // trace la ligne
 stroke(0,0,255); // fixe la couleur utilisée pour le tracé en RVB

 line (xPos0,height-yPos0,xPos,height-yPos); // trace une ligne en tenant compte valeur reçue

 xPos0=xPos; // mémorisation xPos n-1
 yPos0=yPos; // mémorisation xPos n-1

 // à la fin de l'écran revient au début
 if (xPos >= width) {
 xPos = 0;
 xPos0=0; // pour retour de la trace sans ligne

 background(255); // 0 pour noir - 255 pour blanc...  
 }
 else {
 // incrémente la position horizontale (abscisse)
 xPos++;
 }
 }
 }

