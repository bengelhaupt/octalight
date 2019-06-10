/*
   OctaLight
   
   A project by Ben-Noah Engelhaupt (code@bengelhaupt.com) Github: bengelhaupt
   Published under GNU General Public License v3.0
   
   http://bengelhaupt.com/projects/octalight
*/

#ifndef FUNCS_H
#define FUNCS_H

#include "values.h"

#include <NeoPixelBus.h>

typedef struct {
  int count;          //count of the colors
  RgbwColor *colors;  //colors array
} RgbwColors;

typedef enum {
  ORDER,              //order in the array
  RANDOM,             //random order in the array
  RANDOM_SPECTRUM     //random in RGB spectrum
} CHANGE_MODE;


//returns average color shown on whole strip
RgbwColor getAverage(NeoPixelBus<NeoGrbwFeature, NeoEsp8266Dma800KbpsMethod> *strip);

//parses a color from string
RgbwColor getColor(String string);

//parses a colors array from string
RgbwColors getColors(String string);


//shows stroboscope from #startPixel to #endPixel with #colors in #mode with #frequency and #blackPhases numbers of black phases between them
bool stroboscope(NeoPixelBus<NeoGrbwFeature, NeoEsp8266Dma800KbpsMethod> *strip, bool (*icr)(), int startPixel, int endPixel, RgbwColors colors, CHANGE_MODE mode, int blackPhases, int frequency);

//loops for #cycles (<0 for infinity) through RGB spectrum starting at red with #duration for one cycle
bool spectrum(NeoPixelBus<NeoGrbwFeature, NeoEsp8266Dma800KbpsMethod> *strip, bool (*icr)(), int duration, int cycles);

//shows an alternating circle for #cycles with #colors on the outer ring and #innerColor in the inner area with #duration for one cycle
bool alternatingCircle(NeoPixelBus<NeoGrbwFeature, NeoEsp8266Dma800KbpsMethod> *strip, bool (*icr)(), RgbwColors c, RgbwColor innerColor, int duration, int cycles);

//shows a ripple-like animation moving from in to out for #cycles with #colors and #duration for one cycle
bool ripple(NeoPixelBus<NeoGrbwFeature, NeoEsp8266Dma800KbpsMethod> *strip, bool (*icr)(), RgbwColors c, int duration, int cycles);

//shows random colors on each pixel with one single pixel changing in #frequency
bool randomness(NeoPixelBus<NeoGrbwFeature, NeoEsp8266Dma800KbpsMethod> *strip, bool (*icr)(), int frequency);

//shows #max pixels with #starColor randomly distributed, appearing and disappearing with #frequency (per minute)
bool starlight(NeoPixelBus<NeoGrbwFeature, NeoEsp8266Dma800KbpsMethod> *strip, bool (*icr)(), RgbwColor starColor, int frequency, int max);

//sets #color from #startPixel to #endPixel with #transitionSpeed ms
bool setPixelsColor(NeoPixelBus<NeoGrbwFeature, NeoEsp8266Dma800KbpsMethod> *strip, bool (*icr)(), int startPixel, int endPixel, RgbwColor color, int transitionSpeed);

//sets #color of #pixel with #transitionSpeed ms
bool setPixelColor(NeoPixelBus<NeoGrbwFeature, NeoEsp8266Dma800KbpsMethod> *strip, bool (*icr)(), int pixel, RgbwColor color, int transitionSpeed);

#endif
