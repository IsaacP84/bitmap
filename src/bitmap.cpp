#include "bitmap.h"

#include <cstring>
#include <iostream>
#include <fstream>
#include <vector>
#include <exception>
#include <filesystem>

using namespace std;

class Bitmap8 : public BMP
{
public:
    Bitmap8(int32_t width, int32_t height, int16_t bitDepth)
        : BMP(width, height, bitDepth)
    {
        colors_ = ColorMap(kBitDepth);
        data_ = malloc(kWidth * kHeight * sizeof(uint8_t));
        memset(data_, 0, kWidth * kHeight * sizeof(uint8_t));

        if (data_ == NULL)
            throw runtime_error("Failed to allocate memory for data");
    }
    void SetPixel(int32_t x, int32_t y, const Color) override;
    void SetPixel(int32_t x, int32_t y, uint8_t index) override;
    void WriteDataImpl(std::ofstream &f, int row_size) override;
    void Print() override;
};

void Bitmap8::SetPixel(int32_t x, int32_t y, Color c)
{
    throw invalid_argument("Don't use a Color object. Use an index");
}

void Bitmap8::SetPixel(int32_t x, int32_t y, uint8_t index)
{
    if (colors_.length() == 0)
        throw invalid_argument("No color palette.");

    if (colors_.length() < index)
        throw range_error("Color index out of range");

    // Copy the element into the array
    memcpy((uint8_t *)data_ + ((x * kWidth + y) * sizeof(uint8_t)), &index, sizeof(uint8_t));
}

void Bitmap8::WriteDataImpl(std::ofstream &f, int row_size)
{
    const int padding_start_bit = ((row_size - 4) * 8) + (kWidth * kBitDepth % 32);
    uint8_t row[row_size];

    for (int32_t j = 0; j < kHeight; j++)
    {
        // truncates
        // clear bytes from memory to be reused
        for (int i = 0; i < row_size; i++)
            row[i] = 0x00;

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
                bits <<= kBitDepth;

            row[(i * kBitDepth) / 8] = bits;
        }

        f.write(reinterpret_cast<char *>(row), row_size);
    }
}

void Bitmap8::Print()
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
}

class Bitmap24 : public BMP
{
public:
    Bitmap24(int32_t width, int32_t height, int16_t bitDepth)
        : BMP(width, height, bitDepth)
    {
        colors_ = ColorMap(kBitDepth);
        data_ = malloc(kWidth * kHeight * sizeof(Color));
        memset(data_, 0, kWidth * kHeight * sizeof(Color));

        if (data_ == NULL)
            throw runtime_error("Failed to allocate memory for data");
    }
    void SetPixel(int32_t x, int32_t y, Color) override;
    void SetPixel(int32_t x, int32_t y, uint8_t index) override;
    void WriteDataImpl(std::ofstream &f, int row_size) override;
    void Print() override;
};

void Bitmap24::SetPixel(int32_t x, int32_t y, Color c)
{
    memcpy((Color *)data_ + (x * kWidth + y), &c, sizeof(Color));
}

void Bitmap24::SetPixel(int32_t x, int32_t y, uint8_t index)
{
    throw invalid_argument("Don't use an index. Use a Color object");
}

void Bitmap24::WriteDataImpl(std::ofstream &f, int row_size)
{
    // should theoretically be faster because this is on the stack
    uint8_t row[row_size];

    for (int32_t j = 0; j < kHeight; j++)
    {
        // truncates
        // clear bytes from memory to be reused
        for (int i = 0; i < row_size; i++)
            row[i] = 0x00;

        for (uint32_t i = 0; i < kWidth; i++)
        {
            const Color *color = ((Color *)data_ + i * kWidth + j);
            memcpy(&row[i * 3], &color->b, sizeof(uint8_t));
            memcpy(&row[i * 3 + 1], &color->g, sizeof(uint8_t));
            memcpy(&row[i * 3 + 2], &color->r, sizeof(uint8_t));
        }
        f.write(reinterpret_cast<char *>(row), row_size);
    }
}

void Bitmap24::Print()
{
    throw runtime_error("Not implemented yet.");
}

std::unique_ptr<BMP>
BMP::Create(int32_t width, int32_t height, BitDepth bitDepth)
{
    // return ;
    switch (bitDepth)
    {
    case BitDepth::BIT_DEPTH_1:
    case BitDepth::BIT_DEPTH_2:
    case BitDepth::BIT_DEPTH_4:
    case BitDepth::BIT_DEPTH_8:
        return std::move(std::make_unique<Bitmap8>(width, height, bitDepth));
    case BitDepth::BIT_DEPTH_24:
        return std::move(std::make_unique<Bitmap24>(width, height, bitDepth));
    default:
        throw invalid_argument("Bit Depth not handled.");
    }
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

    WriteDataImpl(f, row_size);

    f.close();
    cout << "File created: " + file_name.string() << endl;
    cout << "File Size: " << file_size << endl;
}
