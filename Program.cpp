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
    m_ledColorProgram = 10;
    m_programInit = true;
    
    m_blockForNotification.unlock();

	m_buttons = new ButtonManager();
	m_leds = new LEDManager();
	m_hue = new HueManager();

	m_huesm = new QStateMachine(this);

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
    lightsOff->addTransition(this, SIGNAL(offProgramSwitchLightState()), lightsSwitchToOn);
    lightsOn->addTransition(this, SIGNAL(offProgramSwitchLightState()), lightsSwitchToOff);
    lightsSwitchEvent->addTransition(this, SIGNAL(turnLightsOn()), lightsSwitchToOn);
    lightsSwitchEvent->addTransition(this, SIGNAL(turnLightsOff()), lightsSwitchToOff);
    lightsEventTimeout->addTransition(this, SIGNAL(startNextEvent()), lightsSwitchEvent);
    
    connect(lightsSwitchEvent, SIGNAL(entered()), this, SLOT(runNextEvent()));
    connect(lightsSwitchToOn, SIGNAL(entered()), this, SLOT(turnHueLightsOn()));
    connect(lightsSwitchToOff, SIGNAL(entered()), this, SLOT(turnHueLightsOff()));
    connect(lightsEventTimeout, SIGNAL(entered()), this, SLOT(runUpdateTimeout()));
    connect(lightsOn, SIGNAL(entered()), this, SLOT(releaseLock()));
    connect(lightsOff, SIGNAL(entered()), this, SLOT(releaseLock()));
    
	connect(this, SIGNAL(lightPowerButtonPressed()), this, SLOT(toggleLights()));
    connect(this, SIGNAL(allLightsOn()), this, SLOT(setButtonLedOn()));
    connect(this, SIGNAL(allLightsOff()), this, SLOT(setButtonLedOff()));

	m_huesm->addState(lightsOff);
	m_huesm->addState(lightsOn);
    m_huesm->addState(lightsInit);
    m_huesm->addState(lightsSwitchToOff);
    m_huesm->addState(lightsSwitchToOn);
    m_huesm->addState(lightsSwitchEvent);
    m_huesm->addState(lightsEventTimeout);
	m_huesm->setInitialState(lightsInit);
	m_huesm->start();

	m_nextEvent = new QTimer();

	connect(m_hue, SIGNAL(hueBridgeFound()), this, SLOT(hueBridgeFound()));
	connect(m_hue, SIGNAL(hueLightsFound(int)), this, SLOT(hueLightsFound(int)));
	connect(m_buttons, SIGNAL(buttonPressed(int)), this, SLOT(buttonPressed(int)));
	connect(m_buttons, SIGNAL(panelReady(int, int)), this, SLOT(buttonsFound(int, int)));
	connect(this, SIGNAL(turnLedsOff()), m_leds, SLOT(turnOff()));
	connect(this, SIGNAL(runLedProgram(int)), m_leds, SLOT(setProgram(int)));
	connect(this, SIGNAL(setLedBrightness(int)), m_leds, SLOT(setBrightness(int)));
	connect(m_leds, SIGNAL(finished()), m_leds, SLOT(deleteLater()));
	connect(m_leds, SIGNAL(programDone(int)), this, SLOT(ledProgramDone(int)));
	connect(this, SIGNAL(endLedProgram(int)), m_leds, SLOT(endProgram(int)));
	connect(m_nextEvent, SIGNAL(timeout()), this, SLOT(runUpdateTimeout()));
    connect(m_hue, SIGNAL(newLightState(int, bool)), this, SLOT(updateLightState(int, bool)));
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
    /*
    if (m_hue->allLightsAreOn()) {
        emit allLightsOn();
    }
    if (m_hue->allLightsAreOff()) {
        emit allLightsOff();
    }
    */
}

void Program::releaseLock()
{
    qDebug() << __PRETTY_FUNCTION__;
    m_blockForNotification.unlock();
}

void Program::setColor(QColor c)
{
    for (int i = 0; i < m_lightCount; i++) {
//        qDebug() << __PRETTY_FUNCTION__ << ": Setting light" << i << "to color" << c;
        m_hue->setLightColor(i, c);
    }
    emit colorChangeComplete();
}

void Program::setBrightness(int b)
{
    for (int i = 0; i < m_lightCount; i++) {
//        qDebug() << __PRETTY_FUNCTION__ << ": Setting light" << i << "to brightness" << b;
        m_hue->setBrightness(i, b);
    }
    emit brightnessChangeComplete();
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
    Q_UNUSED(state)
    qDebug() << __PRETTY_FUNCTION__ << ": Light state change for id" << id;
    m_blockForNotification.unlock();
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
    for (int i = 0; i < m_lightCount; i++) {
        if (!m_hue->getLightState(i)) {
            while (!m_blockForNotification.tryLock()) {
                QCoreApplication::processEvents();
            }
            qDebug() << __PRETTY_FUNCTION__ << ": turning on light" << i;
            m_hue->turnLightOn(i);
        }
    }
    // Block one more time while we wait for the last transition to complete
    while (!m_blockForNotification.tryLock()) {
        QCoreApplication::processEvents();
    }
    setBrightness(254);
    setColor(QColor(Qt::white));
    emit allLightsOn();
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
    qDebug() << __PRETTY_FUNCTION__;
    for (int i = 0; i < m_lightCount; i++) {
        if (m_hue->getLightState(i)) {
            while (!m_blockForNotification.tryLock()) {
                QCoreApplication::processEvents();
            }
            qDebug() << __PRETTY_FUNCTION__ << ": turn off light" << i;
            m_hue->turnLightOff(i);
            toggleLedProgram(m_ledColorProgram);
        }
    }
    // Block one more time while we wait for the last transition to complete
    while (!m_blockForNotification.tryLock()) {
        QCoreApplication::processEvents();
    }    
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
        
		if ((dt.time().hour() >= 6) && (dt.time().hour() < 14)) {
			emit turnLightsOn();
			QDateTime next;
			next.setDate(dt.date());
			next.setTime(QTime(17, 0, 1));
            m_nextEvent->stop();
			m_nextEvent->setInterval(dt.msecsTo(next) + 1000);
            m_nextEvent->start();
			qDebug() << __PRETTY_FUNCTION__ << ": Lights on for the next" << dt.msecsTo(next) << "milliseconds";
		}
		else {
			emit turnLightsOff();
			QDateTime next;
			next.setDate(dt.date().addDays(1));
			next.setTime(QTime(6,0,1));
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
		next.setTime(QTime(6,0,1));
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

void Program::buttonsFound(int handle, int first)
{
    Q_UNUSED(first)
	qWarning() << __PRETTY_FUNCTION__;
	m_buttons->turnLedsOff(handle);
    m_buttons->turnRedLedOff(handle);
    m_buttons->turnGreenLedOff(handle);
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
	if (m_hue->allLightsAreOn()) {
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

void Program::toggleLedProgram(int b)
{
    qDebug() << __PRETTY_FUNCTION__ << ": Toggling LED program" << b;
    if (m_ledColorProgram == b) {
        qDebug() << __PRETTY_FUNCTION__ << ": Ending old program";
        emit endLedProgram(b);
        m_buttons->setButtonState(b, false);
        m_ledColorProgram = 10;
    }
    else {
        qDebug() << __PRETTY_FUNCTION__ << ": Starting new program" << b;
        if (m_ledColorProgram != 10) {
            m_buttons->setButtonState(m_ledColorProgram, false);
        }
        m_ledColorProgram = b;
        m_buttons->setButtonState(m_ledColorProgram, true);
        emit runLedProgram(b);
    }
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
            emit offProgramSwitchLightState();
        }
		break;
	case 1:
//        toggleLedProgram(b);
		emit turnLightsOn();
		m_hue->setLightsColor(QColor(Qt::red));
        m_buttons->setButtonState(m_hueColorProgram, false);
        m_buttons->setButtonState(b, true);
        m_hueColorProgram = b;
		break;
	case 2:
//        toggleLedProgram(b);
		emit turnLightsOn();
		m_hue->setLightsColor(QColor(Qt::green));
        m_buttons->setButtonState(m_hueColorProgram, false);
        m_buttons->setButtonState(b, true);
        m_hueColorProgram = b;
		break;
	case 3:
//        toggleLedProgram(b);
		emit turnLightsOn();
		m_hue->setLightsColor(QColor(Qt::blue));
        m_buttons->setButtonState(m_hueColorProgram, false);
        m_buttons->setButtonState(b, true);
        m_hueColorProgram = b;
		break;
	case 4:
//        toggleLedProgram(b);
		emit turnLightsOn();
		m_hue->setLightsColor(QColor(Qt::yellow));
        m_buttons->setButtonState(m_hueColorProgram, false);
        m_buttons->setButtonState(b, true);
        m_hueColorProgram = b;
		break;
	case 5:
//        toggleLedProgram(b);
		emit turnLightsOn();
		m_hue->setLightsColor(QColor(Qt::cyan));
        m_buttons->setButtonState(m_hueColorProgram, false);
        m_buttons->setButtonState(b, true);
        m_hueColorProgram = b;
		break;
	case 8:
//        toggleLedProgram(b);
		emit turnLightsOn();
		m_hue->setLightsColor(QColor(Qt::yellow));
        m_buttons->setButtonState(m_hueColorProgram, false);
        m_buttons->setButtonState(b, true);
        m_hueColorProgram = b;
		break;
	case 9:
//        toggleLedProgram(b);
		emit turnLightsOn();
		m_hue->setLightsColor(QColor(Qt::red));
        m_buttons->setButtonState(m_hueColorProgram, false);
        m_buttons->setButtonState(b, true);
        m_hueColorProgram = b;
		break;
    case 30:
    case 31:
    case 32:
    case 33:
    case 34:
    case 35:
    case 38:
    case 39:
        toggleLedProgram(b);
        qDebug() << __PRETTY_FUNCTION__ << ": Got second panel button press for button" << b;
        break;
	default:
		qWarning() << __PRETTY_FUNCTION__ << ": Invalid button value b =" << b;
		break;
	}
}
