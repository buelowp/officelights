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
	m_on = false;
	m_ct = 153;
	m_Lights = new Lights();
	m_ProgTimer = new QTimer(this);
	m_apiKey = m_KeyStore.apiKey();
	m_ipAddr = m_KeyStore.ipAddr();
	if (m_apiKey.size()) {
		if (m_ipAddr.size())
			m_Bridge = HueBridgeConnection::instance(m_apiKey, m_ipAddr);
		else
			m_Bridge = HueBridgeConnection::instance(m_apiKey);
	}
	else
		m_Bridge = HueBridgeConnection::instance();

	connect(m_Lights, SIGNAL(busyChanged()), this, SLOT(lightsBusyChanged()));
	connect(m_ProgTimer, SIGNAL(timeout()), this, SLOT(progIntTimeout()));
	if (m_Bridge) {
		connect(m_Bridge, SIGNAL(bridgeFoundChanged()), this, SLOT(bridgeFound()));
		connect(m_Bridge, SIGNAL(connectedBridgeChanged()), this, SLOT(connectedBridgeChanged()));
		connect(m_Bridge, SIGNAL(apiKeyChanged()), this, SLOT(apiKeyChanged()));
		connect(m_Bridge, SIGNAL(bridgeFoundChanged()), this, SLOT(bridgeFound()));
		connect(m_Bridge, SIGNAL(statusChanged(int)), this, SLOT(bridgeStatusChange(int)));
	}
}

Program::~Program()
{
}

void Program::init()
{
	/*
	QTime t = QTime::currentTime();

	if (m_apiKey.size() == 0) {
		m_Bridge->createUser("lights");
	}
	else {
		m_Bridge->setApiKey(m_apiKey);
	}

	connect(m_Bridge, SIGNAL(bridgeFoundChanged()), this, SLOT(bridgeFound()));
	connect(m_Bridge, SIGNAL(statusChanged()), this, SLOT(bridgeStatusChange()));

	if (t.hour() < 6 || t.hour() >= 19) {
		setTimeout();
	}
	else
		runWorkdayProgram();
	*/
}

void Program::bridgeFound()
{
	qWarning() << __FUNCTION__ << ": Found a bridge, searching for lights";
	m_Lights->refresh();
}

void Program::lightsBusyChanged()
{
	qWarning() << "Found" << m_Lights->rowCount() << "lights";
	for (int i = 0; i < m_Lights->rowCount(); i++) {
		Light *light = m_Lights->get(i);
		qWarning() << "Found light" << light->name();
		qWarning() << "with SW version" << light->swversion();
		if (light->on()) {
			light->setOn(false);
		}
	}
	m_on = false;
	runWorkdayProgram();
}

void Program::apiKeyChanged()
{
	qWarning() << __FUNCTION__ << ": API key has been reset to" << m_Bridge->apiKey();
}

void Program::connectedBridgeChanged()
{
	qWarning() << __FUNCTION__ << ": Got a connectedBridgeChanged() message";
}

void Program::bridgeStatusChange(int num)
{
	m_BridgeStatus = m_Bridge->status();

	switch(m_BridgeStatus) {
	case HueBridgeConnection::BridgeStatusConnected:
		qWarning() << __FUNCTION__ << ": Bridge is connected, signal num" << num;
		m_Lights->refresh();
		break;
	case HueBridgeConnection::BridgeStatusConnecting:
		qWarning() << __FUNCTION__ << ": Bridge is connecting, signal num" << num;
		break;
	case HueBridgeConnection::BridgeStatusSearching:
		qWarning() << __FUNCTION__ << ": Searching for a bridge, signal num" << num;
		break;
	case HueBridgeConnection::BridgeStatusAuthenticationFailure:
		qWarning() << __FUNCTION__ << ": Unable to authenticate with API key" << m_apiKey;
		break;
	}
}

void Program::switchTimeout()
{
	runWorkdayProgram();
}

void Program::setTimeout()
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

void Program::turnLightsOn(bool state)
{
	for (int i = 0; i < m_Lights->rowCount(); i++) {
		Light *light = m_Lights->get(i);
		light->setOn(state);
	}
	m_on = state;
}

void Program::setLightBri(int b)
{
	if (b > 0 && b < 255) {
		for (int i = 0; i < m_Lights->rowCount(); i++) {
			Light *light = m_Lights->get(i);
			light->setBri(b);
		}
	}
}

void Program::setLightCTColor(quint16 ct)
{
	if (ct <= 500 || ct >= 153) {
		for (int i = 0; i < m_Lights->rowCount(); i++) {
			Light *light = m_Lights->get(i);
			light->setCt(ct);
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
void Program::progIntTimeout()
{
	QTime t = QTime::currentTime();

	if (t.hour() < 7) {
		qWarning() << __FUNCTION__ << ": Too early, turning lights off";
		turnLightsOn(false);
	}
	else if (t.hour() > 17)  {
		qWarning() << __FUNCTION__ << ": Too late, turning lights off";
		turnLightsOn(false);
	}
	else {
		qWarning() << __FUNCTION__ << ": worktime, turning lights on";
		m_ct = 300;
		setLightCTColor(m_ct);
		setLightBri(254);
	}
}

void Program::runWorkdayProgram()
{
	QDateTime dt = QDateTime::currentDateTime();

	if (dt.date().dayOfWeek() < 6) {
		if (dt.time().hour() < 7) {
			qWarning() << __FUNCTION__ << ": Too early, turning lights off";
			turnLightsOn(false);
		}
		else if (dt.time().hour() > 17) {
			qWarning() << __FUNCTION__ << ": Too late, turning lights off";
			turnLightsOn(false);
		}
		else {
			qWarning() << __FUNCTION__ << ": worktime, turning lights on";
			m_ct = 300;
			setLightCTColor(m_ct);
			setLightBri(254);
		}
	}
/*		
	if (t.hour() < 7) {
		turnLightsOn(false);
	}
		m_ct = 153;
		setLightCTColor(m_ct);
	}
	else if (t.hour() <= 14 && t.minute() <= 47) {
		m_ct = 153 + ((t.hour() * 60) + t.minute()) - 360;
		setLightCTColor(m_ct);
	}
	else {
		m_ct = 500;
		setLightCTColor(m_ct);
	}
*/
	m_ProgTimer->start(1000 * 60);		// Run change once a minute
}
