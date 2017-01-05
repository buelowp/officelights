/*
 * ButtonManager.cpp
 *
 *  Created on: Jan 5, 2017
 *      Author: buelowp
 */

#include "ButtonManager.h"

static XKey8 *buttonManager;

extern "C" {
	unsigned int buttonEvent(unsigned char *pData, unsigned int deviceID, unsigned int error)
	{
		return buttonManager->handleDataEvent(pData, deviceID, error);
	}
	unsigned int errorEvent(unsigned int deviceID, unsigned int status)
	{
		return buttonManager->handleErrorEvent(deviceID, status);
	}
}

ButtonManager::ButtonManager(QObject *parent) : QObject(parent)
{
	m_buttonManager = new XKey8();

	m_buttonManager->registerCallback(buttonEvent);
	m_buttonManager->registerErrorCallback(errorEvent);

	connect(m_buttonManager, SIGNAL(panelConnected()), this, SLOT(panelConnected()));
	connect(m_buttonManager, SIGNAL(buttonDown(uint32_t, uint32_t)), this, SLOT(buttonDown(uint32_t, quint32)));
	connect(m_buttonManager, SIGNAL(buttonUp(uint32_t, uint32_t, uint32_t)), this, SLOT(buttonUp(uint32_t, quint32, quint32)));

	m_buttonManager->turnButtonLedsOff();

	buttonManager = m_buttonManager;
}

ButtonManager::~ButtonManager()
{
	delete m_buttonManager;
}

void ButtonManager::panelConnected()
{

}

void ButtonManager::panelDisconnected()
{

}

void ButtonManager::buttonDown(uint32_t button, uint32_t ts)
{

}

void ButtonManager::buttonUp(uint32_t button, uint32_t ts, uint32_t duration)
{
	m_buttonManager->toggleButtonLEDState(button);
	emit buttonPressed(button);
}

void ButtonManager::start()
{
	m_buttonManager->queryForDevice();
}
