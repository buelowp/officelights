/*
 * LEDManager.h
 *
 *  Created on: Jan 5, 2017
 *      Author: buelowp
 */

#ifndef LIGHTSMANAGER_H_
#define LIGHTSMANAGER_H_

#include <QtCore/QtCore>
#include <iostream>
#include <functional>
#include <FastLED/FastLED.h>

#define NUM_LEDS	25
#define SPI_BUS    0
#define SPI_CS	   0

#define LED_TYPE    		WS2801
#define COLOR_ORDER 		GRB
#define BRIGHTNESS          96
#define FRAMES_PER_SECOND 	120


class LEDManager : public QObject {
	Q_OBJECT
public:
	LEDManager(QObject *parent = 0);
	virtual ~LEDManager();

	bool isEnabled() { return m_enabled; }

public slots:
	void process();
	void setBrightness(int);
	void setRGB(int, int, int);
	void turnOff();
	void runProgram(int);

signals:
	void programDone();

private:
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

	CRGB m_leds[NUM_LEDS];
	bool m_enabled;
	bool m_allowRun;

	uint8_t m_CurrentPatternNumber = 0; // Index number of which pattern is current
	uint8_t m_hue = 0; 					// rotating "base color" used by many of the patterns

	QVector< std::function<void(void)> >m_patterns;
};

#endif /* LIGHTSMANAGER_H_ */
