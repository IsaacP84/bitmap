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
extern "C"
{
    struct BITMAP_API Color
    {
        float r = 0;
        float g = 0;
        float b = 0;
        float a = 0;

        Color(float r, float g, float b, float a = 0);

        friend std::ostream;
    };

    struct BITMAP_API ColorMap
    {
        std::vector<Color> colors = std::vector<Color>();
        unsigned int bitDepth = 1;
        unsigned int MAXIMUM_COLORS;

        ColorMap();
        ColorMap(unsigned int bitDepth);
        ColorMap(unsigned int bitDepth, std::vector<Color> colors);

        // Returns the number of assigned colors
        auto length();

        // Return by reference
        Color &getColor(unsigned int index);

        // Add a color
        Color &addColor(float r, float g, float b, float a = 0);

        // is unset for the moment
        void setColor(unsigned int index, Color color);

        operator bool() const;

        Color &operator[](unsigned int index);
    };

    class BITMAP_API BMP
    {
        unsigned int width = 0;
        unsigned int height = 0;
        unsigned int bitDepth = 1;
        ColorMap colors;

    public:
        int **data;

        BMP(unsigned int width, unsigned int height, unsigned int bitDepth = 1);
        ~BMP();

        // Return by reference
        Color &getColor(unsigned int index);

        // Add a color
        Color &addColor(float r, float g, float b, float a = 0);

        void toFile(std::string fileName, bool silent = false);

        void toConsole();
    };
}