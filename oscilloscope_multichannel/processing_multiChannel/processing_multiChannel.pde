// Programme processing
// généré avec le générateur de code Processing
// X. HINAULT - Avril 2011 - tous droits réservés
// www.mon-club-elec.fr

// ------- Licence du code de ce programme -----
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

/////////////// Description du programme ////////////
// Utilise le port Serie
// Utilise une police texte

// Utilise la librairie ControlP5

// Réalise un oscilloscope multivoie avec une carte Arduino
// Controle de réglage coefficient multiplicateur et base du tracé pour chaque voie

/////////////// ENTETE DECLARATIVE ////////////

// inclusion des librairies utilisées

import processing.serial.*; // importe la librairie série processing
import java.awt.Frame;
import java.awt.BorderLayout;

import controlP5.*; // importe la librairie GUI controlP5
// cette librairie doit être présente dans le répertoire /libraries du répertoire Processing
// voir ici : http://www.sojamo.de/libraries/controlP5/

// déclaration objets

// --- port Série ---
Serial  myPort; // Création objet désignant le port série

ControlP5 controlP5; // déclare un objet principal de la librairie GUI controlP5

// --- police texte ---
PFont fontA; // crée un objet de police texte

// déclaration variables globales

//------ déclaration des variables de couleur utiles ----
int jaune=color(255,255,0);
int vert=color(0,255,0);
int rouge=color(255,0,0);
int bleu=color(0,0,255);
int noir=color(0,0,0);
int blanc=color(255,255,255);
int bleuclair=color(0,255,255);
int violet=color(255,0,255);

//--- variables de configuration générales des tracés
int nombreVoies=6; // nombre de voies de mesure
boolean niveauxLogiques=false; // pour affichage niveaux logiques sur tracé
int heightTrace, widthTrace; // variable taille tracé
int decalageGauche=100;
int taillePoliceTrace=9;

// décla tableau  : int[] numbers = new int[3];
// ou : int[] numbers = { 90, 150, 30 };

 int[] xRef = new int [nombreVoies];         // tableau variable x de référence (coin inf gauche du tracé)
 int[] yRef= new int[nombreVoies];       // tableau variable y de référence (coin inf gauche du tracé)

 int[] couleurVoie= new int [6]; // tableau de variables de couleur du tracé des voies analogiques

 int[] xPos = new int [nombreVoies];         // tableau variable abscisse  - x
 int[] xPos0= new int[nombreVoies];       // tableau variable mémorisation xPos n-1

float[] yPos=new float [nombreVoies];  // tableau variable yPos - ordonnée
float[] yPos0=new float[nombreVoies]; // tableau variable yPos n-1

 int[] xTextPos= new int[nombreVoies]; // tableau variable position texte dans la fenêtre
 int[] yTextPos=new int[nombreVoies]; // tableau variable position texte dans la fenetre

 float[] coeff=new float [nombreVoies]; // tableau variables coeff mutiplicateur
 float[] base=new float [nombreVoies]; ; // tableau variables base du tracé en Volt

 boolean etatPause=false; // variable témoin appui sur bouton stop

// déclaration objets controlP5

Toggle togglePause; // déclare un/des objets Toggle
//Toggle [] togglePause= new Toggle[nombreVoies]; // crée un tableau de n Toggle t[i] - 1er indice est 0

//Slider sliderCoeff; // déclare un/ des objet Slider
Slider [] sliderCoeff= new Slider[nombreVoies]; // crée un tableau de n Slider sliderCoeff[i]- 1er indice est 0

//Slider sliderBase; // déclare un/ des objet Slider
Slider [] sliderBase= new Slider[nombreVoies]; // crée un tableau de n Slider sliderBase[i]- 1er indice est 0

// ControlWindow cw1; // déclare un/des objet fenetre ControlWindow
ControlFrame cw1;

/////////////// Fonction SETUP ////////////

void setup(){ // fonction d'initialisation exécutée 1 fois au démarrage

        // ---- initialisation paramètres graphiques utilisés
        colorMode(RGB, 255,255,255); // fixe format couleur R G B pour fill, stroke, etc...
        fill(0,0,255); // couleur remplissage RGB
        stroke (0,255,0); // couleur pourtour RGB
        strokeWeight(1); // largeur du trait en pixels
        rectMode(CORNER); // origine rectangle coin sup gauche

        // --- initialisation fenêtre de base ---
        //size(200, 200); // ouvre une fenêtre xpixels  x ypixels
        size(int(displayWidth*0.4), int(displayHeight*0.8));        
        // 90% de l'écran par défaut - régler au besoin -
        // viser taille maxi en hauteur - taille fenêtre (x,y)

        background(0,0,0); // fond fenetre en noir

        //----- initialise variables utilisées pour les graphiques

        // tracé en largeur pleine
        heightTrace=(height-20)/nombreVoies;
        widthTrace=width-decalageGauche;


        // variables indexées
        for (int i=0; i<nombreVoies; i++) { // passe en revue les éléments de tableau pour n voies

          xPos[i] =1;         // variable abscisse  - x
          xPos0[i]=1;       // variable mémorisation xPos n-1

          yPos[i]=1;  // variable yPos - ordonnée
          yPos0[i]=1; // variable yPos n-1

          xTextPos[i]=0; // variable position texte dans la fenêtre
          yTextPos[i]=50;

           //---- point sup gauche du tracé ---
          xRef[i]=decalageGauche;
          yRef[i]=heightTrace*i;

          //---- paramètres graphiques
          coeff[i]=1.0;
          base[i]=0.0;

        } //------------ fin for i

        //--- les couleurs des voies ----
        couleurVoie[0]=rouge;
        couleurVoie[1]=violet;
        couleurVoie[2]=jaune;
        couleurVoie[3]=bleuclair;
        couleurVoie[4]=blanc;
        couleurVoie[5]=vert;

// --- initialisation des fonctionnalités utilisées ---

        //------------- initialisation de la police texte - à mettre avant série ----
        fontA = loadFont("Dialog.plain-9.vlw"); // charge le fichier police dans l'objet police texte
        // ce fichier doit être présent dans un rép <data> dans le répertoire du programme
        // pour générer un fichier de police à partir des polices système aller dans Tools > create Fonts
        // voir également http://processing.org/learning/text/

        textFont(fontA, 9); // Initialise la police et sa taille (en pixels)

        //------------- initialisation port série ----
        println(Serial.list()); // affiche dans la console la liste des ports séries
        myPort = new Serial(this, Serial.list()[1], 115200); // Initialise une nouvelle instance du port Série
        //myPort = new Serial(this, "/dev/ttyACM0", 115200); // Initialise une nouvelle instance du port Série
        myPort.bufferUntil('\n'); // attendre arrivée d'un saut de ligne pour générer évènement série


//------- tracé initial ---------

        traceInitial();


//======== Initialisation Objets GUI ControlP5 =========

        controlP5 = new ControlP5(this); // initialise l'objet principal de la librairie GUI controlP5

        // typeObjet nomObjet=controlP5.addObjet(paramètres); // pour info : déclaration / initialisation possible en 1 ligne
        // Textfield field = controlP5.addTextfield("myWindowTextfield",70,130,100,20); // exemple

        //======== Initialisation Objets ControlWindow =========

        // addControlWindow(String theWindowName,int theX, int theY, int theWidth, int theHeight,int theFrameRate)
        cw1= addControlFrame("fenetre",250,500);// ajoute une fenetre au ControlP5

        // méthodes propres à l'objet ControlWindow
//        cw1.hideCoordinates(); //masque les coordonnées
//        cw1.setBackground(color(0,0,0));
        cw1.frameRate(15); // fixe le nombre de rafraichissement par seconde de la fenetre
        //cw1.setColorActive(color(255,0,0)); // fixe la couleur active
//        cw1.setTitle("Controle Oscillo"); // titre de la fenetre
        //cw1.setLocation(100, 100) ; // fixe la localisation dans la fenetre ?
        //cw1..setUndecorated(true); // fixe la bordure de la fenêtre

        // ajout de controles à la fenetre ControlWindow
        // nomObjet.setWindow(cw1); // met l'objet dans la fenêtre
        // b1.setWindow(cw1); // met l'objet dans la fenêtre
        // t1.setWindow(cw1); // met l'objet dans la fenêtre
        // s1.setWindow(cw1); // met l'objet dans la fenêtre

        //======== Initialisation Objets Toggle =========

        //for (int i=0; i<nombreVoies; i++) { // passe en revue les éléments de tableau pour n voies

        // addToggle(String theName, boolean theDefaultValue, float theX, float theY, int theWidth, int theHeight)
        togglePause=controlP5.addToggle("togglePause",false,175,10,20,20); // initialise et ajoute un Button au ControlP5

        // méthodes propres à l'objet Toggle
        togglePause.setMode(ControlP5.DEFAULT); // fixe le mode de fonctionnement du Toggle : ControlP5.DEFAULT ou ControlP5.SWITCH
        togglePause.setWindow(cw1); // met l'objet dans la fenêtre

        //t2.setMode(ControlP5.SWITCH); // fixe le mode de fonctionnement du Toggle : ControlP5.DEFAULT ou ControlP5.SWITCH

        // méthodes communes à tous les controles (objet Controller)
        togglePause.setLabelVisible(true); // affichage des labels
        togglePause.setLabel("Pause"); // fixe label objet
        togglePause.setDecimalPrecision(2); // fixe la précision
        togglePause.setColorActive(color(255,0,0)); // fixe la couleur active
        togglePause.setColorBackground(color(255,0,255)); // fixe couleur fond  
        togglePause.setColorForeground(color(0,0,255)); // fixe couleur avant
        togglePause.setColorCaptionLabel(color(0,255,255)); // fixe couleur Label
        togglePause.setColorValueLabel(color(0,0,255)); // fixe la couleur valeur

        // setImages(PImage theImageDefault,PImage theImageOver, PImage theImageActive,PImage theImageHighlight)
        // les images doivent etre de la meme taille que bouton, dans rép prog, type .jpg .png ..
        // un toggle n'utilise que image Default et Active
        //t1.setImages(loadImage("imageDefault.png"),loadImage("imageDefault.png"), loadImage("imageActive.png"),loadImage("imageDefault.png"));

//        } // --------------- fin for i toggle --------------

        //======== Initialisation Objets Sliders =========

        for (int i=0; i<nombreVoies; i++) { // passe en revue les éléments de tableau pour n voies

        // addSlider(theName, theMin, theMax, theDefaultValue, theX, theY, theW, theH)
        sliderCoeff[i]=controlP5.addSlider("sliderCoeff"+i,1,10,1,10,10+(i*80),100,15); // ajoute un Slider au ControlP5
        sliderCoeff[i].setWindow(cw1); // met l'objet dans la fenêtre
        //s1 = (Slider)controlP5.controller("MonSlider1"); // initialise l'objet Slider déclaré

        // méthodes propres à l'objet Slider
        sliderCoeff[i].setNumberOfTickMarks(10); // fixe le nombre crans - n+1 pour n valeurs
        //sliderCoeff.setNumberOfTickMarks((int(s1.max())+1); // fixe le nombre crans - n+1 pour n valeurs
        sliderCoeff[i].showTickMarks(true); // affichage des repères
        sliderCoeff[i].setSliderMode(Slider.FLEXIBLE); // fonctionnement du slider FLEXIBLE ou FIX

        // méthodes communes à tous les controles (objet Controller)
        sliderCoeff[i].setLabelVisible(true); // affichage des labels
        sliderCoeff[i].setLabel("Coefficient"); // fixe label objet
        sliderCoeff[i].setDecimalPrecision(2); // fixe la précision
        sliderCoeff[i].setColorActive(color(255,0,0)); // fixe la couleur active
        sliderCoeff[i].setColorBackground(color(255,255,0)); // fixe couleur fond  
        sliderCoeff[i].setColorForeground(color(0,0,255)); // fixe couleur avant
        //s1.setArrayValue(new float[] {100,255} ); // fixe les valeurs min/max du Slider ?
        sliderCoeff[i].setColorCaptionLabel(color(0,255,255)); // fixe couleur Label
        sliderCoeff[i].setColorValueLabel(color(0,0,255)); // fixe la couleur valeur

        //======== Initialisation Objets Sliders =========

        // addSlider(theName, theMin, theMax, theDefaultValue, theX, theY, theW, theH)
        sliderBase[i]=controlP5.addSlider("sliderBase"+i,0,4.5,0,10,40+(i*80),100,15); // ajoute un Slider au ControlP5
        sliderBase[i].setWindow(cw1); // met l'objet dans la fenêtre
        //s1 = (Slider)controlP5.controller("MonSlider1"); // initialise l'objet Slider déclaré

        // méthodes propres à l'objet Slider
        sliderBase[i].setNumberOfTickMarks(10); // fixe le nombre crans - n+1 pour n valeurs
        //sliderCoeff.setNumberOfTickMarks((int(s1.max())+1); // fixe le nombre crans - n+1 pour n valeurs
        sliderBase[i].showTickMarks(true); // affichage des repères
        sliderBase[i].setSliderMode(Slider.FLEXIBLE); // fonctionnement du slider FLEXIBLE ou FIX

        // méthodes communes à tous les controles (objet Controller)
        sliderBase[i].setLabelVisible(true); // affichage des labels
        sliderBase[i].setLabel("Base"); // fixe label objet
        sliderBase[i].setDecimalPrecision(2); // fixe la précision
        sliderBase[i].setColorActive(color(255,0,0)); // fixe la couleur active
        sliderBase[i].setColorBackground(color(255,255,0)); // fixe couleur fond  
        sliderBase[i].setColorForeground(color(0,0,255)); // fixe couleur avant
        //s1.setArrayValue(new float[] {100,255} ); // fixe les valeurs min/max du Slider ?
        sliderBase[i].setColorCaptionLabel(color(0,255,255)); // fixe couleur Label
        sliderBase[i].setColorValueLabel(color(0,0,255)); // fixe la couleur valeur

        } // --------- fin for i sliders --------------


    } // fin fonction Setup

/////////////// Fonction Draw ////////////

void  draw() { // fonction exécutée en boucle


        // while(true); // stoppe boucle draw

} // fin de la fonction draw()

/////////////// Autres Fonctions ////////////

        //------------- Fonction de gestion des évènements série ----
         void serialEvent (Serial myPort) { // fonction appelée lors de la survenue d'un évènement série

        // ******** Gestion de la valeur reçue sur le port série : **********

        String inString = myPort.readStringUntil('\n'); // chaine stockant la chaîne reçue sur le port Série
        // saut de ligne en marque de fin


        if ((inString != null) && (etatPause==false)) { // si la chaine recue n'est pas vide et toggle pause false


                inString = trim(inString); // enlève espace blancs de la chaine recue

                println("Chaine reçue="+inString); // debug

                //---- extraction des valeurs à partir de la chaine reçue ----
                // la chaine reçue avec la valeur des 6 mesures est sous la forme CAN=:val0:val1:val2:val3:val4:val5:=finCAN

                String[] inSubstring = split(inString, ':'); // extrait

                    println(inSubstring[0]); // debug

                //--- déclaration tableaux utiles
                int[] inByte_brut= new int[6]; // tableau pour valeur reçue en valeur numérique entiere
                float[] inByte= new float[6]; // tableau pour valeur reçue en valeur numérique décimale

                //---- extraction des valeurs de CAN à partir de la chaine
                for (int i=0; i<nombreVoies; i++) {

                     print("Valeur CAN("+i+")= "+ inSubstring[i+1]); // debug

                     inByte_brut[i]=int(inSubstring[i+1]); // conversion valeur reçue en valeur numérique entiere
                     print( " soit en valeur entière : " + inByte_brut[i]); // debug

                     inByte[i]=float(inSubstring[i+1]); // conversion valeur reçue en valeur numérique décimale
                     print( " soit en valeur décimale : " + inByte[i]); // debug

                     println("");
                }

                  println(inSubstring[7]); // debug - chaine de fin de chaine

           if ((match(inSubstring[0],"CAN=")!=null) && (match(inSubstring[7],"=finCAN")!=null)) { // si la chaine reçue est valide  

                  println("La chaine CAN reçue est valide ! "); // debug

          //xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx tracé des courbes xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

                 for (int nbTrace=0; nbTrace<nombreVoies; nbTrace++) { // trace toutes les voies nécessaires

                    inByte[nbTrace]=inByte[nbTrace]-(base[nbTrace]*1023.0/5.0); // adaptation de la base du tracé

                    // ré-échelonne la valeur pour affichage
                     inByte[nbTrace] = map(inByte[nbTrace], 0, 1023, 0, heightTrace); // adaptation valeur CAN à l'ordonnée tracé

                     yPos[nbTrace]=inByte[nbTrace]; // l'ordonnée est la valeur reçue par le port série

                     yPos[nbTrace]=yPos[nbTrace]*coeff[nbTrace]; // adaptation coeff

                    //********** affichage numérique de la valeur reçue et autres calculs *****

                     //---- calcul de la tension à afficher --------
                     float tension=float(inByte_brut[nbTrace]);
                     tension=tension*5000;
                     tension=tension/1023;
                     tension=tension; // adaptation tracé

                    // dessin d'un rectangle sous le texte avant affichage pour effacer texte précédent
                     fill(0,0,0); // couleur de remplissage idem fond
                     stroke( couleurVoie[nbTrace]); // fixe la couleur utilisée pour le tracé en RVB
                     strokeWeight(1); // largeur du trait en pixels
                     rect(xTextPos[nbTrace],  yRef[nbTrace]+yTextPos[nbTrace]-taillePoliceTrace-2 , 75, taillePoliceTrace+5);

                      // Use fill() to change the value or color of the text
                      fill(couleurVoie[nbTrace]); // couleur pour la police
                      text("         ", xTextPos[nbTrace]+2, yRef[nbTrace]+yTextPos[nbTrace]); //efface la valeur précédente
                      //text(inByte_brut, xTextPos, yTextPos);
                      textFont(fontA, 9);
                      text(tension+" mV", xTextPos[nbTrace]+2, yRef[nbTrace]+yTextPos[nbTrace]);


                      //*********** gestion du graphique *********

                     // trace la ligne
                     stroke( couleurVoie[nbTrace]); // fixe la couleur utilisée pour le tracé en RVB
                     strokeWeight(1); // largeur du trait en pixels


                     line (xRef[nbTrace]+xPos0[nbTrace],yRef[nbTrace]+heightTrace-yPos0[nbTrace],xRef[nbTrace]+xPos[nbTrace],yRef[nbTrace]+heightTrace-yPos[nbTrace]); // trace une ligne en tenant compte valeur reçue

                     xPos0[nbTrace]=xPos[nbTrace]; // mémorisation xPos n-1
                     yPos0[nbTrace]=yPos[nbTrace]; // mémorisation xPos n-1

                     // à la fin de l'écran revient au début
                     if (xPos[nbTrace] >= widthTrace) {
                       xPos[nbTrace] = 0;
                       xPos0[nbTrace]=0; // pour retour de la trace sans ligne

                       background(0,0,0); // couleur du fond


                      //------- tracé initial ---------

                      traceInitial();

                      }  // fin RAZ graphique

                     else {// si pas encore la fin du graphique
                       // incrémente la position horizontale (abscisse)
                       xPos[nbTrace]++;
                      } // fin else


                 } // fin for nbTrace

            } // fin si chaine valide

        } // fin condition chaine recue pas vide



} // fin de la fonction de gestion des évènements Série


void traceInitial() {


        for (int nbTrace=0; nbTrace<nombreVoies; nbTrace++) { // boucle pour autant de tracés que de voies


          for (int i=0; i<5; i++) {// tracé des lignes horizontales

            // --- initialisation avant chaque tracé
            fill(0,255,0); // couleur remplissage RGB
            stroke (0,255,0); // couleur pourtour RGB
            if (i==0) strokeWeight(2); else strokeWeight(1); // largeur du trait en pixels - 1er trait plus épais

            // trace ligne niveau tension
            line (xRef[nbTrace],yRef[nbTrace]+heightTrace-((heightTrace/5)*i), xRef[nbTrace] + widthTrace-1,yRef[nbTrace]+heightTrace-((heightTrace/5)*i));

            // affiche valeur tension de la ligne
            textFont(fontA, taillePoliceTrace);
            text(((i/coeff[nbTrace])+base[nbTrace])+"V ("+int(i*1023/5/coeff[nbTrace])+")", xRef[nbTrace]+5, yRef[nbTrace]+heightTrace-((heightTrace/5)*i)+taillePoliceTrace);

          }

          if (niveauxLogiques==true) { // si dessin niveau logique activé

            stroke (255,255,0); // couleur pourtour RGB
            strokeWeight(2); // largeur du trait en pixels
            fill(255,255,0); // couleur remplissage RGB

            // --- ligne niveau logique bas = 0.3 x Vcc pour carte Arduino (ATmega328) ----
            textFont(fontA, taillePoliceTrace);
            text("Niveau BAS", xRef[nbTrace]+5, (yRef[nbTrace]+heightTrace-(0.3*coeff[nbTrace]*heightTrace)-5));
            line (xRef[nbTrace]+0,(yRef[nbTrace]+heightTrace-(0.3*coeff[nbTrace]*heightTrace)), xRef[nbTrace]+widthTrace-1,(yRef[nbTrace]+heightTrace-(0.3*coeff[nbTrace]*heightTrace)));

            // --- ligne niveau logique haut = 0.6 x Vcc pour carte Arduino (ATmega328) ---
            textFont(fontA, taillePoliceTrace);
            text("Niveau HAUT", xRef[nbTrace]+5, (yRef[nbTrace]+heightTrace-(0.6*coeff[nbTrace]*heightTrace)-5));
            line (xRef[nbTrace]+0,(yRef[nbTrace]+heightTrace-(0.6*coeff[nbTrace]*heightTrace)), xRef[nbTrace]+widthTrace-1,(yRef[nbTrace]+heightTrace-(0.6*coeff[nbTrace]*heightTrace)));  

          } // fin si dessin niveaux logiques


        } // fin boucle défilement des tracés

        // affichage du coefficient courant
        //text(inByte_brut, xTextPos, yTextPos);
         fill(0,255,255);      
         textFont(fontA, 10);
        text("Coeff= x"+coeff,20, 10); // la coordonnée du text est point inf gauche

} // fin tracé initial



// Evènement Toggle
void togglePause(int theValue) { // fonction évènement Toggle de meme nom - reçoit la valeur

        println("Evènement Toggle Pause avec valeur = "+togglePause.getState());
        etatPause=togglePause.getState(); // met à jour la variable etat pause
}

// ------ gestion évènements Sliders Coeff- 1 fonction par voie ------

//----- voie 0 ---
void sliderCoeff0(float valeur) { // fonction évènement Slider de meme nom - reçoit la valeur

        println("Evènement Slider Coeff 0 avec valeur = "+valeur);
        coeff[0]=valeur; //MAJ variable coeff

        gestionSliderCoeff(0); // appelle la fonction commune avec indice voulu
}

//----- voie 1 ---
void sliderCoeff1(float valeur) { // fonction évènement Slider de meme nom - reçoit la valeur

        println("Evènement Slider Coeff 1 avec valeur = "+valeur);
        coeff[1]=valeur; //MAJ variable coeff

        gestionSliderCoeff(1); // appelle la fonction commune avec indice voulu
}

//----- voie 2 ---
void sliderCoeff2(float valeur) { // fonction évènement Slider de meme nom - reçoit la valeur

        println("Evènement Slider Coeff 2 avec valeur = "+valeur);
        coeff[2]=valeur; //MAJ variable coeff

        gestionSliderCoeff(2); // appelle la fonction commune avec indice voulu
}

//----- voie 3 ---
void sliderCoeff3(float valeur) { // fonction évènement Slider de meme nom - reçoit la valeur

        println("Evènement Slider Coeff 3 avec valeur = "+valeur);
        coeff[3]=valeur; //MAJ variable coeff

        gestionSliderCoeff(3); // appelle la fonction commune avec indice voulu
}

//----- voie 4 ---
void sliderCoeff4(float valeur) { // fonction évènement Slider de meme nom - reçoit la valeur

        println("Evènement Slider Coeff 4 avec valeur = "+valeur);
        coeff[4]=valeur; //MAJ variable coeff

        gestionSliderCoeff(4); // appelle la fonction commune avec indice voulu
}

//----- voie 5 ---
void sliderCoeff5(float valeur) { // fonction évènement Slider de meme nom - reçoit la valeur

        println("Evènement Slider Coeff 5 avec valeur = "+valeur);
        coeff[5]=valeur; //MAJ variable coeff

        gestionSliderCoeff(5); // appelle la fonction commune avec indice voulu
}

//----------- fonction commune de gestion des sliders Coeff appelée avec index i
void gestionSliderCoeff(int i) {


        // affichage du coefficient courant
        //fill(0,0,0);
        //text("                ", 10, 10); //efface la valeur précédente

        // dessin d'un rectangle sous le texte avant affichage pour effacer texte précédent
        fill(0,0,0); // couleur de remplissage idem fond
        stroke(0,0,0); // fixe la couleur utilisée pour le tracé en RVB
        strokeWeight(0); // largeur du trait en pixels
        rect(10, 0, 75, 10);

        //text(inByte_brut, xTextPos, yTextPos);
         fill(0,255,255);      
         textFont(fontA, 10);
        text("Coeff= x"+coeff[i],20, 10); // la coordonnée du text est point inf gauche

        delay(100); // évite prise en compte multiple

        //traceInitial();

}

// ------ gestion évènement Slider Base - 1 slider par voie  ------

//--- voie 0 ---
void sliderBase0(float valeur) { // fonction évènement Slider de meme nom - reçoit la valeur

        println("Evènement Slider Base 0 avec valeur = "+valeur);
        base[0]=valeur; // base
        delay(100); // évite prise en compte multiple
}

//--- voie 1 ---
void sliderBase1(float valeur) { // fonction évènement Slider de meme nom - reçoit la valeur

        println("Evènement Slider Base 1 avec valeur = "+valeur);
        base[1]=valeur; // base
        delay(100); // évite prise en compte multiple
}

//--- voie 2 ---
void sliderBase2(float valeur) { // fonction évènement Slider de meme nom - reçoit la valeur

        println("Evènement Slider Base 2 avec valeur = "+valeur);
        base[2]=valeur; // base
        delay(100); // évite prise en compte multiple
}

//--- voie 3 ---
void sliderBase3(float valeur) { // fonction évènement Slider de meme nom - reçoit la valeur

        println("Evènement Slider Base 3 avec valeur = "+valeur);
        base[3]=valeur; // base
        delay(100); // évite prise en compte multiple
}

//--- voie 4 ---
void sliderBase4(float valeur) { // fonction évènement Slider de meme nom - reçoit la valeur

        println("Evènement Slider Base 4 avec valeur = "+valeur);
        base[4]=valeur; // base
        delay(100); // évite prise en compte multiple
}

//--- voie 5 ---
void sliderBase5(float valeur) { // fonction évènement Slider de meme nom - reçoit la valeur

        println("Evènement Slider Base 5 avec valeur = "+valeur);
        base[5]=valeur; // base
        delay(100); // évite prise en compte multiple
}

ControlFrame addControlFrame(String theName, int theWidth, int theHeight) {
  Frame f = new Frame(theName);
  ControlFrame p = new ControlFrame(this, theWidth, theHeight);
  f.add(p);
  p.init();
  f.setTitle(theName);
  f.setSize(p.w, p.h);
  f.setLocation(100, 100);
  f.setResizable(false);
  f.setVisible(true);
  return p;
} 

// the ControlFrame class extends PApplet, so we 
// are creating a new processing applet inside a
// new frame with a controlP5 object loaded
public class ControlFrame extends PApplet {

  int w, h;

  int abc = 100;
  
  public void setup() {
    size(w, h);
    frameRate(25);
    cp5 = new ControlP5(this);
    cp5.addSlider("abc").setRange(0, 255).setPosition(10,10);
    cp5.addSlider("def").plugTo(parent,"def").setRange(0, 255).setPosition(10,30);
  }

  public void draw() {
      background(abc);
  }
  
  private ControlFrame() {
  }

  public ControlFrame(Object theParent, int theWidth, int theHeight) {
    parent = theParent;
    w = theWidth;
    h = theHeight;
  }


  public ControlP5 control() {
    return cp5;
  }
  
  
  ControlP5 cp5;

  Object parent;

  
}
//////////////// fin du programme //////////////////////////////
