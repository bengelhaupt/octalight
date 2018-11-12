/*
   OctaLight
   
   A project by Ben-Noah Engelhaupt (code@bensoft.de) Github: bensoftde
   Published under GNU General Public License v3.0
   
   http://bensoft.de/projects/octalight
*/

#include "funcs.h"
#include "values.h"

#include <stdlib.h>

int _minimum(int a, int b) {
  if (a < b) {
    return a;
  }
  return b;
}

RgbwColor getAverage(NeoPixelBus<NeoGrbwFeature, NeoEsp8266Dma800KbpsMethod> *strip) {
  long r = 0, g = 0, b = 0, w = 0;
  for (char i = 0; i < PixelCount; i++) {
    RgbwColor c = strip->GetPixelColor(i);
    r += c.R;
    g += c.G;
    b += c.B;
    w += c.W;
  }
  return RgbwColor(r / PixelCount, g / PixelCount, b / PixelCount, w / PixelCount);
}

RgbwColor getColor(String s) {
  char r = strtol(s.substring(0, 2).c_str(), NULL, 16);
  char g = strtol(s.substring(2, 4).c_str(), NULL, 16);
  char b = strtol(s.substring(4, 6).c_str(), NULL, 16);
  char w = strtol(s.substring(6, 8).c_str(), NULL, 16);

  return RgbwColor(r, g, b, w);
}

RgbwColors getColors(String s) {
  int count = strlen(s.c_str()) / 8;
  RgbwColor *results = (RgbwColor*) malloc(sizeof(RgbwColor) * count);

  for (int i = 0; i < count; i++) {
    int offset = 8 * i;
    char r = strtol(s.substring(offset + 0, offset + 2).c_str(), NULL, 16);
    char g = strtol(s.substring(offset + 2, offset + 4).c_str(), NULL, 16);
    char b = strtol(s.substring(offset + 4, offset + 6).c_str(), NULL, 16);
    char w = strtol(s.substring(offset + 6, offset + 8).c_str(), NULL, 16);
    results[i] = RgbwColor(r, g, b, w);
  }

  RgbwColors r = *((RgbwColors*) malloc(sizeof(RgbwColors)));
  r = {count, results};
  return r;
}

bool stroboscope(NeoPixelBus<NeoGrbwFeature, NeoEsp8266Dma800KbpsMethod> *strip, bool (*icr)(), int startPixel, int endPixel, RgbwColors c, CHANGE_MODE mode, int blackPhases, int frequency) {
  int cycle_length = 1000.0f / frequency;
  if (mode == RANDOM_SPECTRUM) {
    while (true) {
      if (setPixelsColor(strip, icr, startPixel, endPixel, RgbwColor(rand() % 256, rand() % 256, rand() % 256, 0), 0)) return true;
      delay(cycle_length);

      for (int c = 0; c < blackPhases; c++) {
        if (setPixelsColor(strip, icr, startPixel, endPixel, RgbwColor(0, 0, 0, 0), 0)) return true;
        delay(cycle_length);
      }
    }
  }
  else if (mode == RANDOM) {
    while (true) {
      int oldrand, newrand = 0;
      while (c.count > 1 && (newrand = rand() % c.count) == oldrand) {
        if (icr()) return true;
      }
      oldrand = newrand;
      if (setPixelsColor(strip, icr, startPixel, endPixel, c.colors[newrand], 0)) return true;
      delay(cycle_length);

      for (int c = 0; c < blackPhases; c++) {
        if (setPixelsColor(strip, icr, startPixel, endPixel, RgbwColor(0, 0, 0, 0), 0)) return true;
        delay(cycle_length);
      }
    }
  }
  else {
    while (true) {
      for (int i = 0; i < c.count; i++) {
        if (setPixelsColor(strip, icr, startPixel, endPixel, c.colors[i], 0)) return true;
        delay(cycle_length);

        for (int c = 0; c < blackPhases; c++) {
          if (setPixelsColor(strip, icr, startPixel, endPixel, RgbwColor(0, 0, 0, 0), 0)) return true;
          delay(cycle_length);
        }
      }
    }
  }

  return false;
}

bool spectrum(NeoPixelBus<NeoGrbwFeature, NeoEsp8266Dma800KbpsMethod> *strip, bool (*icr)(), int duration, int cycles) {
  while (cycles--) {
    if (setPixelsColor(strip, icr, 0, PixelCount - 1, RgbwColor(255, 0, 0, 0), duration / 3)) return true;
    if (setPixelsColor(strip, icr, 0, PixelCount - 1, RgbwColor(0, 255, 0, 0), duration / 3)) return true;
    if (setPixelsColor(strip, icr, 0, PixelCount - 1, RgbwColor(0, 0, 255, 0), duration / 3)) return true;
  }
  return false;
}

bool alternatingCircle(NeoPixelBus<NeoGrbwFeature, NeoEsp8266Dma800KbpsMethod> *strip, bool (*icr)(), RgbwColors colors, RgbwColor inner, int duration, int cycles) {
  int cycle_length = duration / (colors.count * OuterPixelCount);
  if (setPixelsColor(strip, icr, OuterPixelCount, PixelCount - 1, inner, 0)) return true;
  while (cycles--) {
    for (char col = 0; col < colors.count; col++) {
      for (char i = 0; i < OuterPixelCount; i++) {
        strip->SetPixelColor(_minimum(i, OuterPixelCount), colors.colors[col]);
        strip->Show();
        if (icr()) return true;
        delay(cycle_length);
      }
    }
  }

  return false;
}

bool ripple(NeoPixelBus<NeoGrbwFeature, NeoEsp8266Dma800KbpsMethod> *strip, bool (*icr)(), RgbwColors colors, int duration, int cycles) {
  int cycle_length = 16.0f * duration / (colors.count * PixelCount);
  while (cycles--) {
    for (char col = 0; col < colors.count; col++) {
      for (char i = 0; i <= 13; i++) {
        strip->SetPixelColor(101 - i, colors.colors[col]);
        strip->SetPixelColor(102 + i, colors.colors[col]);
        strip->SetPixelColor(129 - i, colors.colors[col]);
        strip->SetPixelColor(130 + i, colors.colors[col]);
        strip->Show();
        if (icr()) return true;
        delay(cycle_length * 2);
      }
      for (char i = 0; i <= 10; i++) {
        strip->SetPixelColor(0 + i, colors.colors[col]);
        strip->SetPixelColor(87 - i, colors.colors[col]);
        strip->SetPixelColor(66 + i, colors.colors[col]);
        strip->SetPixelColor(65 - i, colors.colors[col]);
        strip->SetPixelColor(44 + i, colors.colors[col]);
        strip->SetPixelColor(43 - i, colors.colors[col]);
        strip->SetPixelColor(22 + i, colors.colors[col]);
        strip->SetPixelColor(21 - i, colors.colors[col]);
        strip->Show();
        if (icr()) return true;
        delay(cycle_length);
      }
    }
  }

  return false;
}

bool randomness(NeoPixelBus<NeoGrbwFeature, NeoEsp8266Dma800KbpsMethod> *strip, bool (*icr)(), int frequency) {
  int cycle_length = 1000.0f / frequency;
  for (int i = 0; i < PixelCount; i++) {
    strip->SetPixelColor(i, RgbwColor(rand() % 256, rand() % 256, rand() % 256, 0));
  }
  strip->Show();
  while (true) {
    strip->SetPixelColor(rand() % PixelCount, RgbwColor(rand() % 256, rand() % 256, rand() % 256, 0));
    strip->Show();
    if (icr()) return true;
    delay(cycle_length);
  }
}

bool starlight(NeoPixelBus<NeoGrbwFeature, NeoEsp8266Dma800KbpsMethod> *strip, bool (*icr)(), RgbwColor starColor, int frequency, int max) {
  int filling_cycle_length = 3000.0f / frequency;
  int changing_cycle_length = 120000.0f / frequency;
  if (setPixelsColor(strip, icr, 0, PixelCount - 1, RgbwColor(0, 0, 0, 0), 5000)) return true;
  char positions [max];
  for (char i = 0; i < max; i++) {
    int pos = rand() % PixelCount;
    if (strip->GetPixelColor(pos) == RgbwColor(0, 0, 0, 0)) {
      positions[i] = pos;
      if (setPixelColor(strip, icr, positions[i], starColor, 1000)) return true;
    } else {
      i--;
      continue;
    }
    delay(filling_cycle_length);
  }
  while (true) {
    if (setPixelColor(strip, icr, positions[max - 1], RgbwColor(0, 0, 0, 0), 1000)) return true;
    for (int r = (max - 1); r > 0; r--) {
      positions[r] = positions[r - 1];
    }
    delay(changing_cycle_length);
    positions[0] = rand() % PixelCount;
    if (setPixelColor(strip, icr, positions[0], starColor, 1000)) return true;
    delay(changing_cycle_length);
  }
}

bool setPixelsColor(NeoPixelBus<NeoGrbwFeature, NeoEsp8266Dma800KbpsMethod> *strip, bool (*icr)(), int startPixel, int endPixel, RgbwColor c, int transitionSpeed) {
  if (icr()) return true;
  transitionSpeed /= 10;
  if (transitionSpeed > 0) {
    RgbwColor p = getAverage(strip);
    for (int s = 1; s <= transitionSpeed; s++) {
      for (int i = startPixel; i <= endPixel; i++) {
        RgbwColor transColor (p.R + (c.R - p.R) * ((float)s / transitionSpeed), p.G + (c.G - p.G) * ((float)s / transitionSpeed), p.B + (c.B - p.B) * ((float)s / transitionSpeed), p.W + (c.W - p.W) * ((float)s / transitionSpeed));
        strip->SetPixelColor(i, transColor);
      }
      if (icr()) return true;
      strip->Show();
      delay(10);
    }
  }
  else {
    for (; startPixel <= endPixel; startPixel++)
      strip->SetPixelColor(startPixel, c);
    strip->Show();
  }

  return false;
}

bool setPixelColor(NeoPixelBus<NeoGrbwFeature, NeoEsp8266Dma800KbpsMethod> *strip, bool (*icr)(), int pixel, RgbwColor c, int transitionSpeed) {
  if (icr()) return true;
  transitionSpeed /= 10;
  if (transitionSpeed > 0) {
    RgbwColor p = strip->GetPixelColor(pixel);
    for (int s = 1; s <= transitionSpeed; s++) {
      RgbwColor transColor (p.R + (c.R - p.R) * ((float)s / transitionSpeed), p.G + (c.G - p.G) * ((float)s / transitionSpeed), p.B + (c.B - p.B) * ((float)s / transitionSpeed), p.W + (c.W - p.W) * ((float)s / transitionSpeed));
      strip->SetPixelColor(pixel, transColor);
      if (icr()) return true;
      strip->Show();
      delay(10);
    }
  }
  else {
    strip->SetPixelColor(pixel, c);
    strip->Show();
  }

  return false;
}

