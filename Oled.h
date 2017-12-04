#ifndef __OLED_H__
#define __OLED_H__

#include <QtCore/QtCore>
#include <QtNetwork/QtNetwork>
#include <ssd1306/OledFont8x8.h>
#include <ssd1306/OledI2C.h>

class PiOled {
public:
    PiOled();
    ~PiOled();
    
    void writeIpData();
    
private:
    std::string getHueIp();
    std::string getWifiIp();
    std::string getEthIp();
    
    QVector<QString> m_onScreen;
    SSD1306::OledI2C *m_oled;
};

#endif
