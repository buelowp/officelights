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

public slots:
	void buttonDown(uint32_t, uint32_t);
	void buttonUp(uint32_t, uint32_t, uint32_t);
	void panelConnected();
	void panelDisconnected();

signals:
	void buttonPressed(uint32_t);

private:
	XKey8 *m_buttonManager;

};

#endif /* BUTTONMANAGER_H_ */
