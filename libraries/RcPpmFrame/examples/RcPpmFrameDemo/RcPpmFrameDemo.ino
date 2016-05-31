#include <RcPpmFrame.h>     /* Librairie utilisee pour generer le train PPM */

#define PPM_GEN_PIN  2 /* Broche de sotie du train PPM */

void setup()
{
  RcPpmFrame_Init(PPM_GEN_PIN,4); /* 4 voies dans le train PPM */
//  RcPpmFrame_SetPolar(RC_PPM_FRAME_POL_NEG); /* Decommenter cette ligne pour generer un PPM avec modulation Negative */
  RcPpmFrame_SetChannelPulseWidth(1, 1000); /* Positionne la largeur d'impulsion de la voie 1 dans le train PPM */
  RcPpmFrame_SetChannelPulseWidth(2, 1500); /* Positionne la largeur d'impulsion de la voie 2 dans le train PPM */
  RcPpmFrame_SetChannelPulseWidth(3, 1700); /* Positionne la largeur d'impulsion de la voie 3 dans le train PPM */
  RcPpmFrame_SetChannelPulseWidth(4, 2000); /* Positionne la largeur d'impulsion de la voie 4 dans le train PPM */
}

void loop()
{
  RcPpmFrame_Generate(); /* Genere un train d'impulsion PPM */
}

