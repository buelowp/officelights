/*
 * Breathing.cpp
 *
 *  Created on: Dec 17, 2015
 *      Author: pete
 */

#include "breathing.h"

HSVHue ColorWheel[] = {
    HUE_ORANGE,
    HUE_PURPLE,
    HUE_GREEN,
    HUE_BLUE
};

Breathing::Breathing(CRGB *s, CRGBPalette16 pal)
{
	m_CurrentPalette = pal;
    strip = s;
    m_x = 0;
    
    QSettings settings(QStandardPaths::standardLocations(QStandardPaths::ConfigLocation).first() + "/officelights/officelights.conf", QSettings::IniFormat);
    m_numLeds = settings.value("NumLeds", 75).toInt();
    m_numColors = sizeof(ColorWheel);
    m_currentHue = ColorWheel[std::experimental::randint(0, m_numColors)];
}

Breathing::~Breathing()
{
}

void Breathing::startup()
{
    qDebug() << __PRETTY_FUNCTION__;
    fill_solid((CRGB*)strip, m_numLeds, CHSV(m_currentHue, 244, value(215)));
    FastLED.setDither(0);
    FastLED.setTemperature(Candle);
}

int Breathing::value(int x) 
{
    return (-210*abs(sin(x*0.01)))+255;
}

void Breathing::action()
{
    int v = 0;

    v = value(m_x);
    if (std::experimental::randint(0, 300) == 28) {
        m_currentHue = ColorWheel[std::experimental::randint(0, m_numColors)];
    }
    fill_solid((CRGB*)strip, m_numLeds, CHSV(m_currentHue, 244, v));
    FastLED.show();
    // 254 seems to be max value
    if (v == 254) {
        QThread::sleep(3);
    }
    else {
        QThread::msleep(30);
    }

    m_x++;
}

