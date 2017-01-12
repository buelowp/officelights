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
	void dailyProgramDone();
	void runDailyProgram();

signals:
	void setLedBrightness(int);
	void setRGB(int, int, int);
	void turnLedsOff();
	void runLedProgram(int);

private:
	ButtonManager *m_buttons;
	LEDManager *m_leds;
	HueManager *m_hue;

	int m_nextProgram;
	int m_currProgram;
};

#endif /* PROGRAM_H_ */
