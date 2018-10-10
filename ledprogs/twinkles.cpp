/*
 * Twinkles.cpp
 *
 *  Created on: Dec 17, 2015
 *      Author: pete
 */

#include "twinkles.h"

Twinkles::Twinkles(CRGB *s, CRGBPalette16 pal)
{
	gCurrentPalette = pal;
	gBackgroundColor = CRGB(16,14,4);
    strip = s;
    
    QSettings settings(QStandardPaths::standardLocations(QStandardPaths::ConfigLocation).first() + "/officelights/officelights.conf", QSettings::IniFormat);
    m_numLeds = settings.value("NumLeds", 75).toInt();
    m_density = settings.value("TwinkleDensity", 8).toInt();
    m_speed = settings.value("TwinkleSpeed", 5).toInt();
}

Twinkles::~Twinkles()
{
}

void Twinkles::startup()
{
}

void Twinkles::setBackgroundColor(CRGB c)
{
    qDebug() << __PRETTY_FUNCTION__ << ": setting background color to" << c;
	gBackgroundColor = c;
}

void Twinkles::setDensity(int d)
{
    qDebug() << __PRETTY_FUNCTION__ << ": Setting particle density to" << d;
	if (d <= 8)
		m_density = d;
}

void Twinkles::setSpeed(int s)
{
    qDebug() << __PRETTY_FUNCTION__ << ": setting twinkle speed to" << s;
	if (s <= 8)
		m_speed = s;
}

//  This function loops over each pixel, calculates the
//  adjusted 'clock' that this pixel should use, and calls
//  "CalculateOneTwinkle" on each pixel.  It then displays
//  either the twinkle color of the background color,
//  whichever is brighter.
void Twinkles::action()
{
	  // "PRNG16" is the pseudorandom number generator
	  // It MUST be reset to the same starting value each time
	  // this function is called, so that the sequence of 'random'
	  // numbers that it generates is (paradoxically) stable.
	uint16_t PRNG16 = 11337;
	uint32_t clock32 = millis();

	uint8_t backgroundBrightness = gBackgroundColor.getAverageLight();

    for ( uint16_t i = 0; i < m_numLeds; i++) {
        PRNG16 = (uint16_t)(PRNG16 * 2053) + 1384; // next 'random' number
        uint16_t myclockoffset16= PRNG16; // use that number as clock offset
        PRNG16 = (uint16_t)(PRNG16 * 2053) + 1384; // next 'random' number
        // use that number as clock speed adjustment factor (in 8ths, from 8/8ths to 23/8ths)
        uint8_t myspeedmultiplierQ5_3 =  ((((PRNG16 & 0xFF)>>4) + (PRNG16 & 0x0F)) & 0x0F) + 0x08;
        uint32_t myclock30 = (uint32_t)((clock32 * myspeedmultiplierQ5_3) >> 3) + myclockoffset16;
        uint8_t  myunique8 = PRNG16 >> 8; // get 'salt' value for this pixel

        // We now have the adjusted 'clock' for this pixel, now we call
        // the function that computes what color the pixel should be based
        // on the "brightness = f( time )" idea.
        CRGB c = computeOneTwinkle( myclock30, myunique8);
        uint8_t cbright = c.getAverageLight();
        int16_t deltabright = cbright - backgroundBrightness;
        
        if (deltabright >= 32 || (!gBackgroundColor)) {
                // If the new pixel is significantly brighter than the background color,
                // use the new color.
            strip[i] = c;
        }
        else if ( deltabright > 0 ) {
                // If the new pixel is just slightly brighter than the background color,
                // mix a blend of the new color and the background color
            strip[i] = blend( gBackgroundColor, c, deltabright * 8);
        }
        else {
                // if the new pixel is not at all brighter than the background color,
                // just use the background color.
            strip[i] = gBackgroundColor;
        }
	}
}

// This function is like 'triwave8',
// (which produces a
// symmetrical up-and-down triangle
// sawtooth waveform), except that this
// function produces a triangle wave with
// a faster attack and a slower decay:
//
uint8_t Twinkles::attackDecayWave8( uint8_t i)
{
	if (i < 86) {
		return (i * 3);
	} else {
		i -= 86;
		return (255 - (i + (i/2)));
	}
}

//  This function takes a time in pseudo-milliseconds,
//  figures out brightness = f( time ), and also hue = f( time )
//  The 'low digits' of the millisecond time are used as
//  input to the brightness wave function.
//  The 'high digits' are used to select a color, so that the color
//  does not change over the course of the fade-in, fade-out
//  of one cycle of the brightness wave function.
//  The 'high digits' are also used to determine whether this pixel
//  should light at all during this cycle, based on the TWINKLE_DENSITY.
CRGB Twinkles::computeOneTwinkle( uint32_t ms, uint8_t salt)
{
	uint16_t ticks = ms >> (8-m_speed);
	uint8_t fastcycle8 = ticks;
	uint8_t slowcycle8 = (ticks >> 8) ^ salt;

	uint8_t bright = 0;
	if( (slowcycle8 & 0x0E) < m_density) {
		bright = attackDecayWave8(fastcycle8);
	}

	uint8_t hue = (slowcycle8 * 16) + salt;
	return ColorFromPalette(gCurrentPalette, hue, bright, NOBLEND);
}
