/*********************************************************
*                                                        *
*   SSD1306Func                                          *
*                                                        *
*   Author: Julius Alvin (Andy) Librando                 *
*   Date created: April 29, 2024                         *
*   Date last modified: May 18, 2024                     *
*                                                        *
*   Short description:                                   *
*   A module containing functions                        *
*     to work with displaying images                     *
*     on OLED screens (SSD1306 128x64).                  *
*                                                        *
*********************************************************/

#include "SSD1306Func.h"

#include <Arduino.h>
#include <string.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define ON 1
#define OFF 0
#define RECOMM -1


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// FILL SCREEN FUNCTIONS - FOR DEBUG/SCREEN TEST

/// @brief Fills the screen with three passes of interlacing lines. For checking OLED screen dead pixels.
/// @param display A pointer pointing to the `Adafruit_SSD1306` display object.
void fillScreenSlow(Adafruit_SSD1306* display) {
  int16_t i;

  display->clearDisplay();

  for(i=0; i<display->height(); i+=2) {
    display->drawFastHLine(0, i, display->width(), ON);
    display->display(); 
    delay(5);
  }
  for(i=0; i<display->width(); i+=2) {
    display->drawFastVLine(i, 0, display->height(), ON);
    display->display(); 
    delay(5);
  }
  for(i=1; i<display->height(); i+=2) {
    display->drawFastHLine(0, i, display->width(), ON);
    display->display(); 
    delay(5);
  }
}

/// @brief Fills the screen instantly. Used for checking OLED dead pixels.
/// @param display A pointer pointing to the `Adafruit_SSD1306` display object.
void fillScreenFast(Adafruit_SSD1306* display) {
  display->fillRect(0, 0, display->width(), display->height(), ON);
  display->display();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// FADE WHITEOUT/BLACKOUT FUNCTIONS

/// @brief A checkerboard-style fade transition. Fades in three steps, to either full black or full white/on.
/// @param display A pointer pointing to the `Adafruit_SSD1306` display object.
/// @param delaytime Number of milliseconds taken per step. Using negative values will use the recommended value (`50`).
/// @param state Either `0` or `1`. Use `0` for a fade-out, while `1` for fade-in.
void fadeGrid(Adafruit_SSD1306* display, long delaytime, uint16_t state) {
  if (delaytime < 0) delaytime = 50;   // Recommended delay time

  for(int i=0; i<display->width(); i+=2) {
    display->drawFastVLine(i, 0, display->height(), state);
  }
  display->display(); 
  delay(delaytime);

  for(int i=0; i<display->height(); i+=2) {
    display->drawFastHLine(0, i, display->width(), state);
  }
  display->display(); 
  delay(delaytime);

  for(int i=1; i<display->width(); i+=2) {
    display->drawFastVLine(i, 0, display->height(), state);
  }
  display->display(); 
  delay(delaytime);

  if (!state) {
    display->clearDisplay();
    display->display();
    delay(delaytime);
  }
}

/// @brief A diagonal version of the grid fade transition. Fades in two steps, to either full black or full white/on.
/// @param display A pointer pointing to the `Adafruit_SSD1306` display object.
/// @param delaytime Number of milliseconds taken per step. Using negative values will use the recommended value (`50`).
/// @param state Either `0` or `1`. Use `0` for a fade-out, while `1` for fade-in.
void fadeCross(Adafruit_SSD1306* display, long delaytime, uint16_t state) {
  if (delaytime < 0) delaytime = 50;
  
  for(int i=0; i<display->width()+display->height(); i+=2) {
    display->drawLine(0, i, i, 0, state);
  }
  display->display(); 
  delay(delaytime);

  for(int i=1; i<display->width()+display->height(); i+=2) {
    display->drawLine(0, i, i, 0, state);
  }
  display->display(); 
  delay(delaytime);

  if (!state) {
    display->clearDisplay();
    display->display();
    delay(delaytime);
  }
}

/// @brief A fade transition resembling vertical blinds. Fades to either full black or full white/on.
/// @param display A pointer pointing to the `Adafruit_SSD1306` display object.
/// @param cycles Number of steps for the fade. Using negative values will use the recommended value (`4`).
/// @param wholedelaytime The duration of the entire fade transition. Using negative values will use the recommended value.
/// @param state Either `0` or `1`. Use `0` for a fade-out, while `1` for fade-in.
void fadeVertical(Adafruit_SSD1306* display, int cycles, long wholedelaytime, uint16_t state) {
  // Recommended values
  if (cycles < 0) cycles = 4;
  if (wholedelaytime < 0) wholedelaytime = 10 * cycles;
  
  for(int i=0; i<cycles; i++) {
    for(int j=i; j<display->width(); j+=cycles) {
      display->drawFastVLine(j, 0, display->height(), state);
    }
    display->display(); 
    delay((long) wholedelaytime / cycles);
  }

  if (!state) {
    display->clearDisplay();
    display->display();
    delay(wholedelaytime / cycles);
  }
}

/// @brief A fade transition resembling horizontal blinds. Fades to either full black or full white/on.
/// @param display A pointer pointing to the `Adafruit_SSD1306` display object.
/// @param cycles Number of steps for the fade. Using negative values will use the recommended value (`3`).
/// @param wholedelaytime The duration of the entire fade transition. Using negative values will use the recommended value.
/// @param state Either `0` or `1`. Use `0` for a fade-out, while `1` for fade-in.
void fadeHorizontal(Adafruit_SSD1306* display, int cycles, long wholedelaytime, uint16_t state) {
  // Recommended values
  if (cycles < 0) cycles = 3;
  if (wholedelaytime < 0) wholedelaytime = 10 * cycles;
  
  for(int i=0; i<cycles; i++) {
    for(int j=i; j<display->height(); j+=cycles) {
      display->drawFastHLine(0, j, display->width(), state);
    }
    display->display(); 
    delay((long) wholedelaytime / cycles);
  }
  
  if (!state) {
    display->clearDisplay();
    display->display();
    delay(wholedelaytime / cycles);
  }
}

/// @brief A fade transition using diagonal lines. Fades to either full black or full white/on.
/// @param display A pointer pointing to the `Adafruit_SSD1306` display object.
/// @param cycles Number of steps for the fade. Using negative values will use the recommended value (`4`).
/// @param wholedelaytime The duration of the entire fade transition. Using negative values will use the recommended value.
/// @param state Either `0` or `1`. Use `0` for a fade-out, while `1` for fade-in.
void fadeDiagonal(Adafruit_SSD1306* display, int cycles, long wholedelaytime, uint16_t state) {
  // Recommended values
  if (cycles < 0) cycles = 4;
  if (wholedelaytime < 0) wholedelaytime = 25 * cycles;
  
  for(int i=0; i<cycles; i++) {
    for(int j=i; j<display->width()+display->height(); j+=cycles) {
      display->drawLine(0, j, j, 0, state);
    }
    display->display(); 
    delay((long) wholedelaytime / cycles);
  }
  
  if (!state) {
    display->clearDisplay();
    display->display();
    delay(wholedelaytime / cycles);
  }
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// IMAGE PROCESSING FUNCTIONS

/// @brief Fades the screen to full white/on with the `fadeGrid` effect, then draws the target bitmap/image with the `fadeCross` effect at specified (x,y) location.
/// @param display A pointer pointing to the `Adafruit_SSD1306` display object.
/// @param delaytime Number of milliseconds taken per step in the 3-step initial fade. Using negative values will use the recommended value (`50`).
/// @param initdelaytime Number of milliseconds to wait after the initial fade before drawing the bitmap. Using negative values will use the recommended value (`500`).
/// @param offset_x The x-coordinate of the image, starting at top-left.
/// @param offset_y The y-coordinate of the image, starting at top-left.
/// @param bmp The bitmap to be drawn.
void fadeInGridBitmap(Adafruit_SSD1306* display, long delaytime, long initdelaytime, int16_t offset_x, int16_t offset_y, Image bmp) {
  // Recommended values
  if (delaytime < 0) delaytime = 50;
  if (initdelaytime < 0) initdelaytime = 500;
  
  fadeGrid(display, delaytime, ON);
  delay(initdelaytime);

  for(int i=0; i<display->width(); i+=2) {
    display->drawFastVLine(i, 0, display->height(), OFF);
  }
  display->drawBitmap(offset_x, offset_y, bmp.bitmap, bmp.width, bmp.height, ON);
  display->display(); 
  delay(delaytime);
  
  for(int i=0; i<display->height(); i+=2) {
    display->drawFastHLine(0, i, display->width(), OFF);
  }
  display->drawBitmap(offset_x, offset_y, bmp.bitmap, bmp.width, bmp.height, ON);
  display->display(); 
  delay(delaytime);

  for(int i=1; i<display->height(); i+=2) {
    display->drawFastHLine(0, i, display->width(), OFF);
  }
  display->drawBitmap(offset_x, offset_y, bmp.bitmap, bmp.width, bmp.height, ON);
  display->display(); 
  delay(delaytime);
}

/// @brief Draws a bitmap, then scrolls it vertically to the specified y-coordinate.
/// @param display A pointer pointing to the `Adafruit_SSD1306` display object.
/// @param initialdelay Number of milliseconds to wait between drawing and scrolling the bitmap. Using negative values will use the recommended value (`500`).
/// @param enddelay Number of milliseconds to delay after reaching the end of scrolling. Using negative values will use the recommended value (`500`).
/// @param scrolldelay Number of milliseconds taken to scroll by one step. Using negative values will use the recommended value (`5`).
/// @param scrollstep Number of pixels to skip per scrolling step. If `1`, scroll row by row; if `2`, skip every other row; etc. Use positive values for scrolling down, or negative values for scrolling up.
/// @param snaptoend If `false`, and `scrollstep > 1`, `scrollstep` will revert to `1` once the remaining rows to scroll is less than `scrollstep`. 
/// @param allowoverflow  If `false`, scrolling will not exceed the bottom of the bitmap. Useful when bitmap height is not divisible by `scrollstep`. 
/// @param offset_x The x-coordinate of the image, starting at top-left. If this function is used with `fadeInGridBitmap`, use the `offset_x` value passed there.
/// @param offset_y The x-coordinate of the image, starting at top-left. If this function is used with `fadeInGridBitmap`, use the `offset_y` value passed there.
/// @param end_y A value "n" where scrolling will stop. For scroll-down, nth row from the bottom, and for scroll-up, nth row from the top.
/// @param bmp The bitmap to be drawn.
void drawVerticalScrollingBitmap(Adafruit_SSD1306* display, long initialdelay, long enddelay, long scrolldelay, int scrollstep, bool snaptoend, bool allowoverflow, int16_t offset_x, int16_t offset_y, int16_t end_y, Image bmp) {
  // Recommended/default values
  if (initialdelay < 0) initialdelay = 500;
  if (enddelay < 0) enddelay = 500;
  if (scrolldelay < 0) scrolldelay = 5;

  // Modify end_y values internally to be the absolute y-coordinate where scrolling will stop
  // TODO: Review this 
  if (scrollstep > 0 && end_y <= 0) end_y = bmp.height;
  else if (scrollstep > 0 && end_y + 64 > bmp.height && !allowoverflow) end_y = bmp.height;
  else if (scrollstep > 0) end_y += 64;
  else if (scrollstep < 0 && end_y + 64 > bmp.height && !allowoverflow) end_y = bmp.height - 64;
  else if (scrollstep < 0 && end_y < 0 && !allowoverflow) end_y = 0;
  
  display->drawBitmap(offset_x, offset_y, bmp.bitmap, bmp.width, bmp.height, ON);
  display->display();
  delay(initialdelay);

  if (scrollstep > 0) {
    int height = 0;
    while (height + 64 - offset_y < end_y) {
      display->drawBitmap(offset_x, -height + offset_y, bmp.bitmap, bmp.width, bmp.height, OFF);

      if (height + 64 - offset_y + scrollstep < end_y) height+=scrollstep;
      else if (height + 64 - offset_y < end_y && !snaptoend) height++;
      else break;

      display->drawBitmap(offset_x, -height + offset_y, bmp.bitmap, bmp.width, bmp.height, ON);
      display->display();
      delay(scrolldelay);
    }

    display->drawBitmap(offset_x, -(end_y - 64), bmp.bitmap, bmp.width, bmp.height, ON);
    display->display();
    delay(enddelay);
  } else if (scrollstep < 0) {
    int height = offset_y;
    while (height < -end_y) {
      display->drawBitmap(offset_x, height, bmp.bitmap, bmp.width, bmp.height, OFF);

      if (height - scrollstep <= -end_y) height-=scrollstep;
      else if (height <= -end_y && !snaptoend) height++;
      else break;

      display->drawBitmap(offset_x, height, bmp.bitmap, bmp.width, bmp.height, ON);
      display->display();
      delay(scrolldelay);
    }

    display->drawBitmap(offset_x, -end_y, bmp.bitmap, bmp.width, bmp.height, ON);
    display->display();
    delay(enddelay);
  }
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// VISUAL NOVEL-ESQUE FUNCTIONS

// TEXT PROCESSING FUNCTIONS

/// @brief Draws the specified dialog text with a header label. Allows both instant and animated text display. Uses serial monitor to invoke click-to-confirm (CTC).
/// @param display A pointer pointing to the Adafruit_SSD1306 display object.
/// @param textspeed A positive integer, at least `1`. Number of characters revealed in one step. Only useful when `onebyone` is `true`. Using negative values will use the recommended value (`1`).
/// @param chardelay Number of milliseconds taken to reveal a step. Using negative values will use the recommended value. Only useful when `onebyone` is `true`. Using negative values will use the recommended value (`10`).
/// @param headerdelay Number of milliseconds to wait after showing header label before displaying the dialog text. Using negative values will use the recommended value (`200`).
/// @param onebyone If `true`, animate the text display by revealing n characters at a time, with n = `textspeed`.  
/// @param headertext A string literal, the header label.
/// @param dialog A string literal, the dialog to be displayed. Use the " ` " character to invoke CTC midway through a dialog.
void drawDialogText(Adafruit_SSD1306* display, uint8_t textspeed, long chardelay, long headerdelay, uint8_t onebyone, const char* headertext, const char* dialog) {
  // Recommended values
  if (textspeed <= 0) textspeed = 1;
  if (chardelay < 0) chardelay = 10;
  if (headerdelay < 0) headerdelay = 200;

  int size = strlen(dialog);

  display->setCursor(0, 0);
  display->write(headertext);
  display->display();
  delay(headerdelay);

  display->setCursor(0, 16);

  if (onebyone) {
    int i = 0;
    while (i + textspeed < size) {
      for (int x = 0; x < textspeed; x++) {
        if (dialog[i+x] != '`') display->write(dialog[i+x]);
        else {
          display->display();
          delay(chardelay);
          ctcSerial(display);
        }
      }

      display->display();
      delay(chardelay);
      i += textspeed;
    }

    while (i < size) {
      if (dialog[i] != '`') {
        display->write(dialog[i]);
        display->display();
        delay(chardelay); 
      }
      else ctcSerial(display);
      i++;
    }
  } 
  
  else {
    display->write(dialog);
    display->display();
    delay(1);
  }

  ctcSerial(display);
}

/// @brief Draws the specified dialog text with a header label. Allows both instant and animated text display. Uses serial monitor to invoke click-to-confirm (CTC), or a timer for auto-progression.
/// @param display A pointer pointing to the Adafruit_SSD1306 display object.
/// @param textspeed A positive integer, at least `1`. Number of characters revealed in one step. Only useful when `onebyone` is `true`. Using negative values will use the recommended value (`1`).
/// @param chardelay Number of milliseconds taken to reveal a step. Using negative values will use the recommended value. Only useful when `onebyone` is `true`. Using negative values will use the recommended value (`10`).
/// @param headerdelay Number of milliseconds to wait after showing header label before displaying the dialog text. Using negative values will use the recommended value (`200`).
/// @param timer Number of milliseconds for the timed CTC. Using negative values will use the recommended value (`10000`).
/// @param timedinput If `true`, enable timer aside from serial monitor for midway CTCs. Otherwise, only use serial monitor. 
/// @param timedend If `true`, enable timer aside from serial monitor for CTC at the end of the dialog. Otherwise, only use serial monitor.
/// @param onebyone If `true`, animate the text display by revealing n characters at a time, with n = `textspeed`.  
/// @param headertext A string literal, the header label.
/// @param dialog A string literal, the dialog to be displayed. Use the " ` " character to invoke CTC midway through the dialog.
void drawTimedDialogText(Adafruit_SSD1306* display, uint8_t textspeed, long chardelay, long headerdelay, long timer, uint8_t timedinput, uint8_t timedend, uint8_t onebyone, const char* headertext, const char* dialog) {
  // Recommended values
  if (textspeed <= 0) textspeed = 1;
  if (chardelay < 0) chardelay = 10;
  if (headerdelay < 0) headerdelay = 200;
  if (timer < 0) timer = 10000;

  int size = strlen(dialog);

  display->setCursor(0, 0);
  display->write(headertext);
  display->display();
  delay(headerdelay);

  display->setCursor(0, 16);

  if (onebyone) {
    int i = 0;
    while (i + textspeed < size) {
      for (int x = 0; x < textspeed; x++) {
        if (dialog[i+x] != '`') display->write(dialog[i+x]);
        else {
          display->display();
          delay(chardelay);
          if (timedinput) ctcTimedSerial(display, timer);
          else ctcSerial(display);
        }
      }

      display->display();
      delay(chardelay);
      i += textspeed;
    }

    while (i < size) {
      if (dialog[i] != '`') {
        display->write(dialog[i]);
        display->display();
        delay(chardelay); 
      }
      else {
        if (timedinput) ctcTimedSerial(display, timer);
        else ctcSerial(display);
      }
      i++;
    }
  } 
  
  else {
    display->write(dialog);
    display->display();
    delay(1);
  }

  if (timedend) ctcTimedSerial(display, timer);
  else ctcSerial(display);
}


/// @brief Clears the header label on a displayed dialog screen. 
/// @param display A pointer pointing to the Adafruit_SSD1306 display object.
void clearHeaderText(Adafruit_SSD1306* display) {
  display->fillRect(0, 0, 128, 16, OFF);
  display->display();
  delay(1);
}

/// @brief Clears the dialog section on a displayed dialog screen.
/// @param display A pointer pointing to the Adafruit_SSD1306 display object.
void clearDialogText(Adafruit_SSD1306* display) {
  display->fillRect(0, 16, 128, 48, OFF);
  display->display();
  delay(1);
}



// DELAY/INPUT FUNCTIONS

/// @brief Use serial monitor to wait for a user input. Displays a CTC indicator at the bottom-right corner.
/// @param display A pointer pointing to the Adafruit_SSD1306 display object.
void ctcSerial(Adafruit_SSD1306* display) {
  int baseX = display->getCursorX();
  int baseY = display->getCursorY();
  uint8_t color = 0;

  while (Serial.available()) Serial.read();   // clear any serial artifacts

  long prev = millis();
  long now;

  while(!Serial.available()) {
    now = millis();
    if (now - prev > 500) {
      prev = now;
      color = !color;
      display->setCursor(116, 0);
      display->setTextColor(color);
      display->write(">>");
      display->display();
    }
  }

  display->setCursor(116, 0);
  display->setTextColor(0);
  display->write(">>");
  display->display();

  while (Serial.available()) Serial.read();   // clear serial buffer for next CTC call
  display->setCursor(baseX, baseY);
  display->setTextColor(1);
}

/// @brief Use serial monitor to wait for a user input, or use a timer. Displays a CTC indicator at the bottom-right corner.
/// @param display A pointer pointing to the Adafruit_SSD1306 display object.
/// @param timerMS CTC timer, in milliseconds.
void ctcTimedSerial(Adafruit_SSD1306* display, long timerMS) {
  int baseX = display->getCursorX();
  int baseY = display->getCursorY();
  uint8_t color = 0;

  while (Serial.available()) Serial.read();   // clear any serial artifacts
  
  long basetime = millis();
  long prev = millis();
  long now = millis();

  while(!Serial.available() && now - basetime < timerMS) {
    now = millis();
    if (now - prev > 500) {
      prev = now;
      color = !color;
      display->setCursor(116, 0);
      display->setTextColor(color);
      display->write(">>");
      display->display();
    }
  }

  display->setCursor(116, 0);
  display->setTextColor(0);
  display->write(">>");
  display->display();

  while (Serial.available()) Serial.read();   // clear serial buffer for next CTC call
  display->setCursor(baseX, baseY);
  display->setTextColor(1);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// EXPERIMENTAL

// CTC functions that use GPIO inputs instead of serial monitor for user input

void ctc(Adafruit_SSD1306* display, uint8_t button) {
  int baseX = display->getCursorX();
  int baseY = display->getCursorY();
  uint8_t color = 0;

  while(digitalRead(button)) continue;

  long prev = millis();
  long now;

  while(!digitalRead(button)) {
    now = millis();
    if (now - prev > 500) {
      prev = now;
      color = !color;
      display->setCursor(116, 0);
      display->setTextColor(color);
      display->write(">>");
      display->display();
    }
  }

  display->setCursor(116, 0);
  display->setTextColor(0);
  display->write(">>");
  display->display();

  while (Serial.available()) Serial.read();   // clear serial buffer for next CTC call
  display->setCursor(baseX, baseY);
  display->setTextColor(1);
}

void ctcTimed(Adafruit_SSD1306* display, uint8_t button, long timerMS) {
  int baseX = display->getCursorX();
  int baseY = display->getCursorY();
  uint8_t color = 0;

  while (Serial.available()) Serial.read();   // clear any serial artifacts
  
  long basetime = millis();
  long prev = millis();
  long now = millis();

  while(!Serial.available() && now - basetime < timerMS) {
    now = millis();
    if (now - prev > 500) {
      prev = now;
      color = !color;
      display->setCursor(116, 0);
      display->setTextColor(color);
      display->write(">>");
      display->display();
    }
  }

  display->setCursor(116, 0);
  display->setTextColor(0);
  display->write(">>");
  display->display();

  while (Serial.available()) Serial.read();   // clear serial buffer for next CTC call
  display->setCursor(baseX, baseY);
  display->setTextColor(1);
}