#include "Oled.h"

PiOled::PiOled()
{
    m_oled = new SSD1306::OledI2C("/dev/i2c-1", 0x3C);
    m_oled->clear();
}

PiOled::~PiOled()
{
    m_oled->clear();
    m_oled->displayUpdate();
}

void PiOled::writeIpData()
{
    QVector<std::string> data;
    
    data.push_back(getWifiIp());
    data.push_back(getEthIp());
    data.push_back(getHueIp());
    
    for (int i = 0; i < data.size();i ++) {
        drawString8x8(SSD1306::OledPoint{0, i * 8}, data[i], SSD1306::PixelStyle::Set, *m_oled);
    }
    m_oled->displayUpdate();
}

std::string PiOled::getHueIp()
{
    return std::string("Unknown");
}

std::string PiOled::getWifiIp()
{
    QNetworkInterface wifi = QNetworkInterface::interfaceFromName("wlan0");
    
    if (wifi.isValid()) {
        QList<QHostAddress> ip = wifi.allAddresses();
        return ip.at(0).toString().toStdString();
    }
    return std::string("unknown");
}

std::string PiOled::getEthIp()
{
    QNetworkInterface eth = QNetworkInterface::interfaceFromName("eth0");
    
    if (eth.isValid()) {
        QList<QHostAddress> ip = eth.allAddresses();
        return ip.at(0).toString().toStdString();
    }
    return std::string("unknown");
}
