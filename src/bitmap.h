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
#include <filesystem>

extern "C"
{
    enum BitDepth
    {
        BIT_DEPTH_1 = 1,
        BIT_DEPTH_2 = 2,
        BIT_DEPTH_4 = 4,
        BIT_DEPTH_8 = 8,
        // BIT_DEPTH_16 = 16,
        BIT_DEPTH_24 = 24,
        BIT_DEPTH_32 = 32
    };
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
        uint32_t length();

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
    public:
        static std::unique_ptr<BMP> Create(int32_t width, int32_t height, BitDepth bitDepth);

        const int32_t kWidth;
        const int32_t kHeight;
        const int16_t kBitDepth;

        virtual void SetPixel(int32_t x, int32_t y, const Color) = 0;
        virtual void SetPixel(int32_t x, int32_t y, uint8_t index) = 0;
        virtual void Print() = 0;

        void ToFile(std::filesystem::path, bool silent = false);

        // ColorMap Abstraction
        Color get_color(uint8_t index);
        void set_color(uint8_t index, Color);
        void set_color(uint8_t index, ColorRGBA);
        void AddColor(Color);
        void AddColor(ColorRGBA);
        void AddColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 0);

        // setters
        // getters

        virtual ~BMP() { free(data_); }

    protected:
        BMP(int32_t width, int32_t height, int16_t bitDepth) : kWidth(width), kHeight(height), kBitDepth(bitDepth) {};
        virtual void WriteDataImpl(std::ofstream &f, int row_size) = 0;
        void *data_;
        ColorMap colors_;
    };
}