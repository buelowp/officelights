/*
 * Twinkles.h
 *
 *  Created on: Dec 17, 2015
 *      Author: pete
 */

#ifndef USER_APPLICATIONS_WINDOWLIGHTS_BREATHING_H_
#define USER_APPLICATIONS_WINDOWLIGHTS_BREATHING_H_

#include <FastLED/FastLED.h>
#include <QtCore/QtCore>

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
    int m_numLeds;
    int m_x;
};

#endif /* USER_APPLICATIONS_WINDOWLIGHTS_TWINKLES_H_ */
