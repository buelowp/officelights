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

static XKey8 *g_buttonManagers;

extern "C" {
	unsigned int buttonEvent(unsigned char *pData, unsigned int deviceID, unsigned int error)
	{
        if (deviceID < 4) {
            return g_buttonManagers->handleDataEvent(pData, deviceID, error);
        }
        return -1;
	}
	unsigned int errorEvent(unsigned int deviceID, unsigned int status)
	{
        if (deviceID < 4) {
            return g_buttonManagers->handleErrorEvent(deviceID, status);
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
    qDebug() << __PRETTY_FUNCTION__ << ": Setting button" << b << "led to" << s;
    
    if (m_panel) {
        m_buttonStates[b] = s;
        if (s)
            m_panel->setButtonBlueLEDState(b, LEDMode::ON);
        else
            m_panel->setButtonBlueLEDState(b, LEDMode::OFF);
    }
}

void ButtonManager::panelConnected(int handle, int count, int first)
{
    for (int i = 0; i < count; i++) {
        if (i == 6 || i == 7) {
            continue;
        }
        
        m_buttonHandles[first + i] = handle;
    }
    emit panelReady(handle, first);
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
    Q_UNUSED(handle);
    qDebug() << __PRETTY_FUNCTION__ << ": got a button event for button" << button;
/*    
    if (m_panel)
        m_panel->toggleButtonLEDState(button);

    m_buttonStates[button] = !m_buttonStates[button];
    */
	emit buttonPressed(button);
}

void ButtonManager::start()
{
    m_panel = new XKey8();
    g_buttonManagers = m_panel;
    m_panel->registerCallback(buttonEvent);
    m_panel->registerErrorCallback(errorEvent);
    connect(m_panel, SIGNAL(panelConnected(int, int, int)), this, SLOT(panelConnected(int, int, int)));
    connect(m_panel, SIGNAL(buttonUp(int, int)), this, SLOT(buttonUp(int, int)));
    m_panel->queryForDevices();
}

void ButtonManager::turnLedsOff(int handle)
{
    int buttonOffset = handle * 10;
    if (m_panel) {
        m_panel->turnButtonLedsOff(handle);
        for (int i = buttonOffset; i < (buttonOffset + 10); i++) {
            m_buttonStates[i] = false;
        }
    }
}

void ButtonManager::turnLedOn(int button)
{
    if (m_panel) {
        m_panel->setButtonBlueLEDState(button, LEDMode::ON);
    }
}

void ButtonManager::turnLedOff(int button)
{
    if (m_panel) {
        m_panel->setButtonBlueLEDState(button, LEDMode::OFF);
    }
}

bool ButtonManager::buttonState(int b)
{
    if (b < m_buttonStates.size())
        return m_buttonStates[b];
        
    return false;
}

void ButtonManager::turnRedLedOff(int handle)
{
    if (m_panel) {
        m_panel->setPanelLED(handle, PanelLED::RED_LED, LEDMode::OFF);
    }
}

void ButtonManager::turnGreenLedOff(int handle)
{
    if (m_panel) {
        m_panel->setPanelLED(handle, PanelLED::GRN_LED, LEDMode::OFF);
    }
}

void ButtonManager::turnRedLedOn(int handle)
{
    if (m_panel) {
        m_panel->setPanelLED(handle, PanelLED::RED_LED, LEDMode::ON);
    }
}

void ButtonManager::turnGreenLedOn(int handle)
{
    if (m_panel) {
        m_panel->setPanelLED(handle, PanelLED::GRN_LED, LEDMode::ON);
    }
}
