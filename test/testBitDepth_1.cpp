#include "test.h"
#include <fstream>

TEST(MatchTest, 10x10)
{
    const std::filesystem::path output_dir = std::filesystem::current_path().append("assets");
    // This code generates a copy of "test/assets/BitDepth_1_Test_10_10.bmp"
    // If the test fails this won't match
    {
        BMP map(10, 10, 1);
        map.AddColor(0, 0, 0);
        map.AddColor(255, 255, 255);
        for (int i = 0; i < 10; i++)
            for (int j = 0; j < 10; j++)
            {
                map.SetPixel(i, j, 1);
            }

        map.SetPixel(1, 0, 0);
        map.ToFile(std::filesystem::path(output_dir).append("BitDepth_1_Test_10_10_Attempt.bmp"));
    }

    // open both files and compare byte by byte
    std::cout << "Comparing files..." << std::endl;
    std::ifstream f1(std::filesystem::path(output_dir).append("BitDepth_1_Test_10_10_Attempt.bmp"), std::ios::binary);
    std::ifstream f2(std::filesystem::path(output_dir).append("BitDepth_1_Test_10_10.bmp"), std::ios::binary);

    ASSERT_TRUE(f1.is_open());
    ASSERT_TRUE(f2.is_open());

    char byte1, byte2;
    while (f1.get(byte1) && f2.get(byte2))
    {
        // std::cout << std::hex << (int)(unsigned char)byte1 << " " << std::hex << (int)(unsigned char)byte2 << std::dec << std::endl;
        ASSERT_EQ(byte1, byte2);
    }
    ASSERT_FALSE(f2.get(byte1));

    // Check if both files reached the end
    ASSERT_TRUE(f1.eof() && f2.eof());
    f1.close();
    f2.close();
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
// TEST(HelloTest, BasicAssertions)
// {
//     std::filesystem::path output_path = ".\\assets\\maze";

//     // Check for assets folder
//     // If it doesn't exist, create it
//     // Attempt to create the directory
//     if (!std::filesystem::is_directory(".\\assets"))
//     {
//         std::cout << "assets directory doesn't exist." << std::endl;
//         if (std::filesystem::create_directory(".\\assets"))
//         {
//             std::cout << "Directory created successfully." << std::endl;
//         }
//         else
//         {
//             std::cout << "Directory creation failed." << std::endl;
//             // ideally throw an error or something
//         }
//     }

//     // Check if the path exists and is a directory
//     if (std::filesystem::exists(output_path))
//     {
//         std::cout << output_path << " already exists." << std::endl;
//     }

//     BMP map(10, 10, 1);
//     for (int i = 0; i < 10; i++)
//         for (int j = 0; j < 10; j++)
//         {
//             map.setPixel(i, j, Color(255, 255, 255));
//         }
//     map.setPixel(1, 0, Color(0, 0, 0));

//     map.toFile(output_path.remove_filename().append("bitDepth_24_test.bmp").string());

//     // // Expect two strings not to be equal.
//     // EXPECT_STRNE("hello", "world");
//     // // Expect equality.
//     // EXPECT_EQ(7 * 6, 42);
// }