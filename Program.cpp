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
	m_ledState = false;

	m_buttons = new ButtonManager();
	m_leds = new LEDManager();
	m_hue = new HueManager();

	m_huesm = new QStateMachine(this);
	m_ledsm = new QStateMachine(this);

	QState *hue_off = new QState();
	QState *hue_dp = new QState();
	QState *hue_other = new QState();

	QState *leds_off = new QState();
	QState *leds_on = new QState();

	hue_off->addTransition(this, SIGNAL(startDailyProgram()), hue_dp);
	hue_off->addTransition(this, SIGNAL(hueProgramStarted()), hue_other);
	hue_dp->addTransition(m_hue, SIGNAL(dailyProgramComplete()), hue_off);
	hue_off->addTransition(m_hue, SIGNAL(dailyProgramStarted()), hue_dp);
	hue_dp->addTransition(this, SIGNAL(hueProgramStarted()), hue_other);
	hue_other->addTransition(this, SIGNAL(hueProgramEnded()), hue_off);
	hue_other->addTransition(m_hue, SIGNAL(dailyProgramStarted()), hue_dp);


	leds_off->addTransition(m_leds, SIGNAL(startLedProgram()), leds_on);
	leds_on->addTransition(m_leds, SIGNAL(endLedProgram()), leds_off);
	leds_on->addTransition(this, SIGNAL(endLedProgram()), leds_off);

	connect(hue_dp, SIGNAL(entered()), this, SLOT(runDailyProgram()));
	connect(hue_dp, SIGNAL(exited()), this, SLOT(endDailyProgram()));
	connect(hue_off, SIGNAL(entered()), this, SLOT(hueIsOff()));
	connect(hue_off, SIGNAL(exited()), this, SLOT(hueIsNotOff()));
	connect(hue_other, SIGNAL(entered()), this, SLOT(hueOtherOn()));
	connect(hue_other, SIGNAL(exited()), this, SLOT(hueOtherOff()));

	connect(leds_on, SIGNAL(entered()), this, SLOT(ledsOn()));
	connect(leds_off, SIGNAL(entered()), this, SLOT(ledsOff()));

	m_huesm->addState(hue_off);
	m_huesm->addState(hue_dp);
	m_huesm->addState(hue_other);
	m_huesm->setInitialState(hue_off);
	m_huesm->start();


	m_ledsm->addState(leds_off);
	m_ledsm->addState(leds_on);
	m_ledsm->setInitialState(leds_off);
	m_ledsm->start();

	connect(m_hue, SIGNAL(hueBridgeFound()), this, SLOT(hueBridgeFound()));
	connect(m_hue, SIGNAL(hueLightsFound(int)), this, SLOT(hueLightsFound(int)));
	connect(m_hue, SIGNAL(wakeUpTime(int)), this, SLOT(hueWakeUpTime(int)));
	connect(m_buttons, SIGNAL(buttonPressed(int)), this, SLOT(buttonPressed(int)));
	connect(m_buttons, SIGNAL(ready()), this, SLOT(buttonsFound()));
	connect(this, SIGNAL(turnLedsOff()), m_leds, SLOT(turnOff()));
	connect(this, SIGNAL(runLedProgram(int)), m_leds, SLOT(setProgram(int)));
	connect(this, SIGNAL(setLedBrightness(int)), m_leds, SLOT(setBrightness(int)));
	connect(m_leds, SIGNAL(finished()), m_leds, SLOT(deleteLater()));
	connect(m_leds, SIGNAL(programDone(int)), this, SLOT(ledProgramDone(int)));
	connect(this, SIGNAL(endLedProgram()), m_leds, SLOT(endProgram()));
	connect(m_hue, SIGNAL(dailyProgramComplete()), this, SLOT(dailyProgramComplete()));
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

void Program::hueIsOff()
{
	qWarning() << __PRETTY_FUNCTION__;
}

void Program::hueIsNotOff()
{
	qWarning() << __PRETTY_FUNCTION__;
}

void Program::hueOtherOn()
{
	qWarning() << __PRETTY_FUNCTION__;
}

void Program::hueOtherOff()
{
	qWarning() << __PRETTY_FUNCTION__;
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
	m_buttons->setButtonState(0, true);
	m_hue->switchDailyProgramState();
}

void Program::endDailyProgram()
{
	qWarning() << __PRETTY_FUNCTION__;
	m_buttons->setButtonState(0, false);
	emit hueProgramEnded();
}

void Program::ledProgramDone(int p)
{
	qWarning() << __PRETTY_FUNCTION__ << ": Ended program" << p;
	if (p >= 0)
		m_buttons->setButtonState(p, false);
}

void Program::runHueAltProgram()
{
	qWarning() << __PRETTY_FUNCTION__;
	m_hue->endDailyProgram();
}

void Program::hueWakeUpTime(int millis)
{
	qWarning() << __PRETTY_FUNCTION__ << ":" << millis;

	QTimer::singleShot(millis, this, SLOT(timeoutWakeup()));
}

void Program::timeoutWakeup()
{
	emit startDailyProgram();
}

void Program::dailyProgramComplete()
{
	qWarning() << __PRETTY_FUNCTION__;
}

void Program::buttonPressed(int b)
{
	qWarning() << __PRETTY_FUNCTION__ << ": buttonstate for" << b << " is" << m_buttons->buttonState(0);
	switch (b) {
	case 0:
		emit endLedProgram();
		m_hue->switchDailyProgramState();
		break;
	case 1:
	case 2:
	case 3:
	case 4:
		emit runLedProgram(b);
		break;
	case 5:
		emit runLedProgram(b);
		emit hueProgramStarted();
		m_hue->setLightsColor(QColor(Qt::green));
		break;
	case 8:
		emit runLedProgram(b);
		emit hueProgramStarted();
		m_hue->setLightsColor(QColor(Qt::yellow));
		break;
	case 9:
		emit runLedProgram(b);
		emit hueProgramStarted();
		m_hue->setLightsColor(QColor(Qt::red));
		break;
	default:
		qWarning() << __PRETTY_FUNCTION__ << ": Invalid button value b =" << b;
		break;
	}
}
