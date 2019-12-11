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
#include "ledprogs/christmas.h"
#include "ledprogs/christmastree.h"
#include "ledprogs/breathing.h"
#include "ledprogs/discostrobe.h"
#include "ledprogs/halloween.h"
#include "ledprogs/theaterchase.h"

//#define NUM_LEDS	75
#define SPI_BUS    0
#define SPI_CS	   0

#define LED_TYPE    		WS2801
#define COLOR_ORDER 		GRB
#define BRIGHTNESS          96
#define FRAMES_PER_SECOND 	120
#define SPARKING            120
#define COOLING             55

#define PURP 0x6611FF
#define ORAN 0xFF4000
#define GREN 0x00FF11
#define WHIT 0xCCCCCC
#define BLU 0x0000FF

const TProgmemRGBPalette16 Snow_p =
{  0x404040, 0x404040, 0x404040, 0x404040,
   0x404040, 0x404040, 0x404040, 0x404040,
   0x404040, 0x404040, 0x404040, 0x404040,
   0x404040, 0x404040, 0x404040, 0xFFFFFF
};

const CRGBPalette16 HalloweenColors_p
( 
  PURP, PURP, PURP, PURP,
  ORAN, ORAN, ORAN, ORAN,
  BLU, BLU, BLU, BLU,
  GREN, GREN, GREN, WHIT
);

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
    void christmas();
    void blue();
    void violet();
    void white();
    void orange();
    void aqua();
    int randomValue(int, int);
    void christmasTree();
    void Fire2012();
    void halloweenBreathing();
    void discostrobe();
    void halloweenTwinkles();
    void halloween();
    void theaterChase();
    
    Twinkles *m_twinkles;
    Christmas *m_christmas;
    ChristmasTree *m_christmasTree;
    Breathing *m_breathing;
    DiscoStrobe *m_strobe;
    Twinkles *m_halloweenTwinkles;
    Halloween *m_halloween;
    TheaterChase *m_theaterChase;
    
	CRGB m_leds[NUM_LEDS];
	bool m_enabled;
	bool m_allowRun;
	bool m_programChange;
	int m_currentProgram;
    bool m_reverseDirection;

	uint8_t m_CurrentPatternNumber = 0; // Index number of which pattern is current
	uint8_t m_hue = 0; 					// rotating "base color" used by many of the patterns

	QVector< std::function<void(void)> >m_patterns;
};

#endif /* LIGHTSMANAGER_H_ */
