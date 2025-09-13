#include "bitmap.h"
#include <iostream>
#include <filesystem>

using namespace std;

int main()
{
    filesystem::path output_dir = std::filesystem::current_path().append("assets");
    // Check for assets folder
    // If it doesn't exist, create it
    cout << output_dir << endl;
    // Attempt to create the directory
    if (!std::filesystem::is_directory(output_dir))
    {
        cout << "assets directory doesn't exist." << endl;
        if (std::filesystem::create_directory(output_dir))
        {
            std::cout << "Directory created successfully." << std::endl;
        }
        else
        {
            std::cout << "Directory creation failed." << std::endl;
            return 1;
        }
    }
    
    BMP map(10, 10, 1);
    map.AddColor(0, 0, 0);
    map.AddColor(255, 255, 255);
    for (int i = 0; i < 10; i++)
        for (int j = 0; j < 10; j++)
        {
            map.SetPixel(i, j, 0);
        }
    map.SetPixel(1, 0, 1);

    map.ToFile(output_dir.append("test.bmp"));

    return 0;
}