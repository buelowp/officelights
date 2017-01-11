/*
 * LightsManager.cpp
 *
 *  Created on: Jan 5, 2017
 *      Author: buelowp
 */

#include "LEDManager.h"

LEDManager::LEDManager(QObject *parent) : QThread(parent)
{
	m_enabled = true;
	m_allowRun = false;
	m_currentProgram = -1;

    try {
        // addLeds may throw an exception if the SPI interface cannot be found.
        FastLED.addLeds<WS2801, 0, 0, RGB>(m_leds, NUM_LEDS);
    } catch (std::exception const &exc) {
        std::cerr << "Exception caught " << exc.what() << "\n";
        m_enabled = false;
    } catch (...) {
        std::cerr << "Unknown exception caught\n";
        m_enabled = false;
    }

    m_patterns.push_back(std::bind(&LEDManager::rainbow, this));
    m_patterns.push_back(std::bind(&LEDManager::rainbowWithGlitter, this));
    m_patterns.push_back(std::bind(&LEDManager::confetti, this));
    m_patterns.push_back(std::bind(&LEDManager::sinelon, this));
    m_patterns.push_back(std::bind(&LEDManager::juggle, this));
    m_patterns.push_back(std::bind(&LEDManager::bpm, this));
}

LEDManager::~LEDManager()
{
}

void LEDManager::run()
{
	qWarning() << __PRETTY_FUNCTION__ << ": Starting LED control thread";
	turnLedsOff();
	while (1) {
		if (m_currentProgram > 0) {
			runProgram(m_currentProgram);
		}
		QThread::sleep(1);
	}
	qWarning() << __PRETTY_FUNCTION__ << ": exiting thread";
}

void LEDManager::turnLedsOff()
{
	for (int i = 0; i < NUM_LEDS; i++)
		m_leds[i] = CRGB::Black;

	FastLED.show();
}

void LEDManager::setProgram(int p)
{
	m_currentProgram = p;
	m_allowRun = true;
}

void LEDManager::endProgram()
{
	m_allowRun = false;
}

void LEDManager::runProgram(int p)
{
	qWarning() << __PRETTY_FUNCTION__ << ": p =" << p;

	switch (p) {
	case 1:
		m_allowRun = true;
		m_currentProgram = p;
		cylon();
		break;
	case 2:
		m_allowRun = true;
		m_currentProgram = p;
		snow();
		break;
	case 3:
		m_allowRun = true;
		m_currentProgram = p;
		demo();
		break;
	case 4:
		m_allowRun = true;
		m_currentProgram = p;
		pulse();
		break;
	case 5:
		m_allowRun = true;
		m_currentProgram = p;
		green();
		break;
	case 8:
		m_allowRun = true;
		m_currentProgram = p;
		yellow();
		break;
	case 9:
		m_allowRun = true;
		m_currentProgram = p;
		red();
		break;
	default:
		break;
	}
}

void LEDManager::setBrightness(int b)
{
	FastLED.setBrightness((uint8_t)b);
	FastLED.show();
}

void LEDManager::red()
{
	while (m_allowRun) {
		for (int i = 0; i < NUM_LEDS; i++) {
			m_leds[i] = CRGB::Red;
		}
		FastLED.setBrightness(200);
		FastLED.show();
		QThread::msleep(100);
	}
	cleanUpProgram();
}

void LEDManager::yellow()
{
	while (m_allowRun) {
		for (int i = 0; i < NUM_LEDS; i++) {
			m_leds[i] = CRGB::Yellow;
		}
		FastLED.setBrightness(200);
		FastLED.show();
		QThread::msleep(100);
	}
	cleanUpProgram();
}

void LEDManager::green()
{
	while (m_allowRun) {
		for (int i = 0; i < NUM_LEDS; i++) {
			m_leds[i] = CRGB::Green;
		}
		FastLED.setBrightness(200);
		FastLED.show();
		QThread::msleep(100);
	}
	cleanUpProgram();
}

void LEDManager::setRGB(int r, int g, int b)
{
	CRGB c;

	c.r = (uint8_t)r;
	c.g = (uint8_t)g;
	c.b = (uint8_t)b;

	for (int i =  0; i < NUM_LEDS; i++) {
		m_leds[i] = c;
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
	if (m_allowRun)
		m_allowRun = false;
}

void LEDManager::cleanUpProgram()
{
	qWarning() << __PRETTY_FUNCTION__ << ": cleaning up program" << m_currentProgram;
        turnLedsOff();
        FastLED.show();
        m_allowRun = false;
        emit programDone(m_currentProgram);
        m_currentProgram = -1;
}

void LEDManager::fadeall()
{
	for(int i = 0; i < NUM_LEDS; i++) {
		m_leds[i].nscale8(250);
	}
}

void LEDManager::pulse()
{
	uint8_t brightness = 1;
	int direction = 1;

	for (int i = 0; i < NUM_LEDS; i++) {
		m_leds[i] = CRGB::Green;
	}
	FastLED.setBrightness(brightness);
	FastLED.show();

	while (m_allowRun) {
		brightness += direction;
		if (brightness == 255)
			direction = -1;
		if (brightness == 0)
			direction = 1;

		FastLED.setBrightness(brightness);
		FastLED.show();
		QThread::msleep(10);
	}
	cleanUpProgram();
}

void LEDManager::cylon()
{
	static uint8_t hue = 0;
	while (m_allowRun) {
		// First slide the led in one direction
		for(int i = 0; i < NUM_LEDS; i++) {
			// Set the i'th led to red
			m_leds[i] = CHSV(hue++, 255, 255);
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
			m_leds[i] = CHSV(hue++, 255, 255);
			// Show the leds
			FastLED.show();
			// now that we've shown the leds, reset the i'th led to black
			// leds[i] = CRGB::Black;
			fadeall();
			// Wait a little bit before we loop around and do it again
			QThread::msleep(10);
		}
	}
	cleanUpProgram();
}

void LEDManager::snow()
{
	cleanUpProgram();
}

// random colored speckles that blink in and fade smoothly
void LEDManager::confetti()
{
	fadeToBlackBy(m_leds, NUM_LEDS, 10);
	int pos = random16(NUM_LEDS);
	m_leds[pos] += CHSV(m_hue + random8(64), 200, 255);
}

// a colored dot sweeping back and forth, with fading trails
void LEDManager::sinelon()
{
	fadeToBlackBy(m_leds, NUM_LEDS, 20);
	int pos = beatsin16(13,0,NUM_LEDS-1);
	m_leds[pos] += CHSV(m_hue, 255, 192);
}

void LEDManager::bpm()
{
	// colored stripes pulsing at a defined Beats-Per-Minute (BPM)
	uint8_t BeatsPerMinute = 62;
	CRGBPalette16 palette = PartyColors_p;
	uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);

	for( int i = 0; i < NUM_LEDS; i++) { //9948
		m_leds[i] = ColorFromPalette(palette, m_hue+(i*2), beat - m_hue+(i*10));
	}
}

// eight colored dots, weaving in and out of sync with each other
void LEDManager::juggle()
{
	byte dothue = 0;
	fadeToBlackBy(m_leds, NUM_LEDS, 20);

	for (int i = 0; i < 8; i++) {
		m_leds[beatsin16(i+7,0,NUM_LEDS-1)] |= CHSV(dothue, 200, 255);
		dothue += 32;
	}
}

// add one to the current pattern number, and wrap around at the end
void LEDManager::nextPattern()
{
	m_CurrentPatternNumber = (m_CurrentPatternNumber + 1) % m_patterns.size();
}

// built-in FastLED rainbow, plus some random sparkly glitter
void LEDManager::rainbowWithGlitter()
{
	rainbow();
	addGlitter(80);
}

void LEDManager::addGlitter(fract8 chanceOfGlitter)
{
	if (random8() < chanceOfGlitter) {
		m_leds[random16(NUM_LEDS)] += CRGB::White;
	}
}

void LEDManager::rainbow()
{
  fill_rainbow(m_leds, NUM_LEDS, m_hue, 7);
}

void LEDManager::demo()
{
	while (m_allowRun) {
		// Call the current pattern function once, updating the 'leds' array
		m_patterns[m_CurrentPatternNumber]();

		// FastLED.delay will call FastLED.show(), and then delay any
		// remaining milliseconds
		FastLED.delay(1000/FRAMES_PER_SECOND);

		// do some periodic updates
		EVERY_N_MILLISECONDS( 20 ) { m_hue++; } // slowly cycle the "base color" through the rainbow
		EVERY_N_SECONDS( 10 ) { nextPattern(); } // change patterns periodically
	}
	cleanUpProgram();
}
