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
    m_stateChangeInProgress = 0;

	m_BridgeStatus = HueBridgeConnection::BridgeStatus::BridgeStatusSearching;
	m_Lights = new Lights();
	m_progTimer = new QTimer(this);
	m_apiKey = m_KeyStore.apiKey();
	m_ipAddr = m_KeyStore.ipAddr();
}

HueManager::~HueManager()
{
}

void HueManager::start()
{
	if (m_apiKey.size()) {
		if (m_ipAddr.size())
			m_Bridge = HueBridgeConnection::instance(m_apiKey, m_ipAddr);
		else
			m_Bridge = HueBridgeConnection::instance(m_apiKey);
	}
	else
		m_Bridge = HueBridgeConnection::instance();

	connect(m_Lights, SIGNAL(busyChanged()), this, SLOT(lightsBusyChanged()));
    connect(m_Lights, SIGNAL(updateLightsCount(int)), this, SLOT(updateLightsCount(int)));
//    connect(m_Lights, SIGNAL(stateChanged(int)), this, SLOT(lightStateChanged(int)));
    connect(m_Lights, SIGNAL(lightStateUpdated(int, bool)), this, SLOT(lightStateChanged(int, bool)));
    
	if (m_Bridge) {
		connect(m_Bridge, SIGNAL(bridgeFoundChanged()), this, SLOT(bridgeFound()));
		connect(m_Bridge, SIGNAL(connectedBridgeChanged()), this, SLOT(connectedBridgeChanged()));
		connect(m_Bridge, SIGNAL(apiKeyChanged()), this, SLOT(apiKeyChanged()));
		connect(m_Bridge, SIGNAL(bridgeFoundChanged()), this, SLOT(bridgeFound()));
		connect(m_Bridge, SIGNAL(statusChanged(int)), this, SLOT(bridgeStatusChange(int)));
	}    
}

/**
 * \func void HueManager::lightStateChanged(int id)
 * \param id The Hue numeric for this light
 * \details This happens when we get a notification that the state
 * of a specific light has changed, as a 1 based index from Hue.
 * We translate to a zero index to ask for it from the Lights object.
 */
void HueManager::lightStateChanged(int id, bool state)
{
    bool isOn = false;
    qDebug() << __PRETTY_FUNCTION__ << ": state changed for " << id << "to" << state;
    qDebug() << __PRETTY_FUNCTION__ << ": lights array has size " << m_Lights->rowCount();
    if (id > 0) {
        Light *light = m_Lights->get(id - 1);
        if (light) {
            isOn = light->on();
            emit newLightState(id, isOn);
        }
        else {
            qDebug() << __PRETTY_FUNCTION__ << ": light is NULL, no signal sent";
        }
    }
}

/**
 * \func bool HueManager::getLightState()
 * \return Returns true if any one of the lights are on, false if none are on
 */
bool HueManager::getLightState()
{
	bool state = false;

	for (int i = 0; i < m_Lights->rowCount(); i++) {
		Light *light = m_Lights->get(i);
		if (light->on()) {
			state = true;
		}
	}
	return state;
}

void HueManager::bridgeFound()
{
	qWarning() << __FUNCTION__ << ": Found a bridge, searching for lights";
	m_Lights->refresh();
	emit hueBridgeFound();
}

void HueManager::updateLightsCount(int c)
{
    qDebug() << __PRETTY_FUNCTION__ << ": lights count is now " << c;
    m_stateChangeInProgress = c;
    emit hueLightsFound(c);
}

void HueManager::lightsBusyChanged()
{
	for (int i = 0; i < m_Lights->rowCount(); i++) {
		Light *light = m_Lights->get(i);
		qWarning() << __PRETTY_FUNCTION__ << "Found light" << light->name() << ", state" << light->on();
//		if (light->on()) {
//			light->setOn(false);
//		}
	}
//	emit hueLightsFound(m_Lights->rowCount());
}

void HueManager::apiKeyChanged()
{
	qWarning() << __FUNCTION__ << ": API key has been reset to" << m_Bridge->apiKey();
    m_KeyStore.setApiKey(m_Bridge->apiKey());
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

int HueManager::turnLightsOn()
{
    int count = 0;
    
    qDebug() << __PRETTY_FUNCTION__ << ": Turning on " << m_Lights->rowCount() << " lights";
	for (int i = 0; i < m_Lights->rowCount(); i++) {
		Light *light = m_Lights->get(i);
        if (!light->on()) {
            light->setOn(true);
            count++;
        }
	}
	return count;
}

int HueManager::turnLightsOff()
{
    int count = 0;
    
    qDebug() << __PRETTY_FUNCTION__ << ": Turning off " << m_Lights->rowCount() << " lights";
	for (int i = 0; i < m_Lights->rowCount(); i++) {
		Light *light = m_Lights->get(i);
        if (light->on()) {
            light->setOn(false);
            count++;
        }
	}
	return count;
}

bool HueManager::turnLightOn(int l)
{
	if (l < m_Lights->rowCount()) {
		Light *light = m_Lights->get(l);
        if (!light->on()) {
            light->setOn(true);
            return true;
        }
	}
	return false;
}

bool HueManager::turnLightOff(int l)
{
	if (l < m_Lights->rowCount()) {
		Light *light = m_Lights->get(l);
        if (light->on()) {
            light->setOn(false);
            return true;
        }
	}
	return false;
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

