#ifndef __CHRISTMASTREE_H__
#define __CHRISTMASTREE_H__

#include <QtCore/QtCore>
#include <FastLED/FastLED.h>

#define NUM_LEDS            75

class ChristmasTree {
public:
    ChristmasTree(CRGB*);
    ~ChristmasTree();
    
    void startup();
    void action();
    
private:
    int randomValue(int, int);
    
    CRGB *strip;
    int m_delay;
    int m_numLeds;
};

#endif
