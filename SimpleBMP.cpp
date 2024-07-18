/**********************************************************************
  Filename    : SimpleBMP.cpp
  Description : This is a library for reading BMP files and getting the RGB data
  as an RGBPixel struct. 
**********************************************************************/
#include "SimpleBMP.h"
#include "SD_MMC.h"

//SD PINS
#define SD_MMC_CMD 38 // THIS CANNOT BE MODIFIED
#define SD_MMC_CLK 39
#define SD_MMC_D0  40

// BMP file header
#pragma pack(push, 1)
struct BMPFileHeader {
    uint16_t bfType;          // BMP file type ('BM' in little-endian)
    uint32_t bfSize;          // File size in bytes
    uint16_t bfReserved1;     // Reserved (should be 0)
    uint16_t bfReserved2;     // Reserved (should be 0)
    uint32_t bfOffBits;       // Offset to image data in bytes
    uint32_t biSize;          // Size of the BITMAPINFOHEADER structure
    int32_t biWidth;          // Image width in pixels
    int32_t biHeight;         // Image height in pixels (positive for bottom-up)
    uint16_t biPlanes;        // Number of color planes (must be 1)
    uint16_t biBitCount;      // Number of bits per pixel (24 for RGB)
    uint32_t biCompression;   // Compression type (0 for uncompressed)
    uint32_t biSizeImage;     // Size of the image data in bytes (can be 0 if uncompressed)
    int32_t biXPelsPerMeter;  // Horizontal resolution in pixels per meter (can be 0)
    int32_t biYPelsPerMeter;  // Vertical resolution in pixels per meter (can be 0)
    uint32_t biClrUsed;       // Number of colors used in the bitmap (0 for full color)
    uint32_t biClrImportant;  // Number of important colors (0 means all colors are important)
};
#pragma pack(pop)

void setupSDCard() {
    SD_MMC.setPins(SD_MMC_CLK, SD_MMC_CMD, SD_MMC_D0);
    // Mount the SD card
    if (!SD_MMC.begin("/sdcard", true, true, SDMMC_FREQ_DEFAULT, 5)) {
        Serial.println("Card Mount Failed");
        return;
    }
}

RGBPixel* readBMP(const char *path, int &width, int &height) {
    // IS THE BMP FILE VALID?
    File bmpFile = SD_MMC.open(path);
    if(!bmpFile){
        Serial.print("Couldn't open file: ");
        Serial.println(path);
        return NULL;
    }
    // IS IT A VALID BMP FILE
    BMPFileHeader header;
    bmpFile.read((uint8_t*)&header, sizeof(BMPFileHeader));
    if(header.bfType != 0x4D42){
        Serial.println("Not a BMP file");
        return NULL;
    }
    //Read File information
    width = header.biWidth;
    height = abs(header.biHeight);
    int bitDepth = header.biBitCount;
    int rowSize = (bitDepth * width + 31) / 32 * 4;
    bool isTopDown = header.biHeight < 0;

    RGBPixel* pixels = new RGBPixel[width * height];
    for(int y = 0; y < height; y++){
        int readY = isTopDown ? y : (height - 1 - y);
        uint8_t row[rowSize];
        bmpFile.seek(header.bfOffBits + readY * rowSize);
        bmpFile.read(row, rowSize);
        for(int x = 0; x < width; x++){
            int pixelIndex = x * (bitDepth / 8);
            int arrayIndex = y * width + x;
            pixels[arrayIndex].b = row[pixelIndex];
            pixels[arrayIndex].g = row[pixelIndex + 1];
            pixels[arrayIndex].r = row[pixelIndex + 2];
        }
    }
    bmpFile.close();
    return pixels;
}