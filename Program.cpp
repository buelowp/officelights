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
    m_holdState = false;
    m_hueColorProgram = 0;
    m_programInit = true;

	m_buttons = new ButtonManager();
	m_leds = new LEDManager();
	m_hue = new HueManager();

	m_huesm = new QStateMachine(this);
	m_ledsm = new QStateMachine(this);

	QState *lightsOff = new HueOffState();
	QState *lightsOn = new HueOnState();
    QState *lightsInit = new HueInitState();
    QState *lightsSwitchToOff = new HueTransitionToOffState();
    QState *lightsSwitchToOn = new HueTransitionToOnState();
    QState *lightsSwitchEvent = new HueEventState();
    QState *lightsEventTimeout = new HueEventTimeout();

    lightsInit->addTransition(this, SIGNAL(initializationDone()), lightsSwitchEvent);
    lightsSwitchToOn->addTransition(this, SIGNAL(allLightsOn()), lightsOn);
    lightsSwitchToOff->addTransition(this, SIGNAL(allLightsOff()), lightsOff);
    lightsOff->addTransition(this, SIGNAL(startNextEvent()), lightsEventTimeout);
    lightsOn->addTransition(this, SIGNAL(startNextEvent()), lightsEventTimeout);
    lightsSwitchEvent->addTransition(this, SIGNAL(turnLightsOn()), lightsSwitchToOn);
    lightsSwitchEvent->addTransition(this, SIGNAL(turnLightsOff()), lightsSwitchToOff);
    lightsEventTimeout->addTransition(this, SIGNAL(startNextEvent()), lightsSwitchEvent);
    
    connect(lightsSwitchEvent, SIGNAL(entered()), this, SLOT(runNextEvent()));
    connect(lightsSwitchToOn, SIGNAL(entered()), this, SLOT(turnHueLightsOn()));
    connect(lightsSwitchToOff, SIGNAL(entered()), this, SLOT(turnHueLightsOff()));
    connect(lightsEventTimeout, SIGNAL(entered()), this, SLOT(runUpdateTimeout()));
    
	connect(this, SIGNAL(lightPowerButtonPressed()), this, SLOT(toggleLights()));
    connect(this, SIGNAL(allLightsOn()), this, SLOT(setButtonLedOn()));
    connect(this, SIGNAL(allLightsOff()), this, SLOT(setButtonLedOff()));

	QState *leds_off = new QState();
	QState *leds_on = new QState();

	leds_off->addTransition(m_leds, SIGNAL(startLedProgram()), leds_on);
	leds_on->addTransition(m_leds, SIGNAL(endLedProgram()), leds_off);
	connect(leds_on, SIGNAL(entered()), this, SLOT(ledsOn()));
	connect(leds_off, SIGNAL(entered()), this, SLOT(ledsOff()));

	m_huesm->addState(lightsOff);
	m_huesm->addState(lightsOn);
    m_huesm->addState(lightsInit);
    m_huesm->addState(lightsSwitchToOff);
    m_huesm->addState(lightsSwitchToOn);
    m_huesm->addState(lightsSwitchEvent);
    m_huesm->addState(lightsEventTimeout);
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
	connect(m_nextEvent, SIGNAL(timeout()), this, SLOT(runUpdateTimeout()));
    connect(m_hue, SIGNAL(newLightState(int, bool)), this, SLOT(updateLightState(int, bool)));
    connect(m_hue, SIGNAL(allLightsUpdated()), this, SLOT(allLightsUpdated()));
}

Program::~Program()
{
}

void Program::init()
{
	qWarning() << __PRETTY_FUNCTION__;
	m_buttons->start();
	m_leds->start();
    m_hue->start();
    m_programInit = true;
}

void Program::echoLightsOff()
{
    qDebug() << __PRETTY_FUNCTION__;
}

void Program::echoLightsOn()
{
    qDebug() << __PRETTY_FUNCTION__;
}

void Program::allLightsUpdated()
{
    qDebug() << __PRETTY_FUNCTION__;
}

void Program::runUpdateTimeout()
{
    qDebug() << __PRETTY_FUNCTION__;
    emit startNextEvent();
}

/**
 * \func void Program::updateLightState(int id, bool state)
 * \details Sets the state to a value. The state map is reset
 * after each attempt at a state change. Then, we keep track of
 * the state change, and let the system know when we are done.
 * This has a gaping hole I don't know how to fix. That is, 
 * what happens if all of the state changes don't happen
 * as expected
 */
void Program::updateLightState(int id, bool state)
{
    qDebug() << __PRETTY_FUNCTION__ << ": Updating state for light" << id << "to" << state;
    
    m_lightsState[id] = state;
//        qDebug() << __PRETTY_FUNCTION__ << ": m_lightsState.size is" << m_lightsState.size();
//        qDebug() << __PRETTY_FUNCTION__ << ": m_lightCount is" << m_lightCount;
//        qDebug() << __PRETTY_FUNCTION__ << ": Are all lights on?" << m_hue->allLightsAreOn();
//        qDebug() << __PRETTY_FUNCTION__ << ": Are all lights off?" << m_hue->allLightsAreOff();
    if (m_lightsState.size() == m_lightCount) {
//        qDebug() << __PRETTY_FUNCTION__ << ": Handling the state change";
        if (m_hue->allLightsAreOn()) {
//            qDebug() << __PRETTY_FUNCTION__ << ": emitting all lights on";
            emit allLightsOn();
        }
        if (m_hue->allLightsAreOff()) {
//            qDebug() << __PRETTY_FUNCTION__ << ": emitting all lights off";
            emit allLightsOff();
        }
    }
}

void Program::setButtonLedOff()
{
    qDebug() << __PRETTY_FUNCTION__;
    m_buttons->setButtonState(0, false);
}

void Program::setButtonLedOn()
{
    qDebug() << __PRETTY_FUNCTION__;
    m_buttons->setButtonState(0, true);
}

/**
 * \func void Program::turnHueLightsOn()
 * \details Ask the Hue manager to turn all of
 * the lights on. It will return the number of
 * lights it asked to turn off. If that number is
 * zero, no state change complete notification will 
 * come, so we need to send it directly.
 */
void Program::turnHueLightsOn()
{
    int count = 0;
    
    m_lightsState.clear();
// Fix this to set to white and bright instead of just turning on. We're getting too many state changes we can't easily track    
    qDebug() << __PRETTY_FUNCTION__;
    if ((count = m_hue->turnLightsOn()) == 0) {
        qDebug() << __PRETTY_FUNCTION__ << ": All lights already on, just sending the all lights on message";
        emit allLightsOn();
    }
    else {
        qDebug() << __PRETTY_FUNCTION__ << ": count of lights changed is" << count;
    }
}

/**
 * \func void Program::turnHueLightsOff()
 * \details Ask the Hue manager to turn all of
 * the lights off. It will return the number of
 * lights it asked to turn off. If that number is
 * zero, no state change complete notification will 
 * come, so we need to send it directly.
 */
void Program::turnHueLightsOff()
{
    int count = 0;
    
    qDebug() << __PRETTY_FUNCTION__;
    if ((count = m_hue->turnLightsOff()) == 0)
        emit allLightsOff();
}

/**
 * \func void Program::runNextEvent()
 * \detail This is where we go when we finished turning
 * things on or off. It will set a timer to run the
 * next event correctly.
 */
void Program::runNextEvent()
{
	QDateTime dt = QDateTime::currentDateTime();

/*    
    // We don't want to run the next program
    if (m_holdState) {
        m_nextEvent->stop();
        qWarning() << __PRETTY_FUNCTION__ << ": Not setting a new event, waiting for a new button press";
        return;
    }
*/    
    if (dt.date().dayOfWeek() < 6) {
        qDebug() << __PRETTY_FUNCTION__ << ": it's a weekday";
        
		if ((dt.time().hour() >= 6) && ((dt.time().hour() >= 10) && (dt.time().minute() < 42))) {
			emit turnLightsOn();
			QDateTime next;
			next.setDate(dt.date());
			next.setTime(QTime(10, 42, 0));
            m_nextEvent->stop();
			m_nextEvent->setInterval(dt.msecsTo(next) + 1000);
            m_nextEvent->start();
			qDebug() << __PRETTY_FUNCTION__ << ": Lights on for the next" << dt.msecsTo(next) << "milliseconds";
		}
		else {
			emit turnLightsOff();
			QDateTime next;
			next.setDate(dt.date().addDays(1));
			next.setTime(QTime(6,0,0));
            m_nextEvent->stop();
			m_nextEvent->setInterval(dt.msecsTo(next));
            m_nextEvent->start();
			qDebug() << __PRETTY_FUNCTION__ << ": Lights should be off for the next" << dt.msecsTo(next) << "milliseconds";
		}
	}
	else {
        qDebug() << __PRETTY_FUNCTION__ << ": it's a weekend, adding" << 8 - dt.date().dayOfWeek() << "days";
		emit turnLightsOff();
		QDateTime next;
		next.setDate(dt.date().addDays(8 - dt.date().dayOfWeek()));
		next.setTime(QTime(6,0,0));
        m_nextEvent->stop();
		m_nextEvent->setInterval(dt.msecsTo(next));
        m_nextEvent->start();
        qDebug() << __PRETTY_FUNCTION__ << ": Weekend, setting lights to turn on in" << dt.msecsTo(next) << "milliseconds";
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
    m_lightCount = c;
    if (m_programInit) {
        emit initializationDone();
        m_programInit = false;
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
		emit pendingOffStateChange();
	}
	else {
		emit pendingOnStateChange();
	}
    m_holdState = !m_holdState;
    qDebug() << __PRETTY_FUNCTION__ << ": m_holdState" << m_holdState;
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
        if (m_hueColorProgram != 0) {
            m_hue->setLightsColor(QColor(Qt::white));
            m_buttons->setButtonState(m_hueColorProgram, false);
            m_hueColorProgram = 0;
        }
        else {
            toggleLights();
        }
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
        m_buttons->setButtonState(b, true);
        m_buttons->setButtonState(0, false);
        m_hueColorProgram = b;
		break;
	case 8:
		emit runLedProgram(b);
		emit turnLightsOn();
		m_hue->setLightsColor(QColor(Qt::yellow));
        m_buttons->setButtonState(b, true);
        m_buttons->setButtonState(0, false);
        m_hueColorProgram = b;
		break;
	case 9:
		emit runLedProgram(b);
		emit turnLightsOn();
		m_hue->setLightsColor(QColor(Qt::red));
        m_hueColorProgram = b;
        m_buttons->setButtonState(b, true);
        m_buttons->setButtonState(0, false);
		break;
	default:
		qWarning() << __PRETTY_FUNCTION__ << ": Invalid button value b =" << b;
		break;
	}
}
