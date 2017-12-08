/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2015, Peter Buelow
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 *THE SOFTWARE.
**/
#ifndef __CHRISTMAS_H__
#define __CHRISTMAS_H__

#define GOING_UP            0
#define GOING_DOWN          1
#define RETURN_TO_NORM      2

#define NUM_XMAS_COLORS	    6

#include <QtCore/QtCore>
#include <FastLED/FastLED.h>
#include <map>
#include <vector>

class Christmas {
public:
  Christmas(CRGB*, int, int);
  ~Christmas();

  void startup();
  void action();
  void setFirstActive(int);
  void addOne();
  void seeTheRainbow();
  CHSV getNextPixel();

private:
  bool scale_pixel_up(int i);
  bool scale_pixel_down(int i);
  bool scale_pixel_to_normal(int i);
  void set_new_pixel_color(int i);
  int random(int, int);

  std::vector<CHSV> pixels;
  std::map<int, int> pixelMap;
  int totalPixels;
  std::map<int,int>::size_type m_numActive;
  int index;
  int swap;
  CRGB *strip;
  int m_numLeds;
  int m_normalBright;
  int m_numColors;
};

#endif
