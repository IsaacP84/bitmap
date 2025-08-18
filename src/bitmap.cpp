#include "bitmap.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <exception>
extern "C"
{
    using namespace std;

    Color::Color(float r, float g, float b, float a)
    {
        this->r = r;
        this->g = g;
        this->b = b;
        this->a = a;
    }

    std::ostream &operator<<(std::ostream &out, Color &data)
    {
        out << '(' << data.r << ',' << data.g << ',' << data.b << ',' << data.a << ')';
        return out;
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

    ColorMap::ColorMap(unsigned int bitDepth, std::vector<Color> colors)
    {
        MAXIMUM_COLORS = 2 << bitDepth;
        colors.reserve(MAXIMUM_COLORS);
    }

    Color &ColorMap::addColor(float r, float g, float b, float a)
    {
        if (colors.size() + 1 > MAXIMUM_COLORS)
        {
            throw std::length_error("Maximum Colors for this bit depth has been reached.");
        }

        colors.push_back(Color(r, g, b, a));
        return colors.back();
    }

    Color &ColorMap::getColor(unsigned int index)
    {
        return colors[index];
    }

    auto ColorMap::length()
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

    BMP::BMP(unsigned int width, unsigned int height, unsigned int bitDepth)
    {
        data = new int *[width];
        for (unsigned int i = 0; i < width; i++)
        {
            data[i] = new int[height];
            for (unsigned int j = 0; j < height; j++)
            {
                data[i][j] = 0;
            }
        }

        this->width = width;
        this->height = height;

        this->bitDepth = bitDepth;

        if (bitDepth <= 8)
        {
            colors = ColorMap(bitDepth);
        }
    }

    BMP::~BMP()
    {
        for (unsigned int i = 0; i < width; i++)
        {
            delete[] data[i];
        }
        delete[] data;
    }

    Color &BMP::addColor(float r, float g, float b, float a)
    {
        return colors.addColor(r, g, b, a);
    }

    Color &BMP::getColor(unsigned int index)
    {
        return colors[index];
    }

    void BMP::toFile(string fileName, bool silent)
    {
        if (!silent)
            cout << "\33[2K\r"
                 << "Creating File";

        // A link the guide I'm using for help
        // https://www.youtube.com/watch?v=vqT5j38bWGg
        // cout << sizeof(int) << endl;
        std::ofstream f;
        f.open(fileName + ".bmp", std::ios::out | std::ios::binary);
        if (!f)
            cout << "\33[2K\r"
                 << "Failed to open file!"
                 << endl;

        // Padding
        int paddingSize = 4 - (width * (bitDepth / 8) % 4);

        const int fileHeaderSize = 14;
        // BITMAPINFOHEADER
        const int informationHeaderSize = 40;
        int colorMapSize = 0;
        const int rowSizeBytes = (width * bitDepth + 31) / 32 * 4;
        const unsigned int rawDataSize = rowSizeBytes * height;

        if (colors)
        {
            colorMapSize = colors.length() * 4;
        }

        int paddingStartBit = ((rowSizeBytes - 4) * 8) + (width * bitDepth % 32);
        // int paddingSizeBits = 32 - (width * bitDepth % 32);

        const unsigned int fileSize = fileHeaderSize + informationHeaderSize + colorMapSize + rawDataSize;

        unsigned int pixelDataOffset = fileHeaderSize + informationHeaderSize + colorMapSize;

        unsigned char *fileHeader = new unsigned char[fileHeaderSize];
        // File type
        fileHeader[0] = 'B';
        fileHeader[1] = 'M';
        // File Size in bytes
        fileHeader[2] = fileSize;
        fileHeader[3] = fileSize >> 8;
        fileHeader[4] = fileSize >> 16;
        fileHeader[5] = fileSize >> 24;
        // Reserved 1
        fileHeader[6] = 0;
        fileHeader[7] = 0;
        // Reserved 2
        fileHeader[8] = 0;
        fileHeader[9] = 0;
        // Pixel Data Offset
        fileHeader[10] = pixelDataOffset;
        fileHeader[11] = pixelDataOffset >> 8;
        fileHeader[12] = pixelDataOffset >> 16;
        fileHeader[13] = pixelDataOffset >> 24;

        f.write(reinterpret_cast<char *>(fileHeader), fileHeaderSize);
        delete[] fileHeader;

        unsigned char *informationHeader = new unsigned char[informationHeaderSize];

        // Size of header
        informationHeader[0] = informationHeaderSize;
        informationHeader[1] = 0;
        informationHeader[2] = 0;
        informationHeader[3] = 0;
        // Width in pixels
        informationHeader[4] = width;
        informationHeader[5] = width >> 8;
        informationHeader[6] = width >> 16;
        informationHeader[7] = width >> 24;
        // Height in pixels
        informationHeader[8] = height;
        informationHeader[9] = height >> 8;
        informationHeader[10] = height >> 16;
        informationHeader[11] = height >> 24;
        // # of Color planes
        informationHeader[12] = 1;
        informationHeader[13] = 0;
        // # of Bits Per Pixel (Set to 1, 2, 4, 8, 16, 24, or 32)
        informationHeader[14] = bitDepth;
        informationHeader[15] = 0;
        // Compression Method
        informationHeader[16] = 0;
        informationHeader[17] = 0;
        informationHeader[18] = 0;
        informationHeader[19] = 0;
        // Image Size
        informationHeader[20] = rawDataSize;
        informationHeader[21] = rawDataSize >> 8;
        informationHeader[22] = rawDataSize >> 16;
        informationHeader[23] = rawDataSize >> 24;
        // Horizontal Resolution (Signed integer)
        informationHeader[24] = 0;
        informationHeader[25] = 0;
        informationHeader[26] = 0;
        informationHeader[27] = 0;
        // Vertical Resolution (Signed integer)
        informationHeader[28] = 0;
        informationHeader[29] = 0;
        informationHeader[30] = 0;
        informationHeader[31] = 0;
        // # of Colors in Color Palette
        informationHeader[32] = colors.length();
        informationHeader[33] = colors.length() >> 8;
        informationHeader[34] = colors.length() >> 16;
        informationHeader[35] = colors.length() >> 24;
        // # of Important Colors used
        informationHeader[36] = 0;
        informationHeader[37] = 0;
        informationHeader[38] = 0;
        informationHeader[39] = 0;

        f.write(reinterpret_cast<char *>(informationHeader), informationHeaderSize);
        delete[] informationHeader;

        if (colors)
        {
            // unsigned char *colorEntry = new unsigned char[4];
            for (unsigned int i = 0; i < colors.length(); i++)
            {
                unsigned char *colorEntry = new unsigned char[4];
                // Blue
                colorEntry[0] = (unsigned char)colors[i].b;

                // cout << "b: " << static_cast<float>(colorEntry[0]) << endl;
                // Green
                colorEntry[1] = (unsigned char)colors[i].g;

                // cout << "g: " << static_cast<float>(colorEntry[1]) << endl;
                // Red
                colorEntry[2] = (unsigned char)colors[i].r;

                // cout << "r: " << static_cast<float>(colorEntry[2]) << endl;
                // Alpha
                colorEntry[3] = (unsigned char)colors[i].a;

                f.write(reinterpret_cast<char *>(colorEntry), 4);
                delete[] colorEntry;
            }

            // for (int i = 0; i < colorMapSize; i++)
            // {
            //     cout << i << " " << static_cast<float>(colorMap[i]) << endl;
            // }
            // cout << endl;
            // f.write(reinterpret_cast<char *>(colorMap), colorMapSize);
        }

        unsigned char *row = new unsigned char[rowSizeBytes];

        for (unsigned int j = 0; j < height; j++)
        {

            // for (unsigned int i = paddingStartBit; i < rowSizeBytes; i++)
            //     row[i] = 0;
            // truncates
            for (unsigned int i = 0; i < rowSizeBytes; i++)
                row[i] = 0;

            if (bitDepth <= 8)
            {
                for (unsigned int i = 0; i < width; i++)
                {
                    unsigned char bits = row[(i * bitDepth) / 8];
                    // do some checks on the data;

                    if ((i * bitDepth) < paddingStartBit)
                    {
                        // cout << 7 - (i * bitDepth % 8) << endl;
                        if (data[i][j] > colors.MAXIMUM_COLORS)
                        {
                            data[i][j] = 1;
                        }

                        // truncate then add 7
                        // start from the highest and go down
                        int offset = (8 - bitDepth) - (i * bitDepth % 8);
                        bits |= (data[i][j] << offset);
                    }
                    else
                    {
                        bits <<= bitDepth;
                    }

                    row[(i * bitDepth) / 8] = bits;
                }
            }
            else
            {
                for (unsigned int i = 0; i < width; i++)
                {
                    if (data[i][j] == 1)
                    {
                        row[i * 3] = static_cast<unsigned char>(0 * 255.0f);
                        row[i * 3 + 1] = static_cast<unsigned char>(0 * 255.0f);
                        row[i * 3 + 2] = static_cast<unsigned char>(0 * 255.0f);
                    }
                    if (data[i][j] == 0)
                    {
                        row[i * 3] = static_cast<unsigned char>(1 * 255.0f);
                        row[i * 3 + 1] = static_cast<unsigned char>(1 * 255.0f);
                        row[i * 3 + 2] = static_cast<unsigned char>(1 * 255.0f);
                    }
                }
                // cout << "after 1" << endl;
            }
            // row[0] = static_cast<unsigned char>(1 * 255.0f);
            f.write(reinterpret_cast<char *>(row), rowSizeBytes);
        }

        delete[] row;
        f.close();

        if (!silent)
        {
            cout << "\33[2K\r"
                 << "File created";
            cout << "\33[2K\r";
            cout << "File Size: " << fileSize << endl;
        }
    }

    void BMP::toConsole()
    {
        for (unsigned int i = 0; i < width; i++)
        {
            for (unsigned int j = 0; j < height; j++)
            {
                cout << data[i][j];
            }
            cout << endl;
        }
    }
}