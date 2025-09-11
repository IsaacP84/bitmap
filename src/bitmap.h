#pragma once

// For this stuff, I used the link below
// https://nullprogram.com/blog/2021/05/31/
#if defined(BUILD_DLL)
    #if defined(_WIN32)
        #define BITMAP_API __declspec(dllexport)
    #elif defined(__ELF__)
        #define BITMAP_API __attribute__((visibility("default")))
    #else
        #define BITMAP_API
    #endif
#else
    #if defined(_WIN32)
        #define BITMAP_API __declspec(dllimport)
    #else
        #define BITMAP_API
    #endif
#endif

#include <string>
#include <vector>
#include <cstdint>


extern "C"
{
    struct BITMAP_API Color
    {
        uint8_t b;
        uint8_t g;
        uint8_t r;

        Color(uint8_t r = 0, uint8_t g = 0, uint8_t b = 0);
    };

    struct BITMAP_API ColorRGBA : Color
    {
        uint8_t a;

        ColorRGBA(uint8_t r = 0, uint8_t g = 0, uint8_t b = 0, uint8_t a = 0);
    };

    struct BITMAP_API ColorMap
    {
        std::vector<ColorRGBA> colors = std::vector<ColorRGBA>();
        unsigned int bitDepth = 1;
        unsigned int MAXIMUM_COLORS;

        ColorMap();
        ColorMap(unsigned int bitDepth);
        ColorMap(unsigned int bitDepth, std::vector<ColorRGBA> colors);

        // Returns the number of assigned colors
        auto length();

        // Return by reference
        ColorRGBA &getColor(unsigned int index);

        // Add a color
        ColorRGBA &addColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 0);

        // is unset for the moment
        void setColor(unsigned int index, ColorRGBA color);

        operator bool() const;

        Color &operator[](unsigned int index);
    };

    class BITMAP_API BMP
    {
        const uint32_t width;
        const uint32_t height;
        const uint8_t bitDepth;
        ColorMap colors;
        uint8_t sizePixelBits;

    public:
        void *data;

        BMP(uint32_t width, uint32_t height, uint8_t bitDepth = 1);
        ~BMP();

        // Return by reference
        Color &getColor(unsigned int index);

        // Add a color
        Color &addColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 0);

        void toFile(std::string fileName, bool silent = false);

        void toConsole();

        // setters
        void setPixel(uint32_t x, uint32_t y, Color c);
        void setPixel(uint32_t x, uint32_t y, uint8_t value);

        // getters
    };
}