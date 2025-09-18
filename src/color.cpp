#include "bitmap.h"

Color::Color(uint8_t r, uint8_t g, uint8_t b)
{
    this->r = r;
    this->g = g;
    this->b = b;
}

ColorRGBA::ColorRGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    this->r = r;
    this->g = g;
    this->b = b;
    this->a = a;
}

// ColorMap
ColorMap::ColorMap()
{
    MAXIMUM_COLORS = 1 << bitDepth;
    colors.reserve(MAXIMUM_COLORS);
}

ColorMap::ColorMap(unsigned int bitDepth)
{
    MAXIMUM_COLORS = 1 << bitDepth;
    colors.reserve(MAXIMUM_COLORS);
}

ColorMap::ColorMap(unsigned int bitDepth, std::vector<ColorRGBA> colors)
{
    MAXIMUM_COLORS = 2 << bitDepth;
    colors.reserve(MAXIMUM_COLORS);
}

ColorRGBA &ColorMap::addColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    if (colors.size() + 1 > MAXIMUM_COLORS)
    {
        throw std::length_error("Maximum Colors for this bit depth has been reached.");
    }

    colors.push_back(ColorRGBA(r, g, b, a));
    return colors.back();
}

ColorRGBA &ColorMap::getColor(unsigned int index)
{
    return colors[index];
}

void ColorMap::setColor(unsigned int index, ColorRGBA color)
{
    colors[index] = color;
}

uint32_t ColorMap::length()
{
    return colors.size();
}

ColorMap::operator bool() const
{
    if (colors.size() > 0)
        return true;
    return false;
}

Color &ColorMap::operator[](unsigned int index)
{
    return colors[index];
}