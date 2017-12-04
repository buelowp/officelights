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
        return -1;
	}
	unsigned int errorEvent(unsigned int deviceID, unsigned int status)
	{
        if (deviceID < 3) {
            return g_buttonManagers[deviceID]->handleErrorEvent(deviceID, status);
        }
        return -1;
	}
}

ButtonManager::ButtonManager(QObject *parent) : QObject(parent)
{
    m_devicesConnected = 0;
    
    for (int i = 0; i < 20; i++) {
        m_buttonStates[i] = false;
    }
}

ButtonManager::~ButtonManager()
{
}

void ButtonManager::setButtonState(int b, bool s)
{
    int index = m_buttonHandles[b];
    int button = b % 10;
    
    qDebug() << __PRETTY_FUNCTION__ << ": Acting on button" << b << "whose panel handle is" << index << "and mapped button is" << button;
    
    XKey8* panel = m_buttonManagers[index];
    
    if (panel) {
        m_buttonStates[b] = s;
        if (s)
            panel->setButtonBlueLEDState(b % 10, LEDMode::ON);
        else
            panel->setButtonBlueLEDState(b % 10, LEDMode::OFF);
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
    Q_UNUSED(button);
    qWarning() << __PRETTY_FUNCTION__ << ": Shouldn't be here";
}

void ButtonManager::buttonUp(int handle, int button)
{
    int index = handle * 10;
    XKey8 *panel = m_buttonManagers[handle];
    
    qDebug() << __PRETTY_FUNCTION__ << ": got a button up on panel" << handle << "for button" << button << "which translates to virtual button" << button + index;
    
    if (panel)
        panel->toggleButtonLEDState(button);

    m_buttonStates[button + index] = !m_buttonStates[button + index];
	emit buttonPressed(button + index);
}

void ButtonManager::start()
{
    std::vector<int> devices;
    int deviceCount = XKey8::queryForDevices(&devices);
    int button = 0;
    
    qDebug() << __PRETTY_FUNCTION__ << ": found" << deviceCount << "devices to query";
    for (int i = 0; i < deviceCount; i++) {
        XKey8 *strip = new XKey8(devices[i]);
        m_buttonManagers[devices[i]] = strip;
        for (int j = button; j < button + 10; j++) {
            m_buttonHandles[j] = devices[i];
            qDebug() << __PRETTY_FUNCTION__ << ": assigning button" << j << "to handle" << devices[i];
        }
        button += 10;
        connect(strip, SIGNAL(panelConnected(int)), this, SLOT(panelConnected(int)));
        connect(strip, SIGNAL(buttonUp(int, int)), this, SLOT(buttonUp(int, int)));
        g_buttonManagers[i] = strip;
        strip->registerCallback(buttonEvent);
        strip->registerErrorCallback(errorEvent);
        m_devicesConnected++;
    }

    for (std::vector<int>::size_type i = 0; i < devices.size(); i++) {
        m_buttonManagers[devices[i]]->queryForDevice();
    }
}

void ButtonManager::turnLedsOff()
{
    int offset = 0;
    
    for (int i = 0; i < m_buttonHandles.size(); i++) {
        qDebug() << __PRETTY_FUNCTION__ << ": Setting leds off for device handle" << m_buttonHandles[i];
        XKey8 *panel = m_buttonManagers[m_buttonHandles[i]];
        if (panel) {
            panel->turnButtonLedsOff();
            for (int i = 0; i < 10; i++) {
                m_buttonStates[i + offset] = false;
            }
            offset += 10;
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
    if (b < m_buttonStates.size())
        return m_buttonStates[b];
        
    return false;
}
