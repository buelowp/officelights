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
#ifndef HUEMANAGER_H_
#define HUEMANAGER_H_

#include <QtCore/QtCore>
#include <libhue/huebridgeconnection.h>
#include <libhue/light.h>
#include <libhue/lights.h>
#include "keystore.h"

class HueManager : public QObject {
	Q_OBJECT
public:
	HueManager(QObject *parent = 0);
	virtual ~HueManager();

	bool isDailyActive() { return m_progTimer->isActive(); }

public slots:
//	void switchTimeout();
	void runDailyProgram();
	void bridgeFound();
	void bridgeStatusChange(int);
	void lightsBusyChanged();
	void connectedBridgeChanged();
	void apiKeyChanged();

	void turnLightsOn();
	void turnLightsOff();

	void turnLightOn(int);
	void turnLightOff(int);

	void setBrightness(int);
	void setBrightness(int, int);

	void setLightsCTColor(quint16);
	void setLightCTColor(int, quint16);

	void setLightsColor(QColor);
	void setLightColor(int, QColor);

	void switchDailyProgramState();
	void endDailyProgram();


signals:
	void hueBridgeFound();
	void hueLightsFound(int);
	void dailyProgramComplete();
	void dailyProgramStarted();

private:
	void setTimeout();

	enum HueBridgeConnection::BridgeStatus m_BridgeStatus;

	HueBridgeConnection *m_Bridge;
	Lights *m_Lights;
	QSettings m_Settings;
	KeyStore m_KeyStore;
	QString m_apiKey;
	QString m_ipAddr;
	QTimer *m_progTimer;
	int m_ct;
};

#endif /* HUEMANAGER_H_ */
