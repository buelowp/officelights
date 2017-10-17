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

	QState *lightsOff = new QState();
	QState *lightsOn = new QState();
    QState *lightsInit = new QState();
    QState *lightsChanging = new QState();

//	lightsOff->addTransition(this, SIGNAL(turnLightsOn()), lightsOn);
//	lightsOn->addTransition(this, SIGNAL(turnLightsOff()), lightsOff);
    lightsInit->addTransition(this, SIGNAL(initializationDone()), lightsOff);
    lightsChanging->addTransition(this, SIGNAL(allLightsOn()), lightsOn);
    lightsChanging->addTransition(this, SIGNAL(allLightsOff()), lightsOff);
    lightsOff->addTransition(this, SIGNAL(pendingOnStateChange()), lightsChanging);
    lightsOn->addTransition(this, SIGNAL(pendingOffStateChange()), lightsChanging);
    
	connect(lightsOn, SIGNAL(entered()), this, SLOT(turnHueLightsOn()));
//    connect(lightsOn, SIGNAL(entered()), this, SLOT(runNextEvent()));
	connect(lightsOff, SIGNAL(entered()), this, SLOT(turnHueLightsOff()));
//    connect(lightsOff, SIGNAL(entered()), this, SLOT(runNextEvent()));
	connect(this, SIGNAL(lightPowerButtonPressed()), this, SLOT(toggleLights()));

	QState *leds_off = new QState();
	QState *leds_on = new QState();

	leds_off->addTransition(m_leds, SIGNAL(startLedProgram()), leds_on);
	leds_on->addTransition(m_leds, SIGNAL(endLedProgram()), leds_off);
	connect(leds_on, SIGNAL(entered()), this, SLOT(ledsOn()));
	connect(leds_off, SIGNAL(entered()), this, SLOT(ledsOff()));

	m_huesm->addState(lightsOff);
	m_huesm->addState(lightsOn);
    m_huesm->addState(lightsInit);
    m_huesm->addState(lightsChanging);
	m_huesm->setInitialState(lightsInit);
	m_huesm->start();

	m_ledsm->addState(leds_off);
	m_ledsm->addState(leds_on);
	m_ledsm->setInitialState(leds_off);
	m_ledsm->start();

	m_nextEvent = new QTimer();

	connect(m_hue, SIGNAL(hueBridgeFound()), this, SLOT(hueBridgeFound()));
	connect(m_hue, SIGNAL(hueLightsFound(int)), this, SLOT(hueLightsFound(int)));
	connect(m_buttons, SIGNAL(buttonPressed(int)), this, SLOT(buttonPressed(int)));
	connect(m_buttons, SIGNAL(ready()), this, SLOT(buttonsFound()));
	connect(this, SIGNAL(turnLedsOff()), m_leds, SLOT(turnOff()));
	connect(this, SIGNAL(runLedProgram(int)), m_leds, SLOT(setProgram(int)));
	connect(this, SIGNAL(setLedBrightness(int)), m_leds, SLOT(setBrightness(int)));
	connect(m_leds, SIGNAL(finished()), m_leds, SLOT(deleteLater()));
	connect(m_leds, SIGNAL(programDone(int)), this, SLOT(ledProgramDone(int)));
	connect(this, SIGNAL(endLedProgram()), m_leds, SLOT(endProgram()));
	connect(m_nextEvent, SIGNAL(timeout()), this, SLOT(runNextEvent()));
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

void Program::updateLightState(bool state)
{
    if (state) {
        m_turnOnCount--;
        if (m_turnOnCount == 0) {
            emit allLightsOn();
        }
    }
    else {
        m_turnOffCount--;
        if (m_turnOffCount == 0) {
            emit allLightsOff();
        }
    }
}

void Program::updateTurnOffCount()
{
    m_turnOffCount++;
}

void Program::updateTurnOnCount()
{
    m_turnOnCount++;
}

void Program::turnHueLightsOn()
{
    qDebug() << __PRETTY_FUNCTION__;
	m_hue->turnLightsOn();
    emit pendingOnStateChange();
}

void Program::turnHueLightsOff()
{
    qDebug() << __PRETTY_FUNCTION__;
	m_hue->turnLightsOff();
    emit pendingOffStateChange();
}

void Program::runNextEvent()
{
	QDateTime dt = QDateTime::currentDateTime();

    qDebug() << __PRETTY_FUNCTION__;
    
	if (dt.date().dayOfWeek() < 6) {
        qDebug() << __PRETTY_FUNCTION__ << ": it's a weekday";
        
		if ((dt.time().hour() >= 6) && (dt.time().hour() <= 17)) {
			emit turnLightsOn();
			QDateTime next;
			QTime turnOff(17, 0, 1);		// Do it one second past to avoid timing out a few ms early
			next.setDate(dt.date());
			next.setTime(turnOff);
			m_nextEvent->setInterval(dt.msecsTo(next));
			qDebug() << __PRETTY_FUNCTION__ << ":" << __LINE__ << ": " << dt.msecsTo(next);
		}
		else {
			emit turnLightsOff();
			QDateTime next;
			QTime turnOn(6,0,0);
			QDate tomorrow = dt.date();
			tomorrow.addDays(1);
			next.setDate(tomorrow);
			next.setTime(turnOn);
			m_nextEvent->setInterval(dt.msecsTo(next));
			qDebug() << __PRETTY_FUNCTION__ << ":" << __LINE__ << ": " << dt.msecsTo(next);
		}
	}
	else {
        qDebug() << __PRETTY_FUNCTION__ << ": it's a weekend";
		QDateTime next;
		QDate monday = dt.date();
		QTime turnOn(6,0,0);
		monday.addDays(8 - dt.date().dayOfWeek());
		next.setDate(monday);
		next.setTime(turnOn);
		m_nextEvent->setInterval(dt.msecsTo(next));
		qDebug() << __PRETTY_FUNCTION__ << ":" << __LINE__ << ": " << dt.msecsTo(next);
	}
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
	if (c == 4) {
		emit initializationDone();
	}
}

void Program::buttonsFound()
{
	qWarning() << __PRETTY_FUNCTION__;
	m_buttons->turnLedsOff();
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
}

void Program::toggleLights()
{
	if (m_hue->getLightState()) {
		emit turnLightsOff();
	}
	else {
		emit turnLightsOn();
	}
}

void Program::turnOffEvening()
{
	emit turnLightsOff();
}

void Program::buttonPressed(int b)
{
	qWarning() << __PRETTY_FUNCTION__ << ": buttonstate for" << b << " is" << m_buttons->buttonState(0);
	switch (b) {
	case 0:
		toggleLights();
		break;
	case 1:
	case 2:
	case 3:
	case 4:
		emit runLedProgram(b);
		break;
	case 5:
		emit runLedProgram(b);
		emit turnLightsOn();
		m_hue->setLightsColor(QColor(Qt::green));
		break;
	case 8:
		emit runLedProgram(b);
		emit turnLightsOn();
		m_hue->setLightsColor(QColor(Qt::yellow));
		break;
	case 9:
		emit runLedProgram(b);
		emit turnLightsOn();
		m_hue->setLightsColor(QColor(Qt::red));
		break;
	default:
		qWarning() << __PRETTY_FUNCTION__ << ": Invalid button value b =" << b;
		break;
	}
}
