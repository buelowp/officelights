/*
 * ButtonManager.h
 *
 *  Created on: Jan 5, 2017
 *      Author: buelowp
 */

#ifndef BUTTONMANAGER_H_
#define BUTTONMANAGER_H_

#include <QtCore/QtCore>
#include <XKey8.h>

class ButtonManager : public QObject {
	Q_OBJECT
public:
	ButtonManager(QObject *parent = 0);
	virtual ~ButtonManager();

	void start();
	void turnLedsOff();
	void turnLedOff(int);
	void turnLedOn(int);

public slots:
	void buttonDown(int, unsigned int);
	void buttonUp(int);
	void panelConnected();
	void panelDisconnected();

signals:
	void buttonPressed(int);
	void ready();

private:
	XKey8 *m_buttonManager;

};

#endif /* BUTTONMANAGER_H_ */
