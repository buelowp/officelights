/*
 * Copyright (c) 2018 <copyright holder> <email>
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#include "theaterchase.h"

static CRGB tcThanksgivingColors[] = {
    CRGB::Red,
    CRGB::Yellow,
    CRGB::Blue,
    CRGB::Green,
};

static CRGB tcHalloweenColors[] = {
    CRGB::Red,
    CRGB::Orange,
    CRGB::Blue,
    CRGB::Green,
    CRGB::Violet,
};

TheaterChase::TheaterChase(CRGB *strip, int count, int choice)
{
    if (strip)
        m_strip = strip;
    
    m_numLeds = count;
    m_color = choice;
    m_colorIndex = 0;
    m_index = 0;
}

TheaterChase::~TheaterChase()
{
}

CRGB TheaterChase::getNextColor()
{
    CRGB c;
    
    switch (m_color) {
    case TC_HALLOWEEN:
        c = tcHalloweenColors[m_colorIndex++];
        if (m_colorIndex == 5)
            m_colorIndex = 0;
        break;
    case TC_THANKSGIVING:
        c = tcThanksgivingColors[m_colorIndex++];
        if (m_colorIndex == 4)
            m_colorIndex = 0;
        break;
    default:
        c = CRGB::White;
        break;
    }
    
    return c;
}

void TheaterChase::action()
{
    CRGB c = getNextColor();
    int q = m_index - 1;
    
    if (!m_strip)
        return;
    
    if (m_index == 3)
        m_index = 0;

    if (q == -1)
        q = 2;
    
    for (int i = 0; i < m_numLeds; i = i + 3) {
        m_strip[i + m_index] = c;    //turn every third pixel on
    }
    for (int i = 0; i < m_numLeds; i = i + 3) {
        m_strip[i + q] = CRGB::Black;        //turn every third pixel off
    }
    m_index++;
}
