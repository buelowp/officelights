/*
 * Twinkles.h
 *
 *  Created on: Dec 17, 2015
 *      Author: pete
 */

#ifndef USER_APPLICATIONS_WINDOWLIGHTS_TWINKLES_H_
#define USER_APPLICATIONS_WINDOWLIGHTS_TWINKLES_H_

#include <FastLED/FastLED.h>
#include <QtCore/QtCore>

#define NUM_LEDS            75

#define TWINKLE_SPEED 		6

// Overall twinkle density.
// 0 (NONE lit) to 8 (ALL lit at once).
// Default is 5.
#define TWINKLE_DENSITY 	8

class Twinkles {
public:
	Twinkles(CRGB*, CRGBPalette16);
	virtual ~Twinkles();

	void startup();
	void action();
	void setBackgroundColor(CRGB);
	void seeTheRainbow();
	void setDensity(int);
	void setSpeed(int);

private:
	uint8_t attackDecayWave8(uint8_t);
	CRGB computeOneTwinkle(uint32_t ms, uint8_t salt);

	CRGBPalette16 gCurrentPalette;
	CRGB gBackgroundColor;
    CRGB *strip;
	int density;
	int speed;
};

#endif /* USER_APPLICATIONS_WINDOWLIGHTS_TWINKLES_H_ */
