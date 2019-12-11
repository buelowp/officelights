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
#include "halloween.h"

#define SCALE_VAL       1
#define SCALE_SAT       2
#define SCALE_VAL_NORM  1

static HSVHue g_HalloweenColorWheel[] = {
    HUE_GREEN,
    HUE_ORANGE,
    HUE_BLUE,
    HUE_PURPLE,
};

Halloween::Halloween(CRGB *s, int p, int a)
{
    Q_UNUSED(p);
    Q_UNUSED(a);
	index = 0;
    strip = s;
    qsrand(QTime::currentTime().msecsSinceStartOfDay());
    swap = random(2, 9);
    
    QSettings settings(QStandardPaths::standardLocations(QStandardPaths::ConfigLocation).first() + "/officelights/officelights.conf", QSettings::IniFormat);
    m_numLeds = settings.value("NumLeds", 75).toInt();
    m_numActive = settings.value("ChristmasNumActive", 10).toInt();
    m_normalBright = settings.value("ChristmasNormalBright", 80).toInt();
    m_numColors = sizeof(g_HalloweenColorWheel) / sizeof(HSVHue);
}

Halloween::~Halloween()
{
}

int Halloween::random(int low, int high)
{
    return (qrand() % (high - low) + 1) + 2;    
}

bool Halloween::scale_pixel_up(int i)
{
    CHSV pixel = pixels[i];
    if (pixel.v == 255 && pixel.s == 0)
        return true;

    if ((pixel.v + SCALE_VAL) >= 255)
        pixel.v = 255;
    else
        pixel.v += SCALE_VAL;

    if ((pixel.s - SCALE_SAT) <= 0)
        pixel.s = 0;
    else
        pixel.s -= SCALE_SAT;

    pixels[i] = pixel;
    return false;
}

bool Halloween::scale_pixel_down(int i)
{
	CHSV pixel = pixels[i];
  if (pixel.v == 0 && pixel.s == 255)
    return true;

  if ((pixel.v - 4) <= 0)
    pixel.v = 0;
  else
    pixel.v -= 4;

  if ((pixel.s + 4) >= 255)
    pixel.s = 255;
  else
    pixel.s += 4;

  pixels[i] = pixel;
  return false;
}

bool Halloween::scale_pixel_to_normal(int i)
{
	CHSV pixel = pixels[i];
	if ((pixel.v + SCALE_VAL_NORM) >= m_normalBright) {
		pixel.v = m_normalBright;
		return true;
	}

	pixel.v += SCALE_VAL_NORM;

	pixels[i] = pixel;
	return false;
}

void Halloween::set_new_pixel_color(int i)
{
    HSVHue color = g_HalloweenColorWheel[std::experimental::randint(0, m_numColors - 1)];
    
    while (pixels[i].h == color) {
        color = g_HalloweenColorWheel[std::experimental::randint(0, m_numColors - 1)];
    }

	pixels[i].v = 0;
	pixels[i].h = color;
	pixels[i].s = 255;
}

void Halloween::startup()
{
	for (int i = 0; i < m_numLeds; i++) {
		CHSV c;
		c.h = g_HalloweenColorWheel[std::experimental::randint(0, m_numColors - 1)];
		c.s = 255;
		c.v = m_normalBright;
		pixels.push_back(c);
	}
	qDebug() << __PRETTY_FUNCTION__ << ": startup with" << pixels.size() << "pixels";
}

/**
 * Initially set 20 pixels active and getting brighter. This number of
 * active pixels might be dynamic, but should grow to be about 20 - 25% of
 * all the pixels
 */
void Halloween::setFirstActive(int c)
{
	int count = 0;
	std::pair<std::map<int,int>::iterator,bool> ret;

	while (count < c) {
		int pixel = random(0, m_numLeds);
		ret = pixelMap.insert(std::pair<int,int>(pixel, GOING_UP));
		count++;
	}
}

void Halloween::action()
{
	std::vector<int> toerase;

	for (auto it = pixelMap.begin(); it != pixelMap.end(); ++it) {
		switch (it->second) {
		case GOING_UP:
			if (scale_pixel_up(it->first)) {
				pixelMap[it->first] = GOING_DOWN;
			}
			break;
		case GOING_DOWN:
			if (scale_pixel_down(it->first)) {
				set_new_pixel_color(it->first);
				pixelMap[it->first] = RETURN_TO_NORM;
			}
			break;
		case RETURN_TO_NORM:
			if (scale_pixel_to_normal(it->first)) {
				int pixel = it->first;
				toerase.push_back(pixel);
			}
			break;
		}
	}
	for (std::vector<int>::size_type i = 0; i < toerase.size(); i++) {
		pixelMap.erase(toerase[i]);
	}

	seeTheRainbow();
}

void Halloween::addOne()
{
	int pixel = random(0, m_numLeds);
	std::pair<std::map<int,int>::iterator,bool> ret;

	if (pixelMap.size() < m_numActive) {
		ret = pixelMap.insert(std::pair<int,int>(pixel, GOING_UP));
	}
}

void Halloween::seeTheRainbow()
{
    CHSV s[m_numLeds];
    for (int j = 0; j < m_numLeds; j++) {
        s[j] = pixels[j];
    }
    hsv2rgb_rainbow(s, strip, m_numLeds);
}
