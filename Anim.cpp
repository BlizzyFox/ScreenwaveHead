#include "Anim.h"
#include <stdint.h>

Anim::Anim(const char* keys[], int frameTimes[], uint8_t numFrames) {
    frameCount = numFrames;
    for (uint8_t i = 0; i < numFrames; i++) {
        keyframes[i] = keys[i];
        keyFrameTime[i] = frameTimes[i];
    }
}

const char* Anim::getFramePath(uint8_t &index) {
    const char* path = keyframes[index];
    return path;
}

int Anim::getFrameTime(uint8_t &index) {
    int time = keyFrameTime[index];
    return time;
}