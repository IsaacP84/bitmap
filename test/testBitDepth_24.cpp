#include "test.h"

class BitmapTest : public testing::Test
{
protected:
    BitmapTest()
    {
        // Check for assets folder
        // If it doesn't exist, create it
        // Attempt to create the directory
        if (!std::filesystem::is_directory(output_dir))
        {
            if (std::filesystem::create_directory(output_dir))
            {
                // std::cout << "Directory created successfully." << std::endl;
            }
            else
            {
                throw std::runtime_error("Directory creation failed.");
            }
        }
    }
    ~BitmapTest() override = default;
    const std::filesystem::path output_dir = std::filesystem::current_path().append("assets");
    uint8_t fileIncrementor = 0;

    
    BMP m0_ = BMP(10, 10, 24);
    BMP m1_ = BMP(100, 100, 24);
    BMP m2_ = BMP(1000, 1000, 24);
};

TEST_F(BitmapTest, Initialization)
{
    BMP map(10, 10, 24);
}

TEST_F(BitmapTest, WriteToFileSmall)
{
    for (int i = 0; i < 10; i++)
        for (int j = 0; j < 10; j++)
        {
            m0_.SetPixel(i, j, Color(255, 255, 255));
        }
    m0_.SetPixel(1, 0, Color(0, 0, 0));

    std::string file_name = "bitDepth_24_test";
    file_name += (fileIncrementor++);
    file_name += ".bmp";
    m0_.ToFile(std::filesystem::path(output_dir).append(file_name));
}

TEST_F(BitmapTest, WriteToFileMedium)
{
    for (int i = 0; i < 100; i++)
        for (int j = 0; j < 100; j++)
        {
            m1_.SetPixel(i, j, Color(255, 255, 255));
        }
    m1_.SetPixel(1, 0, Color(0, 0, 0));

    std::string file_name = "bitDepth_24_test";
    file_name += (fileIncrementor++);
    file_name += ".bmp";
    m1_.ToFile(std::filesystem::path(output_dir).append(file_name));
}

TEST_F(BitmapTest, WriteToFileLarge)
{
    for (int i = 0; i < 1000; i++)
        for (int j = 0; j < 1000; j++)
        {
            m2_.SetPixel(i, j, Color(255, 255, 255));
        }
    m2_.SetPixel(1, 0, Color(0, 0, 0));

    std::string file_name = "bitDepth_24_test";
    file_name += (fileIncrementor++);
    file_name += ".bmp";
    m2_.ToFile(std::filesystem::path(output_dir).append(file_name));
}