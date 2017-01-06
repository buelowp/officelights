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
	m_hue = new HueManager();
	m_ledThread = new QThread();
	m_leds->moveToThread(m_ledThread);

	connect(m_hue, SIGNAL(hueBridgeFound()), this, SLOT(hueBridgeFound()));
	connect(m_hue, SIGNAL(hueLightsFound()), this, SLOT(hueLightsFound()));
	connect(m_buttons, SIGNAL(buttonPressed(int)), this, SLOT(buttonPressed(int)));
	connect(m_buttons, SIGNAL(ready()), this, SLOT(buttonsFound()));
	connect(this, SIGNAL(turnLedsOff()), m_leds, SLOT(turnOff()));
	connect(this, SIGNAL(runLedProgram(int)), m_leds, SLOT(runProgram(int)));
	connect(this, SIGNAL(setLedBrightess(uint8_t)), m_leds, SLOT(setBrightness(uint8_t)));
	connect(m_ledThread, SIGNAL(started()), m_leds, SLOT(process()));
	connect(m_ledThread, SIGNAL(finished()), m_ledThread, SLOT(deleteLater()));
}

Program::~Program()
{
}

void Program::init()
{
	m_buttons->start();
	m_ledThread->start();
}

void Program::hueBridgeFound()
{
}

void Program::hueLightsFound()
{
	m_hue->runDailyProgram();
}

void Program::buttonsFound()
{
	m_buttons->turnLedsOff();
}

void Program::buttonPressed(int b)
{
	switch(b) {
	case 0:
		if (m_buttons->buttonState(b)) {
			m_hue->turnLightsOff();
		}
		else {
			m_hue->runDailyProgram();
		}
		break;
	case 1:
		if (m_buttons->buttonState(b)) {
			m_leds->turnOff();
		}
		else {
			emit (runLedProgram(b));
		}
		break;
	default:
		break;
	}
}
