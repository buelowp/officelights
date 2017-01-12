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

	m_huesm = new QStateMachine(this);
	m_ledsm = new QStateMachine(this);

	QState *hue_off = new QState();
	QState *hue_dp = new QState();
	QState *hue_other = new QState();

	hue_off->addTransition(this, SIGNAL(startDailyProgram()), hue_dp);
	hue_off->addTransition(this, SIGNAL(hueProgramStarted()), hue_other);
	hue_dp->addTransition(m_hue, SIGNAL(dailyProgramComplete()), hue_off);
	hue_dp->addTransition(this, SIGNAL(hueProgramStarted()), hue_other);
	hue_other->addTransition(this, SIGNAL(hueProgramEnded()), hue_off);
	hue_other->addTransition(this, SIGNAL(startDailyProgram()), hue_dp);

	connect(hue_dp, SIGNAL(entered()), this, SLOT(runDailyProgram()));
	connect(hue_dp, SIGNAL(exited()), this, SLOT(endDailyProgram()));
	connect(hue_dp, SIGNAL(entered()), this, SLOT(runHueAltProgram()));

	m_huesm->addState(hue_off);
	m_huesm->addState(hue_dp);
	m_huesm->addState(hue_other);
	m_huesm->setInitialState(hue_off);
	m_huesm->start();

	connect(m_hue, SIGNAL(hueBridgeFound()), this, SLOT(hueBridgeFound()));
	connect(m_hue, SIGNAL(hueLightsFound(int)), this, SLOT(hueLightsFound(int)));
	connect(m_buttons, SIGNAL(buttonPressed(int)), this, SLOT(buttonPressed(int)));
	connect(m_buttons, SIGNAL(ready()), this, SLOT(buttonsFound()));
	connect(this, SIGNAL(turnLedsOff()), m_leds, SLOT(turnOff()));
	connect(this, SIGNAL(runLedProgram(int)), m_leds, SLOT(setProgram(int)));
	connect(this, SIGNAL(setLedBrightness(int)), m_leds, SLOT(setBrightness(int)));
	connect(m_leds, SIGNAL(finished()), m_leds, SLOT(deleteLater()));
	connect(m_leds, SIGNAL(programDone(int)), this, SLOT(ledProgramDone(int)));
}

Program::~Program()
{
}

void Program::init()
{
	qWarning() << __PRETTY_FUNCTION__;
	m_buttons->start();
	m_leds->start();
}

void Program::hueBridgeFound()
{
	qWarning() << __PRETTY_FUNCTION__;
}

void Program::hueLightsFound(int c)
{
	qWarning() << __PRETTY_FUNCTION__ << ": found" << c << "lights";
	if (c > 0) {
		emit startDailyProgram();
	}
}

void Program::buttonsFound()
{
	qWarning() << __PRETTY_FUNCTION__;
	m_buttons->turnLedsOff();
}

void Program::runDailyProgram()
{
	qWarning() << __PRETTY_FUNCTION__;
	m_hue->runDailyProgram();
	m_buttons->setButtonState(0, true);
	m_currProgram = 0;
}

void Program::endDailyProgram()
{
	qWarning() << __PRETTY_FUNCTION__;
	m_buttons->setButtonState(0, false);
}

void Program::ledProgramDone(int p)
{
	qWarning() << __PRETTY_FUNCTION__ << ": Ended program" << p;
	m_buttons->setButtonState(p, false);
}

void Program::runHueAltProgram()
{
	qWarning() << __PRETTY_FUNCTION__;
	m_hue->endDailyProgram();
}

void Program::buttonPressed(int b)
{
	QVector<bool> buttonStates;

	switch (b) {
	case 0:
		m_hue->switchDailyProgramState();
		m_buttons->setButtonState(0, !m_buttons->buttonState(0));
		break;
	case 1:
	case 2:
	case 3:
	case 4:
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
		m_buttons->turnLedsOff();
		m_buttons->setButtonState(b, true);
		emit hueProgramStarted();
		emit runLedProgram(b);
		m_hue->setLightsColor(QColor(Qt::green));
		break;
	case 8:
		m_buttons->turnLedsOff();
		m_buttons->setButtonState(b, true);
		emit hueProgramStarted();
		emit runLedProgram(b);
		m_hue->setLightsColor(QColor(Qt::yellow));
		break;
	case 9:
		m_buttons->turnLedsOff();
		m_buttons->setButtonState(b, true);
		emit hueProgramStarted();
		emit runLedProgram(b);
		m_hue->setLightsColor(QColor(Qt::red));
		break;
	default:
		qWarning() << __PRETTY_FUNCTION__ << ": Invalid button value b =" << b;
		break;
	}
}
