#include "bitmap.h"

#include <cstring>
#include <iostream>
#include <fstream>
#include <vector>
#include <exception>
#include <filesystem>

using namespace std;

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

// BMP
BMP::BMP(int32_t w, int32_t h, uint16_t bitDepth)
    : kWidth(w), kHeight(h), kBitDepth(bitDepth)
{
    switch (kBitDepth)
    {
    case 1:
    case 2:
    case 4:
    case 8:
        colors_ = ColorMap(kBitDepth);
        data_ = malloc(kWidth * kHeight * sizeof(uint8_t));
        memset(data_, 0, kWidth * kHeight * sizeof(uint8_t));

        break;
    case 24:
        data_ = malloc(kWidth * kHeight * sizeof(Color));
        memset(data_, 0, kWidth * kHeight * sizeof(Color));
        break;
    case 32:
        data_ = malloc(kWidth * kHeight * sizeof(ColorRGBA));
        memset(data_, 0, kWidth * kHeight * sizeof(ColorRGBA));
        break;

    default:
        throw invalid_argument("Bit Depth not handled.");
    }

    if (data_ == NULL)
        throw runtime_error("Failed to allocate memory for data");

    // no type checking for high bitdepths
}

BMP::~BMP()
{
    free(data_);
}

Color BMP::get_color(uint8_t index)
{
    return colors_[index];
}

void BMP::set_color(uint8_t index, Color c)
{
    colors_.setColor(index, ColorRGBA(c.r, c.g, c.b));
}

void BMP::set_color(uint8_t index, ColorRGBA c)
{
    colors_.setColor(index, c);
}

void BMP::AddColor(Color c)
{
    colors_.addColor(c.r, c.g, c.b);
}

void BMP::AddColor(ColorRGBA c)
{
    colors_.addColor(c.r, c.g, c.b, c.a);
}

void BMP::AddColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    colors_.addColor(r, g, b, a);
}

void BMP::SetPixel(int32_t x, int32_t y, uint8_t value)
{
    if (colors_.length() == 0)
        throw invalid_argument("No color palette.");

    if (colors_.length() < value)
        throw range_error("Color index out of range");

    if (kBitDepth > 8)
        throw runtime_error("Can't assign by index");

    // Copy the element into the array
    memcpy((uint8_t *)data_ + ((x * kWidth + y) * sizeof(uint8_t)), &value, sizeof(uint8_t));
}

void BMP::SetPixel(int32_t x, int32_t y, const Color c)
{
    if (kBitDepth != 24)
        throw invalid_argument("Don't use a Color object. Use an index");

    // Copy the element into the array
    memcpy((char *)data_ + ((x * kWidth + y) * sizeof(Color)), &c, sizeof(Color));
}

void BMP::ToFile(std::filesystem::path file_name, bool silent)
{
    std::ofstream f;

    f.open(file_name.string(), std::ios::out | std::ios::binary);
    if (!f.is_open())
        throw runtime_error("Failed to open file: " + file_name.string());

    const int file_header_size = 14;

    // BITMAPINFOHEADER
    const int info_header_size = 40;

    // store in bytes
    const int row_size = (kWidth * kBitDepth + 31) / 32 * 4;
    uint32_t raw_data_size = row_size * kHeight;

    const int padding_start_bit = ((row_size - 4) * 8) + (kWidth * kBitDepth % 32);

    const uint32_t file_size = file_header_size + info_header_size + colors_.length() * 4 + raw_data_size;
    const uint32_t pixel_data_offset = file_header_size + info_header_size + colors_.length() * 4;

    unsigned char *file_header = new unsigned char[file_header_size];

    // File type
    file_header[0] = 'B';
    file_header[1] = 'M';
    // File Size in bytes
    file_header[2] = file_size;
    file_header[3] = file_size >> 8;
    file_header[4] = file_size >> 16;
    file_header[5] = file_size >> 24;
    // Reserved 1
    file_header[6] = 0;
    file_header[7] = 0;
    // Reserved 2
    file_header[8] = 0;
    file_header[9] = 0;
    // Pixel Data Offset
    file_header[10] = pixel_data_offset;
    file_header[11] = pixel_data_offset >> 8;
    file_header[12] = pixel_data_offset >> 16;
    file_header[13] = pixel_data_offset >> 24;

    f.write(reinterpret_cast<char *>(file_header), file_header_size);
    delete[] file_header;

    uint8_t *information_header = new unsigned char[info_header_size];

    // Size of header (uint32_t)
    information_header[0] = info_header_size;
    information_header[1] = 0;
    information_header[2] = 0;
    information_header[3] = 0;
    // Width in pixels (int32_t)
    information_header[4] = kWidth;
    information_header[5] = kWidth >> 8;
    information_header[6] = kWidth >> 16;
    information_header[7] = kWidth >> 24;
    // Height in pixels (int32_t)
    information_header[8] = kHeight;
    information_header[9] = kHeight >> 8;
    information_header[10] = kHeight >> 16;
    information_header[11] = kHeight >> 24;
    // # of Color planes (uint16_t)
    information_header[12] = 1;
    information_header[13] = 0;
    // # of Bits Per Pixel (uint16_t)
    information_header[14] = kBitDepth;
    information_header[15] = kBitDepth << 8;
    // Compression Method (uint32_t)
    information_header[16] = 0;
    information_header[17] = 0;
    information_header[18] = 0;
    information_header[19] = 0;
    // Image Size (uint32_t)
    information_header[20] = raw_data_size;
    information_header[21] = raw_data_size >> 8;
    information_header[22] = raw_data_size >> 16;
    information_header[23] = raw_data_size >> 24;
    // Horizontal Resolution (int32_t)
    information_header[24] = 0;
    information_header[25] = 0;
    information_header[26] = 0;
    information_header[27] = 0;
    // Vertical Resolution (int32_t)
    information_header[28] = 0;
    information_header[29] = 0;
    information_header[30] = 0;
    information_header[31] = 0;
    // # of Colors in Color Palette (uint32_t)
    information_header[32] = colors_.length();
    information_header[33] = colors_.length() >> 8;
    information_header[34] = colors_.length() >> 16;
    information_header[35] = colors_.length() >> 24;
    // # of Important Colors used (uint32_t)
    information_header[36] = 0;
    information_header[37] = 0;
    information_header[38] = 0;
    information_header[39] = 0;

    f.write(reinterpret_cast<char *>(information_header), info_header_size);
    delete[] information_header;

    if (colors_)
    {
        for (unsigned int i = 0; i < colors_.length(); i++)
        {
            f.write(reinterpret_cast<char *>(&colors_[i]), sizeof(ColorRGBA));
        }
    }

    // should theoretically be faster because this is on the stack
    uint8_t row[row_size];

    for (int32_t j = 0; j < kHeight; j++)
    {
        // truncates
        // clear bytes from memory to be reused
        for (int i = 0; i < row_size; i++)
            row[i] = 0x00;
        switch (kBitDepth)
        {
        case 1:
        case 2:
        case 4:
        case 8:
        {
            for (int32_t i = 0; i < kWidth; i++)
            {
                uint8_t bits = row[(i * kBitDepth) / 8];

                if ((i * kBitDepth) < padding_start_bit)
                {

                    // black magic fuckery
                    uint8_t data = *((uint8_t *)data_ + (i * kWidth + j));

                    int offset = (8 - kBitDepth) - (i * kBitDepth % 8);
                    bits |= (data << offset);
                }
                else
                {
                    bits <<= kBitDepth;
                }
                row[(i * kBitDepth) / 8] = bits;
            }
            break;
        }
        case 24:
        {
            for (uint32_t i = 0; i < kWidth; i++)
            {
                const Color *color = ((Color *)data_ + i * kWidth + j);
                memcpy(&row[i * 3], &color->b, sizeof(uint8_t));
                memcpy(&row[i * 3 + 1], &color->g, sizeof(uint8_t));
                memcpy(&row[i * 3 + 2], &color->r, sizeof(uint8_t));
            }
            break;
        }
        case 32:
        {
            throw runtime_error("Not implemented yet");
            break;
        }
        }
        f.write(reinterpret_cast<char *>(row), row_size);
    }

    f.close();
    cout << "File created: " + file_name.string() << endl;
    cout << "File Size: " << file_size << endl;
}

void BMP::Print()
{
    switch (kBitDepth)
    {
    case 1:
    case 2:
    case 4:
    case 8:
    {
        for (int32_t i = 0; i < kWidth; i++)
        {
            for (int32_t j = 0; j < kHeight; j++)
            {
                // black magic
                cout << (int)((char *)data_)[i * kWidth + j] << " ";
            }
            cout << endl;
        }
        break;
    }
    case 24:
        // cout << ((Color *)data)->r << endl;
        // for (unsigned int i = 0; i < width; i++)
        // {
        //     for (unsigned int j = 0; j < height; j++)
        //     {
        //         cout << ((Color *)(data + (i * width + j)))->r
        //              << ((Color *)(data + (i * width + j)))->g
        //              << ((Color *)(data + (i * width + j)))->b;
        //     }
        //     cout << endl;
        // }
        throw runtime_error("Bit Depth not handled.");
        break;
    case 32:
        throw runtime_error("Bit Depth not handled.");
        break;

    default:
        throw runtime_error("Bit Depth not handled.");
    }
}
