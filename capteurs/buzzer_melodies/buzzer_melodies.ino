
// http://therandombit.wordpress.com/2011/11/21/arduino-piezo-speaker-super-mario/
// http://pastebin.com/UhsHePFv

//Périodes de chaque note (*0.0000001)
#define tone_C 1911
#define tone_C1 1804
#define tone_D 1703
#define tone_Eb 1607
#define tone_E 1517
#define tone_F 1432
#define tone_F1 1352
#define tone_G 1276
#define tone_Ab 1204
#define tone_A 1136
#define tone_Bb 1073
#define tone_B 1012
#define tone_c 955
#define tone_c1 902
#define tone_d 851
#define tone_eb 803
#define tone_e 758
#define tone_f 716
#define tone_f1 676
#define tone_g 638
#define tone_ab 602
#define tone_a 568
#define tone_bb 536
#define tone_b 506

#define tone_p 0  //pause

const int PIEZO = 6;

long vel = 20000;

// mario
static int mario_m[] = {
  tone_e, tone_e, tone_e, tone_c, tone_e, tone_g, tone_G, tone_c, tone_G, tone_E, tone_A, tone_B, tone_Bb, tone_A, tone_G, tone_e, tone_g, tone_a, tone_f, tone_g, tone_e, tone_c, tone_d, tone_B, tone_c};
static int mario_r[] = {
  6, 12, 12, 6, 12, 24, 24, 18, 18, 18, 12, 12, 6, 12, 8, 8, 8, 12, 6, 12, 12, 6, 6, 6, 12};

// Looks like zelda
int peergynt_m[] = {
  tone_G, tone_E, tone_D, tone_C, tone_D, tone_E, tone_G, tone_E, tone_D, tone_C, tone_D, tone_E, tone_D, tone_E,tone_G, tone_E, tone_G, tone_A, tone_E, tone_A, tone_G, tone_E, tone_D, tone_C};
int peergynt_r[] = {
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 4, 4, 4, 4, 8, 8, 8, 8, 8, 8, 8, 8, 8, 16};

// Some electric guitar...
int smoke_m[] = {
  tone_E, tone_G, tone_A, tone_E, tone_G, tone_Bb, tone_A, tone_E, tone_G, tone_A, tone_G, tone_E};
int smoke_r[] = {
  12, 12, 18, 12, 12, 6, 18, 12, 12, 18, 12, 24};

// Start like something similar to le reine au nez rouge...
int natal_m[] = {
  tone_G, tone_A, tone_G, tone_E, tone_G, tone_A, tone_G, tone_E, tone_c, tone_c, tone_A, tone_B, tone_B, tone_G, tone_A, tone_G, tone_A, tone_c, tone_B, tone_A, tone_G, tone_A, tone_G, tone_E};
int natal_r[] = {
  12, 4, 8, 16, 12, 4, 8, 16, 12, 4, 16, 12, 4, 16, 12, 4, 8, 8, 8, 8, 12, 4, 8, 16};

// Seems like some electric guitar solo...
int LTS_m[] = {
  tone_Bb, tone_G, tone_G, tone_Bb, tone_G, tone_G, tone_Bb, tone_G, tone_G, tone_Bb, tone_G, tone_G, tone_Bb, tone_G, tone_C, tone_G, tone_Bb, tone_G, tone_G, tone_Bb, tone_G, tone_G, tone_Bb, tone_G, tone_G, tone_Bb, tone_G, tone_G, tone_Bb, tone_G, tone_F, tone_D, tone_F, tone_D, tone_G, tone_F, tone_D, tone_C, tone_Bb, tone_G, tone_Bb, tone_C, tone_C1, tone_C, tone_Bb, tone_F, tone_D, tone_Bb, tone_G, tone_F, tone_D, tone_C, tone_Bb, tone_D, tone_C, tone_Bb, tone_G} 
;
int LTS_r[] = {
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4};

void setup()
{
  pinMode(PIEZO, OUTPUT);
  delay(2000);

  Serial.begin(115200);
}

void loop()
{
  //  const int LEN = (sizeof(smoke_r) + 1) / 4;
  //  Serial.println(LEN);
  //  for (int i=0; i < LEN; i++)
  //  {
  //    int tom = smoke_m[i];
  //    int tempo = smoke_r[i];
  //
  //    long tvalue = tempo * vel;
  //
  //    tocar(tom, tvalue);
  //
  //    delayMicroseconds(1000); //pause between notes
  //  }

  Serial.println(sizeof(mario_r));
  music(mario_m, mario_r, sizeof(mario_r));
  delay(3000);
  music(peergynt_m, peergynt_r, sizeof(peergynt_r));
  delay(3000);
  music(smoke_m, smoke_r, sizeof(smoke_r));
  delay(3000);
  music(natal_m, natal_r, sizeof(natal_r));
  delay(3000);
  music(LTS_m, LTS_r, sizeof(LTS_r));

  while(1){  
  };
}

void music(int m[], int r[], int size_r) {
  int LEN = (size_r + 1) / 4;
  Serial.println(LEN);
  for (int i=0; i < LEN; i++)
  {
    int tom = m[i];
    int tempo = r[i];

    long tvalue = tempo * vel;

    tocar(tom, tvalue);

    delayMicroseconds(1000); //pause between notes
  }
}

void tocar(int tom, long tempo_value)
{
  long tempo_gasto = 0;
  while (tempo_gasto < tempo_value && tempo_value < 640000) // enters an infinite loop when tempo_value is a big value
  {  
    digitalWrite(PIEZO, HIGH);
    delayMicroseconds(tom / 2);

    digitalWrite(PIEZO, LOW);
    delayMicroseconds(tom/2);

    tempo_gasto += tom;
  }
}








