/*
 * LightsManager.cpp
 *
 *  Created on: Jan 5, 2017
 *      Author: buelowp
 */

#include "LEDManager.h"

LEDManager::LEDManager(QObject *parent) : QObject(parent)
{
	m_enabled = true;
	m_allowRun = false;

    try {
        // addLeds may throw an exception if the SPI interface cannot be found.
        FastLED.addLeds<WS2801, 0, 0, RGB>(leds, NUM_LEDS);
    } catch (std::exception const &exc) {
        std::cerr << "Exception caught " << exc.what() << "\n";
        m_enabled = false;
    } catch (...) {
        std::cerr << "Unknown exception caught\n";
        m_enabled = false;
    }
}

LEDManager::~LEDManager()
{
}

void LEDManager::process()
{
	while (1)
		QThread::sleep(100);
}

void LEDManager::runProgram(int p)
{
	switch (p) {
	case 1:
		m_allowRun = true;
		cylon();
		break;
	case 2:
		m_allowRun = true;
		snow();
		break;
	default:
		break;
	}
}
void LEDManager::setBrightness(uint8_t b)
{
	FastLED.setBrightness(b);
	FastLED.show();
}

void LEDManager::setRGB(uint8_t r, uint8_t g, uint8_t b)
{
	CRGB c;

	c.r = r;
	c.g = g;
	c.b = b;

	for (int i =  0; i < NUM_LEDS; i++) {
		leds[i] = c;
	}
	FastLED.show();
}

/**
 * \func void LEDManager::turnOff()
 * Turn off the LED strip. May not happe instantly, as it should allow the
 * current animation to complete.
 */
void LEDManager::turnOff()
{
	m_allowRun = false;
}

void LEDManager::fadeall()
{
	for(int i = 0; i < NUM_LEDS; i++) {
		leds[i].nscale8(250);
	}
}

void LEDManager::cylon()
{
	static uint8_t hue = 0;
	while (m_allowRun) {
		// First slide the led in one direction
		for(int i = 0; i < NUM_LEDS; i++) {
			// Set the i'th led to red
			leds[i] = CHSV(hue++, 255, 255);
			// Show the leds
			FastLED.show();
			// now that we've shown the leds, reset the i'th led to black
			// leds[i] = CRGB::Black;
			fadeall();
			// Wait a little bit before we loop around and do it again
			QThread::msleep(10);
		}

		// Now go in the other direction.
		for(int i = (NUM_LEDS)-1; i >= 0; i--) {
			// Set the i'th led to red
			leds[i] = CHSV(hue++, 255, 255);
			// Show the leds
			FastLED.show();
			// now that we've shown the leds, reset the i'th led to black
			// leds[i] = CRGB::Black;
			fadeall();
			// Wait a little bit before we loop around and do it again
			QThread::msleep(10);
		}
	}
	FastLED.clear();
	FastLED.show();
	emit programDone();
}

void LEDManager::snow()
{

}
