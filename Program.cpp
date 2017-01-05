/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Solutions component.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
** Author: Peter Buelow
** Created: April 24, 2016
****************************************************************************/
#include "Program.h"

Program::Program(QObject *parent) : QObject(parent)
{
	m_buttons = new ButtonManager();
	m_leds = new LEDManager();
//	m_hue = new HueManager();

//	m_progTimer = new QTimer(this);
//	connect(m_progTimer, SIGNAL(timeout()), this, SLOT(progIntTimeout()));
//	connect(m_hue, SIGNAL(hueBridgeFound()), this, SLOT(hueBridgeFound()));
//	connect(m_hue, SIGNAL(hueLightsFound()), this, SLOT(hueLightsFound()));
	connect(m_buttons, SIGNAL(buttonPressed(int)), this, SLOT(buttonPressed(int)));
	connect(m_buttons, SIGNAL(ready()), this, SLOT(buttonsFound()));

	m_buttons->start();
}

Program::~Program()
{
}

void Program::init()
{
}

void Program::hueBridgeFound()
{

}

void Program::hueLightsFound()
{

}

void Program::buttonsFound()
{
	m_buttons->turnLedsOff();
}

void Program::buttonPressed(int b)
{
	m_buttons->turnLedOn(b);
}

void Program::switchProgramTimeout()
{
	runWorkdayProgram();
}

void Program::setProgramTimeout()
{
	QTime t = QTime::currentTime();

	if (t.hour() < 6 || t.hour() >= 19) {
		if (t.hour() < 6) {
			int millisToWakeup = ((((6 - t.hour()) * 60) + (60 - t.minute())) * 60000);
			QTimer::singleShot(millisToWakeup, this, SLOT(timeout()));
		}
		if (t.hour() >= 19) {
			int millisToWakeup = (((24 - t.hour()) * 60) + (60 - t.minute()) * 60000);
			QTimer::singleShot(millisToWakeup, this, SLOT(timeout()));
		}
	}
}


/**
 * \func void Program::progIntTimeout()
 * We assume this is called once a minute. Every minute, it either gets warmer
 * or cooler based on CT value and time of day. If it's morning, get cooler as
 * the morning gets later. If it's afternoon, get warmer. Don't get warmer than 500
 * which is the max value
 */
void Program::programIntTimeout()
{
	QTime t = QTime::currentTime();

	if (t.hour() < 7) {
		qWarning() << __FUNCTION__ << ": Too early, turning lights off";
		m_hue->turnLightsOn();
	}
	else if (t.hour() > 17)  {
		qWarning() << __FUNCTION__ << ": Too late, turning lights off";
		m_hue->turnLightsOff();
	}
	else {
		qWarning() << __FUNCTION__ << ": worktime, turning lights on";
		m_hue->setLightsCTColor(300);
		m_hue->setBrightness(254);
	}
}

void Program::runWorkdayProgram()
{
	QDateTime dt = QDateTime::currentDateTime();

	if (dt.date().dayOfWeek() < 6) {
		if (dt.time().hour() < 7) {
			qWarning() << __FUNCTION__ << ": Too early, turning lights off";
			m_hue->turnLightsOff();
		}
		else if (dt.time().hour() > 17) {
			qWarning() << __FUNCTION__ << ": Too late, turning lights off";
			m_hue->turnLightsOff();
		}
		else {
			qWarning() << __FUNCTION__ << ": worktime, turning lights on";
			m_hue->setLightsCTColor(300);
			m_hue->setBrightness(254);
		}
	}
	m_progTimer->start(1000 * 60);		// Run change once a minute
}
