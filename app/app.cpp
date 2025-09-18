#include <iostream>
#include <filesystem>

#include "bitmap.h"


using namespace std;

int main()
{
    const filesystem::path output_dir = std::filesystem::current_path().append("assets");
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

    {
        std::unique_ptr<BMP> map_ptr = BMP::Create(10, 10, BitDepth::BIT_DEPTH_1);
        BMP &map = *map_ptr;
        map.AddColor(0, 0, 0);
        map.AddColor(255, 255, 255);
        for (int i = 0; i < 10; i++)
            for (int j = 0; j < 10; j++)
            {
                map.SetPixel(i, j, 1);
            }
            
        map.SetPixel(1, 0, 0);
        map.Print();
        map.hidden_text("hidden_key");
        map.ToFile(std::filesystem::path(output_dir).append("test1.bmp"));
    }

    {
        std::unique_ptr<BMP> map_ptr = BMP::Create(10, 10, BitDepth::BIT_DEPTH_4);
        BMP &map = *map_ptr;
        map.AddColor(0, 0, 0);
        map.AddColor(255, 255, 255);
        for (int i = 0; i < 10; i++)
            for (int j = 0; j < 10; j++)
            {
                map.SetPixel(i, j, 1);
            }
            
        map.SetPixel(1, 0, 0);
        map.Print();

        map.hidden_text("This is a hidden message!");
        map.ToFile(std::filesystem::path(output_dir).append("test2.bmp"));
    }

    {
        std::unique_ptr<BMP> map_ptr = BMP::Create(10, 10, BitDepth::BIT_DEPTH_24);
        BMP &map = *map_ptr;
        // map.AddColor(0, 0, 0);
        // map.AddColor(255, 255, 255);
        for (int i = 0; i < 10; i++)
            for (int j = 0; j < 10; j++)
            {
                map.SetPixel(i, j, {255, 0, 0});
            }
            
        map.SetPixel(0, 0, {0, 0, 0});
        // map.Print();
        map.hidden_text("hidden");
        map.ToFile(std::filesystem::path(output_dir).append("test3.bmp"));
    }

    
    cout << "Done" << endl;
    return 0;
}