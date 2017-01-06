/*
 * HueManager.cpp
 *
 *  Created on: Jan 5, 2017
 *      Author: buelowp
 */

#include "HueManager.h"

HueManager::HueManager(QObject *parent) : QObject(parent)
{
	m_on = false;
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

	connect(m_progTimer, SIGNAL(timeout()), this, SLOT(progIntTimeout()));
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
	m_on = false;
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
	m_on = true;
}

void HueManager::turnLightsOff()
{
	for (int i = 0; i < m_Lights->rowCount(); i++) {
		Light *light = m_Lights->get(i);
		light->setOn(false);
	}
	m_on = false;
	m_progTimer->stop();
	setTimeout();
}

void HueManager::turnLightOn(int l)
{
	if (l < m_Lights->rowCount()) {
		Light *light = m_Lights->get(l);
		light->setOn(true);
	}
	m_on = true;
}

void HueManager::turnLightOff(int l)
{
	if (l < m_Lights->rowCount()) {
		Light *light = m_Lights->get(l);
		light->setOn(false);
	}
	m_on = false;
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
		if (dt.time().hour() < 7) {
			turnLightsOff();
		}
		else if (dt.time().hour() > 17) {
			turnLightsOff();
		}
		else {
			setLightsCTColor(300);
			setBrightness(254);
		}
	}
	m_progTimer->start(1000 * 60);		// Run change once a minute
}

void HueManager::setTimeout()
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

