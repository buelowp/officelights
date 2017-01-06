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
	m_nextProgram = -1;

	m_buttons = new ButtonManager();
	m_leds = new LEDManager();
	m_hue = new HueManager();

	connect(m_hue, SIGNAL(hueBridgeFound()), this, SLOT(hueBridgeFound()));
	connect(m_hue, SIGNAL(hueLightsFound(int)), this, SLOT(hueLightsFound(int)));
	connect(m_buttons, SIGNAL(buttonPressed(int)), this, SLOT(buttonPressed(int)));
	connect(m_buttons, SIGNAL(ready()), this, SLOT(buttonsFound()));
	connect(this, SIGNAL(turnLedsOff()), m_leds, SLOT(turnOff()), Qt::QueuedConnection);
	connect(this, SIGNAL(runLedProgram(int)), m_leds, SLOT(runProgram(int)), Qt::QueuedConnection);
	connect(this, SIGNAL(setLedBrightness(int)), m_leds, SLOT(setBrightness(int)), Qt::QueuedConnection);
	connect(m_leds, SIGNAL(finished()), m_leds, SLOT(deleteLater()));
	connect(m_leds, SIGNAL(programDone()), this, SLOT(ledProgramDone()));
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
	}
}

void Program::buttonsFound()
{
	m_buttons->turnLedsOff();
}

void Program::ledProgramDone()
{
	if (m_nextProgram != -1)
		emit(runLedProgram(m_nextProgram));

	m_nextProgram = -1;
}

void Program::buttonPressed(int b)
{
	qWarning() << __PRETTY_FUNCTION__ << ": b =" << b << ", state is" << m_buttons->buttonState(b);
	switch(b) {
	case 0:
		if (!m_buttons->buttonState(b)) {
			m_hue->turnLightsOff();
		}
		else {
			m_hue->turnLightsOn();
		}
		break;
	case 1:
	case 2:
	case 3:
		qWarning() << __PRETTY_FUNCTION__ << ": m_nextProgram =" << m_nextProgram;
		if (!m_buttons->buttonState(b)) {
			emit(turnLedsOff());
			m_nextProgram = -1;
		}
		else {
			if (m_nextProgram == -1) {
				qWarning() << __PRETTY_FUNCTION__ << ": running program" << b;
				emit(runLedProgram(b));
			}
			else {
				m_nextProgram = b;
				emit(turnLedsOff());
			}
		}
		break;
	default:
		break;
	}
}
