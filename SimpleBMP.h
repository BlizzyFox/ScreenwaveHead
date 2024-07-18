#ifndef SimpleBMP_h
#define SimpleBMP_h

#include <stdint.h>

struct RGBPixel {
    uint8_t r;
    uint8_t g;
    uint8_t b;
};
void setupSDCard();
RGBPixel* readBMP(const char *path, int &width, int &height);
#endif