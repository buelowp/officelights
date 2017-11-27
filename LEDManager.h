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
#ifndef LIGHTSMANAGER_H_
#define LIGHTSMANAGER_H_

#include <QtCore/QtCore>
#include <iostream>
#include <functional>
#include <FastLED/FastLED.h>

#include "ledprogs/twinkles.h"

#define NUM_LEDS	68
#define SPI_BUS    0
#define SPI_CS	   0

#define LED_TYPE    		WS2801
#define COLOR_ORDER 		GRB
#define BRIGHTNESS          96
#define FRAMES_PER_SECOND 	120

const TProgmemRGBPalette16 Snow_p =
{  0x404040, 0x404040, 0x404040, 0x404040,
   0x404040, 0x404040, 0x404040, 0x404040,
   0x404040, 0x404040, 0x404040, 0x404040,
   0x404040, 0x404040, 0x404040, 0xFFFFFF
};

class LEDManager : public QThread {
	Q_OBJECT
public:
	LEDManager(QObject *parent = 0);
	virtual ~LEDManager();

public slots:
	void setBrightness(int);
	void setRGB(int, int, int);
	void turnOff();
	void setProgram(int);
	void endProgram(int);

signals:
	void programDone(int);
	void programStarted(int);
	void startLedProgram();
	void endLedProgram();

protected:
    void run() Q_DECL_OVERRIDE;

private:
	void runProgram(int);
	void cylon();
	void snow();
	void demo();
	void fadeall();
	void rainbow();
	void addGlitter(fract8);
	void rainbowWithGlitter();
	void nextPattern();
	void juggle();
	void bpm();
	void sinelon();
	void confetti();
	void turnLedsOff();
	void pulse();
	void red();
	void yellow();
	void green();

    Twinkles *twinkles;
	CRGB m_leds[NUM_LEDS];
	bool m_enabled;
	bool m_allowRun;
	bool m_programChange;
	int m_currentProgram;

	uint8_t m_CurrentPatternNumber = 0; // Index number of which pattern is current
	uint8_t m_hue = 0; 					// rotating "base color" used by many of the patterns

	QVector< std::function<void(void)> >m_patterns;
};

#endif /* LIGHTSMANAGER_H_ */
