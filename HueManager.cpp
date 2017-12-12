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
    m_stateCount = 0;

	m_BridgeStatus = HueBridgeConnection::BridgeStatus::BridgeStatusSearching;
	m_Lights = new Lights();
	m_progTimer = new QTimer(this);
	m_apiKey = m_KeyStore.apiKey();
	m_ipAddr = m_KeyStore.ipAddr();
}

HueManager::~HueManager()
{
}

void HueManager::lightDataChanged(QModelIndex i, QModelIndex j, QVector<int> k)
{
    Q_UNUSED(i)
    Q_UNUSED(j)
    Q_UNUSED(k)
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
    connect(m_Lights, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)), this, SLOT(lightDataChanged(QModelIndex,QModelIndex,QVector<int>)));
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
//    qDebug() << __PRETTY_FUNCTION__ << ": state changed for " << id << "to" << state;
//    qDebug() << __PRETTY_FUNCTION__ << ": lights array has size " << m_Lights->rowCount();
    Q_UNUSED(state)
    qDebug() << __PRETTY_FUNCTION__ << ": got state change for light" << id << ", state count is" << m_stateCount;

    if (m_stateCount != 0) {
        if (id > 0) {
            Light *light = m_Lights->get(m_stateCount - 1);
            if (light) {
                m_stateCount--;
                emit newLightState(id, light->on());
            }
        }
        if (m_stateCount == 0) {
            emit allLightsUpdated();
        }
    }
}

bool HueManager::allLightsAreOn()
{
    for (int i = 0; i < m_Lights->rowCount(); i++) {
        Light *light = m_Lights->get(i);
        if (!light->on()) {
            return false;
        }
    }
    return true;
}

bool HueManager::allLightsAreOff()
{
    for (int i = 0; i < m_Lights->rowCount(); i++) {
        Light *light = m_Lights->get(i);
        if (light->on()) {
            return false;
        }
    }
    return true;
}

/**
 * \func bool HueManager::getLightState()
 * \return Returns true if any one of the lights are on, false if none are on
 */
bool HueManager::getLightState(int i)
{
    if (i < m_Lights->rowCount()) {
		Light *light = m_Lights->get(i);
		if (light->on()) {
			return true;
		}
	}
	return false;
}

void HueManager::bridgeFound()
{
	qDebug() << __FUNCTION__ << ": Found a bridge, searching for lights";
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
{/*
    if (m_stateCount > 0) {
        m_stateCount--;
        qDebug() << __PRETTY_FUNCTION__ << ": Decrementing Hue state change counter to" << m_stateCount;
    }
    else {
        qDebug() << __PRETTY_FUNCTION__ << ": State count is now 0";
        emit allLightsUpdated();
    }

	for (int i = 0; i < m_Lights->rowCount(); i++) {
		Light *light = m_Lights->get(i);
		qDebug() << __PRETTY_FUNCTION__ << "Found light id" << light->id() << ": with name" << light->name() << ", state" << light->on();
        emit allLightsUpdated();
	}
*/
}

void HueManager::apiKeyChanged()
{
	qWarning() << __FUNCTION__ << ": API key has been reset to" << m_Bridge->apiKey();
    m_KeyStore.setApiKey(m_Bridge->apiKey());
}

void HueManager::connectedBridgeChanged()
{
	qDebug() << __FUNCTION__ << ": Got a connectedBridgeChanged() message";
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
            light->setBri(254);
            light->setColor(Qt::white);
            light->setOn(true);
            count++;
        }
	}
    m_stateCount = count;
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
	m_stateCount = count;
	return count;
}

bool HueManager::turnLightOn(int l)
{
	if (l < m_Lights->rowCount()) {
		Light *light = m_Lights->get(l);
        if (!light->on()) {
            light->setOn(true);
            m_stateCount++;
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
            m_stateCount++;
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
            m_stateCount++;
		}
	}
}

void HueManager::setBrightness(int b, int l)
{
	if (b > 0 && b < 255) {
		if (l < m_Lights->rowCount()) {
			Light *light = m_Lights->get(l);
			light->setBri(b);
            m_stateCount++;
		}
	}
}

void HueManager::setLightsColor(QColor c)
{
	m_progTimer->stop();
	for (int i = 0; i < m_Lights->rowCount(); i++) {
		m_Lights->get(i)->setColor(c);
        m_stateCount++;
	}
}

void HueManager::setLightColor(int l, QColor c)
{
	if (l < m_Lights->rowCount()) {
		Light *light = m_Lights->get(l);
		light->setColor(c);
        m_stateCount++;
	}
}

void HueManager::setLightsCTColor(quint16 ct)
{
	if (ct <= 500 || ct >= 153) {
		for (int i = 0; i < m_Lights->rowCount(); i++) {
			Light *light = m_Lights->get(i);
			light->setCt(ct);
            m_stateCount++;
		}
	}
}

void HueManager::setLightCTColor(int l, quint16 ct)
{
	if (ct <= 500 || ct >= 153) {
		if (l < m_Lights->rowCount()) {
			Light *light = m_Lights->get(l);
			light->setCt(ct);
            m_stateCount++;
		}
	}
}

