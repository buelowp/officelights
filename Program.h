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
#ifndef PROGRAM_H_
#define PROGRAM_H_

#include <QtCore/QtCore>
#include "ButtonManager.h"
#include "LEDManager.h"
#include "HueManager.h"


class Program : public QObject {
	Q_OBJECT
public:
	Program(QObject *parent = 0);
	virtual ~Program();

	void init();

public slots:
	void buttonPressed(int);
	void hueBridgeFound();
	void hueLightsFound(int);
	void buttonsFound();
	void ledProgramDone(int);
	void runHueAltProgram();
	void hueIsOff();
	void hueIsNotOff();
	void hueOtherOn();
	void hueOtherOff();
	void ledsOn() { m_ledState = true; }
	void ledsOff() { m_ledState = false; }

	void turnHueLightsOn();
	void turnHueLightsOff();
	void toggleLights();
    void runNextEvent();
    void updateTurnOnCount();
    void updateTurnOffCount();
    void updateLightState(int, bool);

signals:
	void setLedBrightness(int);
	void setRGB(int, int, int);
	void turnLedsOff();
	void runLedProgram(int);
	void startDailyProgram();
	void hueProgramStarted();
	void hueProgramEnded();
	void endLedProgram();

	void turnLightsOn();
	void turnLightsOff();
	void dailyProgramStarted();
	void dailyProgramComplete();
    void lightPowerButtonPressed();
    void initializationDone();
    void pendingOffStateChange();
    void pendingOnStateChange();
    void allLightsOn();
    void allLightsOff();

private:
	void turnOnMorning();
	void turnOffEvening();

	ButtonManager *m_buttons;
	LEDManager *m_leds;
	HueManager *m_hue;
	QStateMachine *m_huesm;
	QStateMachine *m_ledsm;
	QTimer *m_nextEvent;
    QMap<int, bool> m_lightsState;

    int m_lightCount;
	bool m_ledState;
    int m_turnOffCount;
    int m_turnOnCount;
};

#endif /* PROGRAM_H_ */
