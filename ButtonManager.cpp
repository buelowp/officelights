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
#include "ButtonManager.h"

static XKey8 *g_buttonManagers[4];

extern "C" {
	unsigned int buttonEvent(unsigned char *pData, unsigned int deviceID, unsigned int error)
	{
        qDebug() << __PRETTY_FUNCTION__ << ": handling event for device" << deviceID;
        if (deviceID < 3) {
            return g_buttonManagers[deviceID]->handleDataEvent(pData, deviceID, error);
        }
	}
	unsigned int errorEvent(unsigned int deviceID, unsigned int status)
	{
        if (deviceID < 3) {
            return g_buttonManagers[deviceID]->handleErrorEvent(deviceID, status);
        }
	}
}

ButtonManager::ButtonManager(QObject *parent) : QObject(parent)
{
    m_devicesConnected = 0;
    
	for (int i = 0; i < 10; i++)
		m_buttonState.push_back(false);
}

ButtonManager::~ButtonManager()
{
	delete m_buttonManager;
}

void ButtonManager::setButtonState(int b, bool s)
{
	if (b <= 10) {
		if (s)
			m_buttonManager->setButtonBlueLEDState(b, LEDMode::ON);
		else
			m_buttonManager->setButtonBlueLEDState(b, LEDMode::OFF);

		m_buttonState[b] = s;
	}
}

void ButtonManager::panelConnected(int handle)
{
    if (--m_devicesConnected == 0)
        emit panelReady(handle);
}

void ButtonManager::panelConnected()
{
	qWarning() << __PRETTY_FUNCTION__ << ": Buttons seem to be ready to use";
	emit ready();
}

void ButtonManager::panelDisconnected()
{

}

void ButtonManager::buttonDown(int button, unsigned int ts)
{
	Q_UNUSED(button)
	Q_UNUSED(ts)
}

void ButtonManager::buttonUp(int button)
{
    qWarning() << __PRETTY_FUNCTION__ << ": Shouldn't be here";
}

void ButtonManager::buttonUp(int handle, int button)
{
    XKey8 *panel = m_buttonManagers[handle];
    QVector<bool> buttons = m_buttonState[handle];
    
    if (panel)
        panel->toggleButtonLEDState(button);
    
	buttons[button] = !buttons[button];
	emit buttonPressed(button);
}

void ButtonManager::start()
{
    QVector<int> devices;
    
    for (int i = 0; i < XKey8::queryForDevices(&devices); i++) {
        XKey8 *strip = new XKey8(devices[i]);
        m_buttonManagers[devices[i]] = strip;
        connect(strip, SIGNAL(panelConnected(int)), this, SLOT(panelConnected(int)));
        connect(strip, SIGNAL(buttonUp(int, int)), this, SLOT(buttonUp(int, int)));
        g_buttonManagers[i] = strip;
        strip->registerCallback(buttonEvent);
        strip->registerErrorCallback(errorEvent);
        m_devicesConnected++;
    }

    for (int i = 0; i < devices.size(); i++) {
        m_buttonManagers[devices[i]]->queryForDevice();
    }
}

void ButtonManager::turnLedsOff()
{
    QMapIterator<int, XKey8*> i(m_buttonManagers);
    while (i.hasNext()) {
        i.next();
        i.value()->turnButtonLedsOff();
        QVector<bool> buttons = m_buttonState[i.key()];
        for (int i = 0; i < buttons.size(); i++) {
            buttons[i] = false;
        }
    }
}

void ButtonManager::turnLedOn(int)
{
}

void ButtonManager::turnLedOff(int)
{
}

bool ButtonManager::buttonState(int b)
{
	if (b < 11)
		return m_buttonState[b];

	return false;
}
