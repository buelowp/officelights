/*
 * HueManager.cpp
 *
 *  Created on: Jan 5, 2017
 *      Author: buelowp
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
	setTimeout();
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

void HueManager::setLightColor(QColor c)
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

void HueManager::progIntTimeout()
{
	QTime t = QTime::currentTime();

	if (t.hour() < 7) {
		turnLightsOn();
	}
	else if (t.hour() > 17)  {
		turnLightsOff();
	}
	else {
		setLightsCTColor(300);
		setBrightness(254);
	}
}

void HueManager::runDailyProgram()
{
	QDateTime dt = QDateTime::currentDateTime();

	if (dt.date().dayOfWeek() < 6) {
		qWarning() << __PRETTY_FUNCTION__ << ": " << dt.time().hour();
		if (dt.time().hour() < 7) {
			qWarning() << __PRETTY_FUNCTION__ << ": Before I get in";
			emit dailyProgramComplete();
			turnLightsOff();
			m_progTimer->stop();
			setTimeout();
		}
		else if (dt.time().hour() >= 16) {
			qWarning() << __PRETTY_FUNCTION__ << ": I should have left";
			emit dailyProgramComplete();
			turnLightsOff();
			m_progTimer->stop();
			setTimeout();
		}
		else {
			qWarning() << __PRETTY_FUNCTION__ << ": The lights are on";
			turnLightsOn();
			setLightsCTColor(300);
			setBrightness(254);
			if (!m_progTimer->isActive())
				m_progTimer->start(1000 * 60);		// Run change once a minute
		}
	}
}

void HueManager::setTimeout()
{
        QTime t = QTime::currentTime();

        if (t.hour() < 6 || t.hour() >= 19) {
                if (t.hour() < 6) {
                        int millisToWakeup = ((((6 - t.hour()) * 60) + (60 - t.minute())) * 60000);
                        QTimer::singleShot(millisToWakeup, this, SLOT(runDailyProgram()));
                }
                if (t.hour() >= 19) {
                        int millisToWakeup = (((24 - t.hour()) * 60) + (60 - t.minute()) * 60000);
                        QTimer::singleShot(millisToWakeup, this, SLOT(runDailyProgram()));
                }
        }
}

