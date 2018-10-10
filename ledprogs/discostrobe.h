/*
 * Twinkles.h
 *
 *  Created on: Dec 17, 2015
 *      Author: pete
 */

#ifndef USER_APPLICATIONS_WINDOWLIGHTS_DISCOSTROBE_H_
#define USER_APPLICATIONS_WINDOWLIGHTS_DISCOSTROBE_H_

#include <FastLED/FastLED.h>
#include <QtCore/QtCore>

#define ZOOMING_BEATS_PER_MINUTE 15

class DiscoStrobe {
public:
	DiscoStrobe(CRGB*, CRGBPalette16);
	virtual ~DiscoStrobe();

	void startup();
	void action();

private:
    void discoStrobeWithPalette();
    void delayToSyncFrameRate(uint8_t framesPerSecond);
    void drawRainbowDashes(uint8_t, uint16_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t);
    void discoWorker(uint8_t, uint8_t, int8_t, uint8_t, uint8_t);

    CRGBPalette16 m_CurrentPalette;
    CRGB *m_leds;
    int m_numLeds;
    uint8_t m_strobePhase;
};

#endif /* USER_APPLICATIONS_WINDOWLIGHTS_TWINKLES_H_ */
