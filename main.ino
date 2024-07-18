#include "SimpleBMP.h"
#include "ProtogenDriver.h"
#include "FastLED.h"
#include <WiiChuck.h>
#include <Wire.h>
#include "Anim.h"
#include <stdint.h>

#define FPS 120

using namespace std;
uint8_t counter = 0;
uint8_t zero = 0;
uint8_t one = 1;
uint8_t two = 2;

unsigned long prevMillisEyes = 0;
unsigned long prevMillisMouth = 0;
uint8_t frameIndex = 0;
const char* eyeImage = "/eye.bmp";
const char* mouthImage = "/mouth/mouthb.bmp";
uint8_t joystickState = 0; // current state, max 255 states
uint8_t prevState = 0;

CRGB PrimaryColor = CRGB::Purple;

//NunchuckValues
Accessory nunchuck;

//Just define the animation here dumbass.
Anim eyeIdle  = Anim((const char*[]){"/eye/eye1.bmp", "/eye/eye2.bmp", "/eye/eye3.bmp", "/eye/eye2.bmp", "/eye/eye1.bmp", "/eye/eye2.bmp", "/eye/eye3.bmp"}, (int[]){3000, 70, 200, 70, 7000, 70, 200}, 7);
Anim bootAnim = Anim((const char*[]){"/boot/1.bmp"}, (int[]){1000}, 1);
Anim wideEyes = Anim((const char*[]){"/eye/wide.bmp"}, (int[]){1000}, 1);
Anim wideMout = Anim((const char*[]){"/mouth/mouthSmall.bmp"}, (int[]){1000}, 1);
Anim squintEyes = Anim((const char*[]){"/eye/squint.bmp"}, (int[]){1000}, 1);
Anim squintMout = Anim((const char*[]){"/mouth/squint.bmp"}, (int[]){1000}, 1);
Anim Xeyes = Anim((const char*[]){"/eye/X.bmp"}, (int[]){1000}, 1);
Anim cryEyes = Anim((const char*[]){"/cry/eye2.bmp", "/cry/eye3.bmp", "/cry/eye4.bmp", "/cry/eye5.bmp", "/cry/eye6.bmp", "/cry/eye7.bmp", "/cry/eye8.bmp", "/cry/eye9.bmp"}, (int[]){100, 100, 100, 100, 100, 100, 100, 100}, 8);
Anim cryMouth = Anim((const char*[]){"/mouth/cry.bmp"}, (int[]){1000}, 1);
//C button Animations
Anim heartEye = Anim((const char*[]){"/heart/h1.bmp", "/heart/h2.bmp", "/heart/h3.bmp"}, (int[]){500, 200, 200}, 3);
//Z button Animations
Anim boy = Anim((const char*[]){"/eye/boy.bmp", "/mouth/boy.bmp"}, (int[]){1, 1}, 2);
//Hold Both Animations


void setup() {
    Serial.begin(115200);
    setupSDCard();
    setupProtogenDriver();
    //Setup nunchuck.
    Wire.begin(21, 47); //Hardwired I2c Pins.
    setupNunchuck();
}

void loop() {
    unsigned long currentMillisEyes = millis();
    unsigned long currentMillisMout = millis();
    
    //State will be between 1 and 5 with no modifiers.
    //State will be between 6 and 11 with C button modifier.
    //State will be between 12 and 16 with Z button modifier.
    //State will be between 17 and 21 with C and Z button modifiers.

    switch(joystickState) {
      case 1:
        //Looping Idle Animation.
        loopingAnimation(eyeIdle, currentMillisEyes, prevMillisEyes, frameIndex);
        eyeImage = eyeIdle.getFramePath(frameIndex);
        mouthImage = "/mouth/mouthb.bmp";
        //updateEyes(eyeImage);
        //updateMouthWithColor(mouthImage, PrimaryColor);
        updateFaceWithColor(eyeImage, mouthImage, PrimaryColor);
        drawScreen(FPS);
        updateState();
        break;
      case 2:
        //Crying Anim.
        loopingAnimation(cryEyes, currentMillisEyes, prevMillisEyes, frameIndex);
        eyeImage = cryEyes.getFramePath(frameIndex);
        mouthImage = cryMouth.getFramePath(zero);
        updateEyes(eyeImage);
        updateMouthWithColor(mouthImage, PrimaryColor);
        drawScreen(FPS);
        updateState();
        break;
      case 3:
        //X eyes
        //rework to smile
        /**
        eyeImage = Xeyes.getFramePath(zero);
        mouthImage = cryMouth.getFramePath(zero);
        updateFaceWithColor(eyeImage, mouthImage, PrimaryColor);
        drawScreen(FPS);
        updateState();
        */
        loopingAnimation(eyeIdle, currentMillisEyes, prevMillisEyes, frameIndex);
        eyeImage = eyeIdle.getFramePath(frameIndex);
        mouthImage = "/mouth/smile.bmp";
        updateFaceWithColor(eyeImage, mouthImage, PrimaryColor);
        drawScreen(FPS);
        updateState();
        break;
      case 4:
        //Wide Eyes
        eyeImage = wideEyes.getFramePath(zero);
        mouthImage = wideMout.getFramePath(zero);
        updateFaceWithColor(eyeImage, mouthImage, PrimaryColor);
        drawScreen(FPS);
        updateState();
        break;
      case 5:
        //Squint Eyes
        eyeImage = squintEyes.getFramePath(zero);
        mouthImage = squintMout.getFramePath(zero);
        updateFaceWithColor(eyeImage, mouthImage, PrimaryColor);
        drawScreen(FPS);
        updateState();
        break;
      case 6:
        //Heart eyes
        loopingAnimation(heartEye, currentMillisEyes, prevMillisEyes, frameIndex);
        eyeImage = heartEye.getFramePath(frameIndex);
        mouthImage = squintMout.getFramePath(zero);
        updateEyes(eyeImage);
        updateMouthWithColor(mouthImage, PrimaryColor);
        drawScreen(FPS);
        updateState();
        break;
      case 11:
        //Boy Kisser
        eyeImage = boy.getFramePath(zero);
        mouthImage = boy.getFramePath(one);
        updateEyes(eyeImage);
        updateMouth(mouthImage);
        drawScreen(FPS);
        updateState();
        break;
      default:
        //This is the boot animation. it defaults here and when play times is finished it calls updateState()
        if (playTimes(bootAnim, currentMillisEyes, prevMillisEyes, frameIndex, counter, 2)) {updateState();}
        eyeImage = bootAnim.getFramePath(frameIndex);
        updateFace(eyeImage, nullptr);
        drawScreen(FPS);
        break;
    };
}

void updateState() {
  //Nunchuck
  nunchuck.readData();
  uint8_t joyX = nunchuck.getJoyX();
  uint8_t joyY = nunchuck.getJoyY();
  bool buttonC = nunchuck.getButtonC();
  bool buttonZ = nunchuck.getButtonZ();
  //default Idle

  joystickState = 1;
  if (joyX > 178) {
    joystickState = 3;
  } else if (joyX < 76) {
    joystickState = 2;
  }
  if (joyY > 178) {
    joystickState = 4;
  } else if (joyY < 76) {
    joystickState = 5;
  }
  
  if(buttonC){
    joystickState += 5; //increace by 5 for next group of states.
  }

  if(buttonZ){
    joystickState += 10; //increace by 10 for next group of states.
    Serial.println("Button Z pressed");
  }
  
  if(joystickState != prevState) {  
    Serial.print("State: "); //Debug
    Serial.println(joystickState);
    prevState = joystickState;
  }

  //Add button modifiers.
  //Add sensor modifiers.
  //for now just set state to 1
}

void loopingAnimation(Anim animation, unsigned long current, unsigned long &prev, uint8_t &i){
  if (current - prev >= animation.getFrameTime(i)){
    i++;
    if (i >= animation.frameCount){
      i = 0;
    }
    prev = current;
  }
}

bool playTimes(Anim animation, unsigned long current, unsigned long &prev, uint8_t &i, uint8_t &j, uint8_t times){
  if (current - prev >= animation.getFrameTime(i)){
    i++;
    if (i >= animation.frameCount){
      j++;
      i = 0;
    }
    prev = current;
  }
  if (j >= times){
    return true;
  }
  return false;
}

bool playOnce(Anim animation, unsigned long current, unsigned long &prev, uint8_t &i){
  if (current - prev >= animation.getFrameTime(i)){
    i++;
    if (i > animation.frameCount){
      i = animation.frameCount - 1;
      return true;
    }
    prev = current;
  }
  return false; 
}

void setupNunchuck() {
  nunchuck.begin();
  if (nunchuck.type == Unknown) {
    nunchuck.type = NUNCHUCK;
  }
}