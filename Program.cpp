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
#include "Program.h"

Program::Program(QObject *parent) : QObject(parent)
{
	m_nextProgram = -1;
	m_currProgram = -1;

	m_buttons = new ButtonManager();
	m_leds = new LEDManager();
	m_hue = new HueManager();


	connect(m_hue, SIGNAL(hueBridgeFound()), this, SLOT(hueBridgeFound()));
	connect(m_hue, SIGNAL(hueLightsFound(int)), this, SLOT(hueLightsFound(int)));
	connect(m_buttons, SIGNAL(buttonPressed(int)), this, SLOT(buttonPressed(int)));
	connect(m_buttons, SIGNAL(ready()), this, SLOT(buttonsFound()));
	connect(this, SIGNAL(turnLedsOff()), m_leds, SLOT(turnOff()));
	connect(this, SIGNAL(runLedProgram(int)), m_leds, SLOT(setProgram(int)));
	connect(this, SIGNAL(setLedBrightness(int)), m_leds, SLOT(setBrightness(int)));
	connect(m_leds, SIGNAL(finished()), m_leds, SLOT(deleteLater()));
	connect(m_leds, SIGNAL(programDone(int)), this, SLOT(ledProgramDone(int)));
	connect(m_hue, SIGNAL(dailyProgramComplete()), this, SLOT(dailyProgramDone()));
}

Program::~Program()
{
}

void Program::init()
{
	m_buttons->start();
	m_leds->start();
}

void Program::hueBridgeFound()
{
}

void Program::hueLightsFound(int c)
{
	qWarning() << __PRETTY_FUNCTION__ << ": found" << c << "lights";
	if (c > 0) {
		m_hue->runDailyProgram();
		m_buttons->setButtonState(0, true);
		m_currProgram = 0;
	}
}

void Program::buttonsFound()
{
	m_buttons->turnLedsOff();
}

void Program::dailyProgramDone()
{
	if (m_currProgram == 0)
		m_buttons->setButtonState(0, false);

	if (m_currProgram > 0)
		emit turnLedsOff();

	m_currProgram = -1;
}

void Program::runDailyProgram()
{
	qWarning() << __PRETTY_FUNCTION__ << ": running daily program";
	m_hue->runDailyProgram();
	m_buttons->setButtonState(0, true);
	m_currProgram = 0;
}

void Program::ledProgramDone(int p)
{
	qWarning() << __PRETTY_FUNCTION__ << ": Ended program" << p;
	m_buttons->setButtonState(p, false);
}

void Program::buttonPressed(int b)
{
	qWarning() << __PRETTY_FUNCTION__ << ": b =" << b << ", state is" << m_buttons->buttonState(b);

	switch (b) {
	case 0:
		qWarning() << __PRETTY_FUNCTION__ << ": m_currProgram =" << m_currProgram;
		qWarning() << __PRETTY_FUNCTION__ << ": m_nextProgram =" << m_nextProgram;
		if (m_currProgram == b) {
			m_currProgram = -1;
			m_hue->turnLightsOff();
			m_buttons->setButtonState(0, false);
		}
		else {
			emit turnLedsOff();
			runDailyProgram();
		}
		break;
	case 1:
	case 2:
	case 3:
	case 4:
		qWarning() << __PRETTY_FUNCTION__ << ": m_currProgram =" << m_currProgram;
		qWarning() << __PRETTY_FUNCTION__ << ": m_nextProgram =" << m_nextProgram;
		if (m_currProgram == b) {
			m_currProgram = 0;
			emit turnLedsOff();
		}
		else {
			emit runLedProgram(b);
			if (m_currProgram > 0)
				m_buttons->setButtonState(m_currProgram, false);
			m_buttons->setButtonState(b, true);
			m_currProgram = b;
		}
		break;
	case 5:
		qWarning() << __PRETTY_FUNCTION__ << ": m_currProgram =" << m_currProgram;
		qWarning() << __PRETTY_FUNCTION__ << ": m_nextProgram =" << m_nextProgram;
		if (m_currProgram == b) {
			runDailyProgram();
			emit turnLedsOff();
		}
		else {
			emit runLedProgram(b);
			m_hue->setLightsColor(QColor(Qt::green));
			m_buttons->setButtonState(m_currProgram, false);
			m_buttons->setButtonState(b, true);
			m_currProgram = b;
		}
		break;
	case 8:
		qWarning() << __PRETTY_FUNCTION__ << ": m_currProgram =" << m_currProgram;
		qWarning() << __PRETTY_FUNCTION__ << ": m_nextProgram =" << m_nextProgram;
		if (m_currProgram == b) {
			runDailyProgram();
			emit turnLedsOff();
		}
		else {
			emit runLedProgram(b);
			m_hue->setLightsColor(QColor(Qt::yellow));
			m_buttons->setButtonState(m_currProgram, false);
			m_buttons->setButtonState(b, true);
			m_currProgram = b;
		}
		break;
	case 9:
		qWarning() << __PRETTY_FUNCTION__ << ": m_currProgram =" << m_currProgram;
		qWarning() << __PRETTY_FUNCTION__ << ": m_nextProgram =" << m_nextProgram;
		if (m_currProgram == b) {
			runDailyProgram();
			emit turnLedsOff();
		}
		else {
			emit runLedProgram(b);
			m_hue->setLightsColor(QColor(Qt::red));
			m_buttons->setButtonState(m_currProgram, false);
			m_buttons->setButtonState(b, true);
			m_currProgram = b;
		}
		break;
	default:
		qWarning() << __PRETTY_FUNCTION__ << ": Invalid button value b =" << b;
		break;
	}
}
