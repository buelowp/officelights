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
#include <FastLED/FastLED.h>

#define NUM_LEDS	25

class LEDManager : public QObject {
	Q_OBJECT
public:
	LEDManager(QObject *parent = 0);
	virtual ~LEDManager();

	bool isEnabled() { return m_enabled; }

public slots:
	void process();
	void setBrightness(uint8_t);
	void setRGB(uint8_t, uint8_t, uint8_t);
	void turnOff();
	void runProgram(int);

signals:
	void programDone();

private:
	void cylon();
	void snow();
	void fadeall();

	CRGB leds[NUM_LEDS];
	bool m_enabled;
	bool m_allowRun;
};

#endif /* LIGHTSMANAGER_H_ */
