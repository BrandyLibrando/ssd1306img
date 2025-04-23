#ifndef SSD1306FUNC_H_
#define SSD1306FUNC_H_


#include <Arduino.h>
#include <string.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


typedef struct ImageByteArray {
  const uint8_t* PROGMEM bitmap;    // The byte array of the 1-bit/binary image.
  const int width;                  // Width of the image, in pixels.
  const int height;                 // Height of the image, in pixels.
} Image;


void fillScreenSlow(Adafruit_SSD1306*);
void fillScreenFast(Adafruit_SSD1306*);
void fadeGrid(Adafruit_SSD1306*, long, uint16_t);
void fadeCross(Adafruit_SSD1306*, long, uint16_t);
void fadeVertical(Adafruit_SSD1306*, int, long, uint16_t);
void fadeHorizontal(Adafruit_SSD1306*, int, long, uint16_t);
void fadeDiagonal(Adafruit_SSD1306*, int, long, uint16_t);

void fadeInGridBitmap(Adafruit_SSD1306*, long, long, int16_t, int16_t, Image);
void drawVerticalScrollingBitmap(Adafruit_SSD1306*, long, long, long, int, bool, bool, int16_t, int16_t, int16_t, Image);

void drawDialogText(Adafruit_SSD1306*, uint8_t, long, long, uint8_t, const char*, const char*);
void drawTimedDialogText(Adafruit_SSD1306*, uint8_t, long, long, long, uint8_t, uint8_t, uint8_t, const char*, const char*);
void clearHeaderText(Adafruit_SSD1306*);
void clearDialogText(Adafruit_SSD1306*);

void ctcSerial(Adafruit_SSD1306*);
void ctcTimedSerial(Adafruit_SSD1306*, long);
void ctc(Adafruit_SSD1306*, uint8_t);
void ctcTimed(Adafruit_SSD1306*, uint8_t, long);


#endif