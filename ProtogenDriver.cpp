/**********************************************************************
  Filename    : ProtogenDriver.cpp
  Description : This allows driving the Protogen screen by loading in images from SD card. 
  Additional functions to be added for directly drawing to the screen. This library needs 
  a lot of work to be truely reusable.
**********************************************************************/

#include <stdint.h>
#include "ProtogenDriver.h"
#include "SimpleBMP.h"
#include "FastLED.h"

#define DATA_PIN 12
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS 384

CRGB leds[NUM_LEDS];    // Define the array of leds

/**
const char* previousMouthImagePath;
const char* previousEyeImagePath;*/

void setupProtogenDriver() {
    FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(10);
}

void drawScreen(int FPS){
    FastLED.show();
    FastLED.delay(1000/FPS);
}

void updateMouth(const char* mouthImagePath){
    /**if(strcmp(mouthImagePath, previousMouthImagePath) == 0){ // DOES NOT COMPILE
      Serial.println("prev image used for mouth");
      return;
    }*/
    if(mouthImagePath == NULL){
        return;
    }

    int width, height;
    RGBPixel* mouthArray = readBMP(mouthImagePath, width, height);
    for (int i = 0; i < width * height; i++){
        int X = i % width;
        int Y = i / width;
        CRGB colorMouth = CRGB(mouthArray[i].r, mouthArray[i].g, mouthArray[i].b);
        leds[graph(X, Y)] = colorMouth;
    }
    //previousEyeImagePath = mouthImagePath;
    delete[] mouthArray;
}

void updateEyes(const char* eyeImagePath){
    /**if(strcmp(eyeImagePath, previousEyeImagePath) == 0){ // no need to read the image again if it hasn't changed.
      Serial.println("prev image used for eyes");
      return;
    }*/
    if(eyeImagePath == NULL){
        return;
    }
    int width, height;
    RGBPixel* eyeArray = readBMP(eyeImagePath, width, height);
    for (int i = 0; i < width * height; i++){
        int X = i % width;
        int Y = i / width;
        CRGB color = CRGB(eyeArray[i].r, eyeArray[i].g, eyeArray[i].b);
        leds[graph(40 + (7-X), Y)] = color;
        leds[graph(32 + X, Y)]     = color;
    }
    //previousEyeImagePath = eyeImagePath;
    delete[] eyeArray;
}

void updateEyeR(const char* imagePath){
  if(imagePath == NULL) {return;}
  int width, height;
  RGBPixel* eyeArray = readBMP(imagePath, width, height);
  for (int i = 0; i < width * height; i++){
    int X = i % width;
    int Y = i / width;
    CRGB color = CRGB(eyeArray[i].r, eyeArray[i].g, eyeArray[i].b);
    leds[graph(32 + X, Y)] = color;
  }
  delete[] eyeArray;
}

void updateEyeL(const char* imagePath){
  if(imagePath == NULL) {return;}
  int width, height;
  RGBPixel* eyeArray = readBMP(imagePath, width, height);
  for (int i = 0; i < width * height; i++){
    int X = i % width;
    int Y = i / width;
    CRGB color = CRGB(eyeArray[i].r, eyeArray[i].g, eyeArray[i].b);
    leds[graph(40 + X, Y)] = color;
  }
  delete[] eyeArray;
}

void updateFace(const char* eyeImagePath, const char* mouthImagePath){
    updateMouth(mouthImagePath);
    updateEyes(eyeImagePath);
}

void updateMouthWithColor(const char* mouthImagePath, CRGB color){
  updateMouth(mouthImagePath);
  multiplyPixels(color, 0, 256);
}

void updateFaceWithColor(const char* eyeImagePath, const char* mouthImagePath, CRGB color){
    updateMouth(mouthImagePath);
    updateEyes(eyeImagePath);
    multiplyPixels(color, 0, NUM_LEDS);
}

void multiplyPixels(CRGB color, int start, int end){
    for (int i = start; i < end; i++){
        leds[i] = multiplyColor(leds[i], color);
    }
}

CRGB multiplyColor(CRGB color1, CRGB color2){  //Use when multiplying to a BW mask or Color image we want to color fill.
  //COLOR 1 SHOULD ALWAYS BE THE MASK
  int avg = (color1.red + color1.green + color1.blue) / 3;
  color1 = CRGB(avg, avg, avg);
  
  return CRGB(
    (color1.red * color2.red)     / 255,
    (color1.green * color2.green) / 255,
    (color1.blue * color2.blue)   / 255
  );
}

int graph(int X, int Y){
  int pos = 0;
  X = X+1;
  Y = Y+1;
  pos = pos + ((X-1)*8);
  if (X % 2 == 0) {
    pos = pos + (9-Y);
  }
  else {
    pos = pos + Y;
  }
  pos = pos-1;
  return pos;
}