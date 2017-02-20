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
#include "HueManager.h"

HueManager::HueManager(QObject *parent) : QObject(parent)
{
	m_ct = 153;
	m_BridgeStatus = HueBridgeConnection::BridgeStatus::BridgeStatusSearching;
	m_Lights = new Lights();
	m_progTimer = new QTimer(this);
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

	connect(m_progTimer, SIGNAL(timeout()), this, SLOT(runDailyProgram()));
	connect(m_Lights, SIGNAL(busyChanged()), this, SLOT(lightsBusyChanged()));
	if (m_Bridge) {
		connect(m_Bridge, SIGNAL(bridgeFoundChanged()), this, SLOT(bridgeFound()));
		connect(m_Bridge, SIGNAL(connectedBridgeChanged()), this, SLOT(connectedBridgeChanged()));
		connect(m_Bridge, SIGNAL(apiKeyChanged()), this, SLOT(apiKeyChanged()));
		connect(m_Bridge, SIGNAL(bridgeFoundChanged()), this, SLOT(bridgeFound()));
		connect(m_Bridge, SIGNAL(statusChanged(int)), this, SLOT(bridgeStatusChange(int)));
	}
}

HueManager::~HueManager()
{
}

void HueManager::bridgeFound()
{
	qWarning() << __FUNCTION__ << ": Found a bridge, searching for lights";
	m_Lights->refresh();
	emit hueBridgeFound();
}

void HueManager::lightsBusyChanged()
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
	emit hueLightsFound(m_Lights->rowCount());
}

void HueManager::apiKeyChanged()
{
	qWarning() << __FUNCTION__ << ": API key has been reset to" << m_Bridge->apiKey();
}

void HueManager::connectedBridgeChanged()
{
	qWarning() << __FUNCTION__ << ": Got a connectedBridgeChanged() message";
}

void HueManager::bridgeStatusChange(int num)
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

void HueManager::turnLightsOn()
{
	for (int i = 0; i < m_Lights->rowCount(); i++) {
		Light *light = m_Lights->get(i);
		light->setOn(true);
	}
}

void HueManager::turnLightsOff()
{
	for (int i = 0; i < m_Lights->rowCount(); i++) {
		Light *light = m_Lights->get(i);
		light->setOn(false);
	}
	if (m_progTimer->isActive())
		m_progTimer->stop();
	setIdleTimeout();
}

void HueManager::turnLightOn(int l)
{
	if (l < m_Lights->rowCount()) {
		Light *light = m_Lights->get(l);
		light->setOn(true);
	}
}

void HueManager::turnLightOff(int l)
{
	if (l < m_Lights->rowCount()) {
		Light *light = m_Lights->get(l);
		light->setOn(false);
	}
}

void HueManager::setBrightness(int b)
{
	if (b > 0 && b < 255) {
		for (int i = 0; i < m_Lights->rowCount(); i++) {
			Light *light = m_Lights->get(i);
			light->setBri(b);
		}
	}
}

void HueManager::setBrightness(int b, int l)
{
	if (b > 0 && b < 255) {
		if (l < m_Lights->rowCount()) {
			Light *light = m_Lights->get(l);
			light->setBri(b);
		}
	}
}

void HueManager::setLightsColor(QColor c)
{
	m_progTimer->stop();
	for (int i = 0; i < m_Lights->rowCount(); i++) {
		m_Lights->get(i)->setColor(c);
	}
}

void HueManager::setLightColor(int l, QColor c)
{
	if (l < m_Lights->rowCount()) {
		Light *light = m_Lights->get(l);
		light->setColor(c);
	}
}

void HueManager::setLightsCTColor(quint16 ct)
{
	if (ct <= 500 || ct >= 153) {
		for (int i = 0; i < m_Lights->rowCount(); i++) {
			Light *light = m_Lights->get(i);
			light->setCt(ct);
		}
	}
}

void HueManager::setLightCTColor(int l, quint16 ct)
{
	if (ct <= 500 || ct >= 153) {
		if (l < m_Lights->rowCount()) {
			Light *light = m_Lights->get(l);
			light->setCt(ct);
		}
	}
}

/** Does not turn off the lights, used to stop the timer **/
void HueManager::endDailyProgram()
{
	qWarning() << __PRETTY_FUNCTION__ << ": isActive is" << m_progTimer->isActive();
	if (m_progTimer->isActive())
		m_progTimer->stop();
}

int HueManager::setDailyProgramTimeout()
{
	QTime t = QTime::currentTime();
	QTime et(16, 0, 0);

	return t.secsTo(et);
}

void HueManager::runDailyProgram()
{
	QDateTime dt = QDateTime::currentDateTime();

	if (dt.date().dayOfWeek() < 6) {
		if (dt.time().hour() < 7) {
			emit dailyProgramComplete();
			turnLightsOff();
			m_progTimer->stop();
			setIdleTimeout();
		}
		else if (dt.time().hour() >= 16) {
			emit dailyProgramComplete();
			turnLightsOff();
			m_progTimer->stop();
			setIdleTimeout();
		}
		else {
			if (!m_progTimer->isActive()) {
				int seconds = setDailyProgramTimeout();
				qDebug() << __PRETTY_FUNCTION__ << ": seconds" << seconds;
				if (seconds > 0) {
					qWarning() << __PRETTY_FUNCTION__ << ": Starting daily program to expire in" << seconds << "seconds";
					m_progTimer->start(seconds * 1000 + 10);		//Timeout at 4pm + 10 seconds to avoid falling into a < 1 second boundary
					turnLightsOn();
					setLightsCTColor(300);
					setBrightness(254);
					emit dailyProgramStarted();
				}
			}
		}
	}
}

void HueManager::switchDailyProgramState()
{
	qWarning() << __PRETTY_FUNCTION__ << ": isActive is" << m_progTimer->isActive();
	if (m_progTimer->isActive()) {
		turnLightsOff();
		emit dailyProgramComplete();
	}
	else
		runDailyProgram();
}

void HueManager::setIdleTimeout()
{
	QTime t = QTime::currentTime();
	int millisToWakeup = 0;

	if (t.hour() < 6)
		millisToWakeup = ((((6 - t.hour()) * 60) + (60 - t.minute())) * 60000);
	else
		millisToWakeup = (((24 - t.hour()) * 60) + (60 - t.minute()) * 60000);

	emit wakeUpTime(millisToWakeup);
}

