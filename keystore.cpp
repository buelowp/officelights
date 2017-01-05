/*
 * Copyright 2013 Michael Zanetti
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; version 2.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors:
 *      Michael Zanetti <michael_zanetti@gmx.net>
 */

#include "keystore.h"

KeyStore::KeyStore(QObject *parent): QObject(parent),
    m_settings(QStandardPaths::standardLocations(QStandardPaths::ConfigLocation).first() + "/officelights/officelights.conf", QSettings::IniFormat)
{
}

QString KeyStore::apiKey() const
{
    qDebug() << "reading from" << m_settings.fileName();
    return m_settings.value("apiKey").toString();
}

QString KeyStore::ipAddr() const
{
    qDebug() << "reading from" << m_settings.fileName();
    return m_settings.value("ipAddr").toString();
}

void KeyStore::setApiKey(const QString &apiKey)
{
    m_settings.setValue("apiKey", apiKey);
    emit apiKeyChanged();
}

void KeyStore::setIpAddr(const QString &ipAddr)
{
    m_settings.setValue("ipAddr", ipAddr);
    emit ipAddrChanged();
}
