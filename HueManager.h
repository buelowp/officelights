/*
 * HueManager.h
 *
 *  Created on: Jan 5, 2017
 *      Author: buelowp
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

	void runDailyProgram();

public slots:
//	void switchTimeout();
	void progIntTimeout();
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

signals:
	void hueBridgeFound();
	void hueLightsFound(int);

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
	bool m_on;
};

#endif /* HUEMANAGER_H_ */
