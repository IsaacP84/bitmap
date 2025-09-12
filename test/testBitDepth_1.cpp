#include "test.h"

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