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
	connect(m_buttonManager, SIGNAL(buttonUp(int)), this, SLOT(buttonUp(int)));

	buttonManager = m_buttonManager;
}

ButtonManager::~ButtonManager()
{
	delete m_buttonManager;
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

}

void ButtonManager::buttonUp(int button)
{
	m_buttonManager->toggleButtonLEDState(button);
	emit buttonPressed(button);
}

void ButtonManager::start()
{
	m_buttonManager->queryForDevice();
}

void ButtonManager::turnLedsOff()
{
	m_buttonManager->turnButtonLedsOff();
}

void ButtonManager::turnLedOn(int b)
{
}

void ButtonManager::turnLedOff(int b)
{
}
