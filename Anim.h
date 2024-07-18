#ifndef ANIM_H
#define ANIM_H

#include <stdint.h>

using namespace std;

class Anim {
public:
    char* imagePath;
    int frameTime;
    static const uint8_t maxFrames = 20;
    uint8_t frameCount;
    int keyFrameTime[maxFrames];
    const char* keyframes[maxFrames];
    Anim(const char* keys[], int frameTimes[], uint8_t numFrames);
    const char* getFramePath(uint8_t &frameIndex);
    int getFrameTime(uint8_t &frameIndex);
};

#endif