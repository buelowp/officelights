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
	bool buttonState(int);
	void setButtonState(int, bool);

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
	QVector<bool> m_buttonState;

};

#endif /* BUTTONMANAGER_H_ */
