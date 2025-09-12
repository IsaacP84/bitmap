#include "bitmap.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <exception>

#include <filesystem>

extern "C"
{
    using namespace std;

    struct InfoHeader
    {
        uint32_t biSize;
        int32_t biWidth;
        int32_t biHeight;
        uint16_t biPlanes;
        uint16_t biBitCount; // This defines the bit depth
        uint32_t biCompression;
        uint32_t biSizeImage;
        int32_t biXPelsPerMeter;
        int32_t biYPelsPerMeter;
        uint32_t biClrUsed;
        uint32_t biClrImportant;
    };

    Color::Color(uint8_t r, uint8_t g, uint8_t b)
    {
        this->r = r;
        this->g = g;
        this->b = b;
    }

    // std::ostream &operator<<(std::ostream &os, const Color &data)
    // {
    //     os << '(' << data.r << ',' << data.g << ',' << data.b << ')';
    //     return os;
    // }

    ColorRGBA::ColorRGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        this->r = r;
        this->g = g;
        this->b = b;
        this->a = a;
    }

    // std::ostream &operator<<(std::ostream &out, const ColorRGBA &data)
    // {
    //     // out << static_cast<const Color &>(data);
    //     out << '(' << data.r << ',' << data.g << ',' << data.b << ',' << data.a << ')';
    //     return out;
    // }

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

    BMP::BMP(uint32_t w, uint32_t h, uint8_t bitDepth)
        : width(w), height(h), bitDepth(bitDepth)
    {
        switch (bitDepth)
        {
        case 1:
        case 2:
        case 4:
        case 8:
            colors = ColorMap(bitDepth);
            data = malloc(width * height * sizeof(uint8_t));
            break;
        case 24:
            data = malloc(width * height * sizeof(Color));
            break;
        case 32:
            data = malloc(width * height * sizeof(Color));
            break;

        default:
            throw "Bit Depth not handled.";
        }

        if (data == NULL)
        {
            perror("Failed to allocate memory for data");
            exit(EXIT_FAILURE);
        }

        // no type checking for high bitdepths
    }

    BMP::~BMP()
    {
        free(data);
        data = NULL;
    }

    Color &BMP::addColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        return colors.addColor(r, g, b, a);
    }

    Color &BMP::getColor(unsigned int index)
    {
        return colors[index];
    }

    void BMP::setPixel(uint32_t x, uint32_t y, uint8_t value)
    {
        if (colors.length() == 0)
        {
            __throw_invalid_argument("No color palette.");
        }

        if (bitDepth > 8)
        {
            __throw_invalid_argument("Cant assign by index");
        }
        // Copy the element into the array
        memcpy((char *)data + ((x * width + y) * sizeof(uint8_t)), &value, sizeof(uint8_t));
    }

    void BMP::setPixel(uint32_t x, uint32_t y, const Color c)
    {
        if (bitDepth != 24)
        {
            __throw_invalid_argument("Don't use a Color object. Use an index");
        }
        // Copy the element into the array
        memcpy((char *)data + ((x * width + y) * sizeof(Color)), &c, sizeof(Color));
    }

    void BMP::toFile(std::filesystem::path fileName, bool silent)
    {
        // A link the guide I'm using for help
        // https://www.youtube.com/watch?v=vqT5j38bWGg
        // cout << sizeof(int) << endl;
        std::ofstream f;

        f.open(fileName.string(), std::ios::out | std::ios::binary);
        if (!f)
            runtime_error("Failed to open file.");

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
            for (unsigned int i = 0; i < colors.length(); i++)
            {
                f.write(reinterpret_cast<char *>(&colors[i]), sizeof(ColorRGBA));
            }
        }

        // unsigned char *row = new unsigned char[rowSizeBytes];
        // should theoretically be faster because this is on the stack
        uint8_t row[rowSizeBytes];

        for (unsigned int j = 0; j < height; j++)
        {

            // for (unsigned int i = paddingStartBit; i < rowSizeBytes; i++)
            //     row[i] = 0;
            // truncates
            // clear bytes from memory to be reused
            for (unsigned int i = 0; i < rowSizeBytes; i++)
                row[i] = 0;

            if (bitDepth <= 8)
            {
                for (unsigned int i = 0; i < width; i++)
                {
                    uint8_t bits = row[(i * bitDepth) / 8];
                    // do some checks on the data;

                    if ((i * bitDepth) < paddingStartBit)
                    {

                        // uint8_t dataCut = *(int*)data[i * width + j];
                        // black magic fuckery
                        uint8_t dataCut = *((uint8_t *)data + (i * width + j) * sizeof(uint8_t));

                        // cout << 7 - (i * bitDepth % 8) << endl;
                        // have an unsafe check
                        if (dataCut > colors.MAXIMUM_COLORS)
                        {
                            dataCut = -1;
                        }

                        // truncate then add 7
                        // start from the highest and go down
                        int offset = (8 - bitDepth) - (i * bitDepth % 8);
                        bits |= (dataCut << offset);
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
                for (uint32_t i = 0; i < width; i++)
                {
                    // Color cl = Color(rand() % 255, rand() % 255, rand() % 255);
                    // Color cl = Color(255, 255, 255);

                    // Color *dataCut = &cl;
                    const Color *dataCut = ((Color *)data + i * width + j);
                    memcpy(&row[i * 3], &dataCut->b, sizeof(uint8_t));
                    memcpy(&row[i * 3 + 1], &dataCut->g, sizeof(uint8_t));
                    memcpy(&row[i * 3 + 2], &dataCut->r, sizeof(uint8_t));
                }
                // cout << "after 1" << endl;
            }
            // row[0] = static_cast<unsigned char>(1 * 255.0f);
            f.write(reinterpret_cast<char *>(row), rowSizeBytes);
        }
        f.close();
        cout << "File created" << endl;
        cout << "File Size: " << fileSize << endl;
    }

    void BMP::toConsole()
    {
        switch (bitDepth)
        {
        case 1:
        case 2:
        case 4:
        case 8:
            for (unsigned int i = 0; i < width; i++)
            {
                for (unsigned int j = 0; j < height; j++)
                {
                    cout << *(uint8_t *)(data + (i * width + j));
                }
                cout << endl;
            }
            break;
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

            break;
        case 32:

            break;

        default:
            throw "Bit Depth not handled.";
        }
    }
}