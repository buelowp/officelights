#include "christmastree.h"

static CRGB ChristmasColorWheel[] = {
		CRGB::Red,
		CRGB::Yellow,
		CRGB::Blue,
		CRGB::Green,
		CRGB::Purple,
        CRGB::Cyan,
        CRGB::Magenta,
};

ChristmasTree::ChristmasTree(CRGB *s)
{
    strip = s;
    QSettings settings(QStandardPaths::standardLocations(QStandardPaths::ConfigLocation).first() + "/officelights/officelights.conf", QSettings::IniFormat);
    m_delay = settings.value("ChristmasTreeDelay", 250).toInt();
    m_numLeds = settings.value("NumLeds", 75).toInt();
}

ChristmasTree::~ChristmasTree()
{
}

int ChristmasTree::randomValue(int low, int high)
{
    return qrand() % ((high - low) + 1) + low; 
}

void ChristmasTree::startup()
{
    int rval = 0;
    for (int i = 0; i < m_numLeds; i++) {
        rval = randomValue(0, 6);
        strip[i] = ChristmasColorWheel[rval];
    }
}

void ChristmasTree::action()
{
    EVERY_N_MILLISECONDS(m_delay) {
        strip[randomValue(0, (m_numLeds - 1))] = ChristmasColorWheel[randomValue(0, 6)];
    }
}
