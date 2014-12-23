#include <sdcard.h>
#include <sdraw.h>
#include <SPI.h>

SdRaw sdRaw;  // SD RAW class
SdSpi sdspi;  // SD SPI class

void setup()
{
  Serial.begin(9600);
  sdspi.pinSS((uChar)10);  // SD Select pin
  //delay(2000);  // wait for Serial Monitor 8n1

  DRESULT res;  // FatFs return code
  uLong sector = firstSector;  // sector to read/write

  DSTATUS ds = sdRaw.initSd();  // init SD or test if memory card is inserted
//  if (ds) showError("init", RES_NOTRDY);
//  else {
    Serial.println(F("A card is inserted."));
    uChar buffRead[50];
    res = sdRaw.readRaw(buffRead, sector, 0, 500);
    Serial.println(F("Show the first fifty bytes :"));
    if (res) showError("read buff", res);
    else showBlock(buffRead, 500);  // show the first 50 bytes
//  }
}

void loop()
{
  //  delay(50);
}

void showBlock(uChar* buff, int count)
{
  for (int i = 0; i < count; i++) Serial.print((char)buff[i]);
  Serial.println();
}

void showError(String msg, DRESULT res)
{
  String result[] = {
    "Successful", "R/W Error", "Write Protected", "Not Ready", "Invalid Parameter"      };
  Serial.print(msg); 
  Serial.print(", ERROR: "); 
  Serial.println(result[res]); 
}



