#include <Arduino.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <WT901.h>
#include <Wire.h>
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include <math.h>

#define TEMP2 26 //R1 = 9.75  R_TEMP1 = 10k  - 3950
#define TEMP3 27 //R1 = 9.75  R_TEMP1 = 10k  - 3950

#define SCK  18
#define MISO  19
#define MOSI  23
#define CS  5

#define AMP 10000000

SPIClass spi = SPIClass(VSPI);
int info = 100;
unsigned long dtime = 0, gtime = 0;
String day = "";
int read_number=0;


float aX,aY,aZ,mX,mY,mZ,t1,t2;



void initSDCard(){
  if (!SD.begin()) {
    Serial.println("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD.cardType();
  if(cardType == CARD_NONE){
    Serial.println("No SD card attached");
    return;
  }
}

void setup(){
  Serial.begin(9600);
  Serial.println("Start");
  initSDCard();
  dtime=millis();
  JY901.StartIIC();
  info=1;
}

float readTemp(int pin){
  float v1 = analogRead(pin);
  float r1 = (4095 - v1) / v1 * 10;
  float t1 = log((r1-0.61248363656951)/30.0619993355909)/(-0.0463466112115211);
  return t1;
}

void loop(){
  if(millis()-dtime > 1000){
    info++;
    String fileDir1 = ("/Acce/" + String(info) + ".txt");
    String fileDir2 = ("/Mag/" + String(info) + ".txt");
    String fileDir3 = ("/Temp/" + String(info) + ".txt");
    String fileDir4 = ("/Total/" + String(info) + ".txt");
    File file1 = SD.open(fileDir1, "w");
    File file2 = SD.open(fileDir2, "w");
    File file3 = SD.open(fileDir3, "w");
    File file4 = SD.open(fileDir4, "w");
    if(!file1) {
      Serial.println("Failed to open file for appending");
      return;
    }
    if(!file2) {
      Serial.println("Failed to open file for appending");
      return;
    }
    if(!file3) {
      Serial.println("Failed to open file for appending");
      return;
    }
    if(!file4) {
      Serial.println("Failed to open file for appending");
      return;
    }

    dtime=micros();
    gtime = micros()-dtime;
    while(gtime < 5e6){
      JY901.GetAcc();
      aX = (float)JY901.stcAcc.a[0]/32768*16*AMP;
      aY = (float)JY901.stcAcc.a[1]/32768*16*AMP;
      aZ = (float)JY901.stcAcc.a[2]/32768*16*AMP;
      
      JY901.GetMag();
      mX = JY901.stcMag.h[0];
      mY = JY901.stcMag.h[1];
      mZ = JY901.stcMag.h[2];
      
      t1=readTemp(25);
      t2=readTemp(26);

      file1.println(String(gtime)+","+String(aX)+","+String(aY)+","+String(aZ));
      file2.println(String(gtime)+","+String(mX)+","+String(mY)+","+String(mZ));
      file3.println(String(gtime)+","+String(t1)+","+String(t2));
      file4.println(String(gtime)+","+String(aX)+","+String(aY)+","+String(aZ)+","+String(mX)+","+String(mY)+","+String(mZ)+","+String(t1)+","+String(t2));
      gtime=micros()-dtime;

    }
    file1.close();
    file2.close();
    file3.close();
    dtime=micros();
    Serial.println("Done");
  }
}


