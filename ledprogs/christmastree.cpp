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
    for (int i = 0; i < NUM_LEDS; i++) {
        rval = randomValue(0, 6);
        strip[i] = ChristmasColorWheel[rval];
    }
}

void ChristmasTree::action()
{
    EVERY_N_MILLISECONDS(250) {
        strip[randomValue(0, 75)] = ChristmasColorWheel[randomValue(0, 6)];
    }
}
