/*
Office Light management system
Copyright (C) 2017  Peter Buelow (goballstate at gmail dot com)

This file is part of officelights.

officelights is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

officelights is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with officelights. If not, see <http://www.gnu.org/licenses/>.
*/
#include "LEDManager.h"

LEDManager::LEDManager(QObject *parent) : QThread(parent)
{
	m_enabled = true;
	m_allowRun = false;
	m_currentProgram = -1;
	m_programChange = false;
    m_reverseDirection = false;

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
    m_twinkles = new Twinkles(m_leds, Snow_p);
    m_christmas = new Christmas(m_leds, 55, 10);
    m_christmasTree = new ChristmasTree(m_leds);
    m_breathing = new Breathing(m_leds, HalloweenColors_p);
    m_strobe = new DiscoStrobe(m_leds, HalloweenColors_p);
    m_halloweenTwinkles = new Twinkles(m_leds, HalloweenColors_p);
    m_halloween = new Halloween(m_leds, 55, 10);
    
    QDateTime dt = QDateTime::currentDateTime();
    qsrand(dt.currentMSecsSinceEpoch());
}

LEDManager::~LEDManager()
{
}

int LEDManager::randomValue(int low, int high)
{
    return qrand() % ((high - low) + 1) + low; 
}

void LEDManager::run()
{
	qWarning() << __PRETTY_FUNCTION__ << ": Starting LED control thread";
	turnLedsOff();
	while (1) {
		if (m_programChange) {
            qDebug() << __PRETTY_FUNCTION__ << ": Switching to new program" << m_currentProgram;
			runProgram(m_currentProgram);
			emit programStarted(m_currentProgram);
			m_programChange = false;
		}
		QThread::msleep(1000);
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
	if (m_currentProgram == p) {
        qDebug() << __PRETTY_FUNCTION__ << ": Ending program" << p;
		endProgram(p);
	}
	else {
        qDebug() << __PRETTY_FUNCTION__ << ": New program" << p << "requested";
		// Allow any old program to finish first
		m_allowRun = false;
		QThread::msleep(100);
		m_currentProgram = p;
		m_allowRun = true;
		m_programChange = true;
	}
}

void LEDManager::endProgram(int)
{
	qWarning() << __PRETTY_FUNCTION__;
	if (m_currentProgram >= 0) {
		qWarning() << __PRETTY_FUNCTION__ << ": cleaning up program" << m_currentProgram;
		m_allowRun = false;
        QThread::msleep(100);
		turnLedsOff();
		emit programDone(m_currentProgram);
		m_currentProgram = -1;
		emit endLedProgram();
	}
}

void LEDManager::runProgram(int p)
{
	qWarning() << __PRETTY_FUNCTION__ << ": p =" << p;

	switch (p) {
	case 1:
		cylon();
		break;
	case 2:
		snow();
		break;
	case 3:
		demo();
		break;
	case 4:
		christmas();
		break;
	case 5:
		confetti();
		break;
	case 8:
		Fire2012();
		break;
	case 9:
		red();
		break;
    case 30:
        halloweenBreathing();
        break;
    case 31:
        discostrobe();
        break;
    case 32:
        halloweenTwinkles();
        break;
    case 33:
        halloween();
        break;
    case 34:
        white();
        break;
    case 35:
        orange();
        break;
    case 38:
        violet();
        break;
    case 39:
        aqua();
        break;
	default:
		qWarning() << __PRETTY_FUNCTION__ << ": unknown program" << p;
		break;
	}
}

void LEDManager::discostrobe()
{
    m_strobe->startup();
    FastLED.show();
    while (m_allowRun) {
        m_strobe->action();
        FastLED.show();
        QCoreApplication::processEvents();
    }
    turnLedsOff();
}

void LEDManager::christmasTree()
{
    m_christmasTree->startup();
    FastLED.show();
    while (m_allowRun) {
        m_christmasTree->action();
        FastLED.show();
        QCoreApplication::processEvents();
    }
    turnLedsOff();
}

void LEDManager::setBrightness(int b)
{
	FastLED.setBrightness((uint8_t)b);
	FastLED.show();
}

void LEDManager::red()
{
    qDebug() << __PRETTY_FUNCTION__;
	while (m_allowRun) {
		emit startLedProgram();
		for (int i = 0; i < NUM_LEDS; i++) {
			m_leds[i] = CRGB::Red;
		}
		FastLED.setBrightness(200);
		FastLED.show();
        QCoreApplication::processEvents();

	}
}

void LEDManager::violet()
{
    qDebug() << __PRETTY_FUNCTION__;
	while (m_allowRun) {
		emit startLedProgram();
		for (int i = 0; i < NUM_LEDS; i++) {
			m_leds[i] = CRGB::DarkViolet;
		}
		FastLED.setBrightness(200);
		FastLED.show();
        QCoreApplication::processEvents();

	}
}

void LEDManager::white()
{
    qDebug() << __PRETTY_FUNCTION__;
	while (m_allowRun) {
		emit startLedProgram();
		for (int i = 0; i < NUM_LEDS; i++) {
			m_leds[i] = CRGB::White;
		}
		FastLED.setBrightness(200);
		FastLED.show();
        QCoreApplication::processEvents();

	}
}

void LEDManager::aqua()
{
    qDebug() << __PRETTY_FUNCTION__;
	while (m_allowRun) {
		emit startLedProgram();
		for (int i = 0; i < NUM_LEDS; i++) {
			m_leds[i] = CRGB::Aqua;
		}
		FastLED.setBrightness(200);
		FastLED.show();
        QCoreApplication::processEvents();

	}
}

void LEDManager::orange()
{
    qDebug() << __PRETTY_FUNCTION__;
	while (m_allowRun) {
		emit startLedProgram();
		for (int i = 0; i < NUM_LEDS; i++) {
			m_leds[i] = CRGB::OrangeRed;
		}
		FastLED.setBrightness(200);
		FastLED.show();
        QCoreApplication::processEvents();

	}
}

void LEDManager::yellow()
{
    qDebug() << __PRETTY_FUNCTION__;
	while (m_allowRun) {
		emit startLedProgram();
		for (int i = 0; i < NUM_LEDS; i++) {
			m_leds[i] = CRGB::Yellow;
		}
		FastLED.setBrightness(200);
		FastLED.show();
        QCoreApplication::processEvents();
	}
}

void LEDManager::green()
{
    qDebug() << __PRETTY_FUNCTION__;
	while (m_allowRun) {
		emit startLedProgram();
		for (int i = 0; i < NUM_LEDS; i++) {
			m_leds[i] = CRGB::Green;
		}
		FastLED.setBrightness(200);
		FastLED.show();
        QCoreApplication::processEvents();
	}
}

void LEDManager::blue()
{
    qDebug() << __PRETTY_FUNCTION__;
	while (m_allowRun) {
		emit startLedProgram();
		for (int i = 0; i < NUM_LEDS; i++) {
			m_leds[i] = CRGB::Blue;
		}
		FastLED.setBrightness(200);
		FastLED.show();
        QCoreApplication::processEvents();
	}
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
 * Turn off the LED strip. May not happen instantly, as it should allow the
 * current animation to complete.
 */
void LEDManager::turnOff()
{
	if (m_allowRun)
		m_allowRun = false;
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

    qDebug() << __PRETTY_FUNCTION__;
	for (int i = 0; i < NUM_LEDS; i++) {
		m_leds[i] = CRGB::Red;
	}
	FastLED.setBrightness(brightness);
	FastLED.show();

	while (m_allowRun) {
		emit startLedProgram();
		brightness += direction;
		if (brightness == 255)
			direction = -1;
		if (brightness == 0)
			direction = 1;

		FastLED.setBrightness(brightness);
		FastLED.show();
		QCoreApplication::processEvents();
	}
}

void LEDManager::cylon()
{
    qDebug() << __PRETTY_FUNCTION__;
	static uint8_t hue = 0;
		emit startLedProgram();
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
		QCoreApplication::processEvents();
	}
}

void LEDManager::halloweenBreathing()
{
    qDebug() << __PRETTY_FUNCTION__;
    m_breathing->startup();
    while (m_allowRun) {
        m_breathing->action();
		QCoreApplication::processEvents();
    }
}

void LEDManager::snow()
{
    qDebug() << __PRETTY_FUNCTION__;
    m_twinkles->setDensity(8);
    m_twinkles->setSpeed(4);
    while (m_allowRun) {
        m_twinkles->action();
        FastLED.show();
		QCoreApplication::processEvents();
    }
}

void LEDManager::halloweenTwinkles()
{
    CRGB background(20, 5, 30);

    qDebug() << __PRETTY_FUNCTION__;
    m_halloweenTwinkles->setDensity(8);
    m_halloweenTwinkles->setSpeed(4);
    m_halloweenTwinkles->setBackgroundColor(background);
    while (m_allowRun) {
        m_halloweenTwinkles->action();
        FastLED.show();
		QCoreApplication::processEvents();
    }
}

void LEDManager::christmas()
{
    qDebug() << __PRETTY_FUNCTION__;
    m_christmas->startup();
    m_christmas->setFirstActive(5);
    while (m_allowRun) {
        m_christmas->action();
        m_christmas->seeTheRainbow();
        if (randomValue(1, 4) == 3) {
            m_christmas->addOne();
        }
        FastLED.show();
		QCoreApplication::processEvents();
        QThread::msleep(20);
    }
}

void LEDManager::halloween()
{
    qDebug() << __PRETTY_FUNCTION__;
    m_halloween->startup();
    m_halloween->setFirstActive(5);
    while (m_allowRun) {
        m_halloween->action();
        m_halloween->seeTheRainbow();
        if (randomValue(1, 4) == 3) {
            m_halloween->addOne();
        }
        FastLED.show();
		QCoreApplication::processEvents();
        QThread::msleep(20);
    }
}


// random colored speckles that blink in and fade smoothly
void LEDManager::confetti()
{
    qDebug() << __PRETTY_FUNCTION__;
	fadeToBlackBy(m_leds, NUM_LEDS, 10);
	int pos = random16(NUM_LEDS);
	m_leds[pos] += CHSV(m_hue + random8(64), 200, 255);
}

// a colored dot sweeping back and forth, with fading trails
void LEDManager::sinelon()
{
    qDebug() << __PRETTY_FUNCTION__;
	fadeToBlackBy(m_leds, NUM_LEDS, 20);
	int pos = beatsin16(13,0,NUM_LEDS-1);
	m_leds[pos] += CHSV(m_hue, 255, 192);
}

void LEDManager::bpm()
{
    qDebug() << __PRETTY_FUNCTION__;
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

    qDebug() << __PRETTY_FUNCTION__;
	for (int i = 0; i < 8; i++) {
		m_leds[beatsin16(i+7,0,NUM_LEDS-1)] |= CHSV(dothue, 200, 255);
		dothue += 32;
	}
}

// add one to the current pattern number, and wrap around at the end
void LEDManager::nextPattern()
{
    qDebug() << __PRETTY_FUNCTION__;
	m_CurrentPatternNumber = (m_CurrentPatternNumber + 1) % m_patterns.size();
}

// built-in FastLED rainbow, plus some random sparkly glitter
void LEDManager::rainbowWithGlitter()
{
    qDebug() << __PRETTY_FUNCTION__;
	rainbow();
	addGlitter(80);
}

void LEDManager::addGlitter(fract8 chanceOfGlitter)
{
    qDebug() << __PRETTY_FUNCTION__;
	if (random8() < chanceOfGlitter) {
		m_leds[random16(NUM_LEDS)] += CRGB::White;
	}
}

void LEDManager::rainbow()
{
    qDebug() << __PRETTY_FUNCTION__;
    fill_rainbow(m_leds, NUM_LEDS, m_hue, 7);
}

void LEDManager::demo()
{
    qDebug() << __PRETTY_FUNCTION__;
	while (m_allowRun) {
		emit startLedProgram();
		// Call the current pattern function once, updating the 'leds' array
		m_patterns[m_CurrentPatternNumber]();

		// FastLED.delay will call FastLED.show(), and then delay any
		// remaining milliseconds
		FastLED.delay(1000/FRAMES_PER_SECOND);

		// do some periodic updates
		EVERY_N_MILLISECONDS( 20 ) { m_hue++; } // slowly cycle the "base color" through the rainbow
		EVERY_N_SECONDS( 10 ) { nextPattern(); } // change patterns periodically
		QCoreApplication::processEvents();
	}
}

void LEDManager::Fire2012()
{
    while (m_allowRun) {
        // Array of temperature readings at each simulation cell
        static uint8_t heat[NUM_LEDS];

        // Step 1.  Cool down every cell a little
        for( int i = 0; i < NUM_LEDS; i++) {
            heat[i] = qsub8( heat[i],  random8(0, ((COOLING * 10) / NUM_LEDS) + 2));
        }
  
        // Step 2.  Heat from each cell drifts 'up' and diffuses a little
        for( int k= NUM_LEDS - 1; k >= 2; k--) {
            heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
        }
    
        // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
        if( random8() < SPARKING ) {
            int y = random8(7);
            heat[y] = qadd8( heat[y], random8(160,255) );
        }

        // Step 4.  Map from heat cells to LED colors
        for( int j = 0; j < NUM_LEDS; j++) {
            CRGB color = HeatColor( heat[j]);
            int pixelnumber;
            if ( m_reverseDirection ) {
                pixelnumber = (NUM_LEDS-1) - j;
            } else {
                pixelnumber = j;
            }
            m_leds[pixelnumber] = color;
        }
        FastLED.show(); // display this frame   
        FastLED.delay(1000 / FRAMES_PER_SECOND);
        QCoreApplication::processEvents();
    }
}
