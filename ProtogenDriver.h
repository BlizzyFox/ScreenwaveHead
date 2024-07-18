#ifndef ProtogenDriver_h
#define ProtogenDriver_h

#include "FastLED.h"

void setupProtogenDriver();
void updateFace(const char* eyeImagePath, const char* mouthImagePath);
void updateMouth(const char* mouthImagePath);
void updateEyes(const char* eyeImagePath);
void drawScreen(int FPS);
int graph(int X, int Y);
void updateFaceWithColor(const char* eyeImagePath, const char* mouthImagePath, CRGB color);
void updateMouthWithColor(const char* mouthImagePath, CRGB color);
void multiplyPixels(CRGB color, int start, int end);
CRGB multiplyColor(CRGB color1, CRGB color2);
void updateEyeL(const char* imagePath);
void updateEyeR(const char* imagePath);

#endif