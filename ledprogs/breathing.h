/*
 * Twinkles.h
 *
 *  Created on: Dec 17, 2015
 *      Author: pete
 */

#ifndef USER_APPLICATIONS_WINDOWLIGHTS_BREATHING_H_
#define USER_APPLICATIONS_WINDOWLIGHTS_BREATHING_H_

#include <QtCore/QtCore>
#include <FastLED/FastLED.h>
#include <experimental/random>

class Breathing {
public:
	Breathing(CRGB*, CRGBPalette16);
	virtual ~Breathing();

	void startup();
	void action();
    int value(int);

private:
    CRGB *strip;
    CRGBPalette16 m_CurrentPalette;
    HSVHue m_currentHue;
    int m_numLeds;
    int m_numColors;
    int m_x;
};

#endif /* USER_APPLICATIONS_WINDOWLIGHTS_TWINKLES_H_ */
