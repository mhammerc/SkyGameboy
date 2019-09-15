#include <memory>

#include "file_reader.h"
#include "virtual_memory.h"
#include "cpu.h"

// startup:
// 0x0000h = PC -> boot loader
int main()
{
    VirtualMemory virtualMemory("roms/dmg_boot.bin");
//    VirtualMemory virtualMemory("/Users/mhammerc/Downloads/cpu_instrs/cpu_instrs.gb");
    CPU cpu(virtualMemory);

    while (true)
    {
        cpu.nextTick();
    }

    return 0;
}

//#include <vector>
//
//#include <SFML/Window.hpp>
//#include <SFML/Graphics.hpp>
//#include <SFML/System/Vector2.hpp>
//
//typedef unsigned char uint8;
//
//void pollEvents(sf::Window &window)
//{
//    sf::Event event;
//    while (window.pollEvent(event))
//    {
//        if (event.type == sf::Event::Closed)
//            window.close();
//    }
//}
//
//struct RawImage
//{
//    sf::Vector2i size;
//    std::vector<uint8> data;
//};
//
//RawImage generateRawImage(const sf::Vector2i &size)
//{
//    RawImage raw;
//    raw.size = size;
//
//    raw.data.resize(size.x * size.y * 4);
//    for (int i = 0; i < raw.data.size(); i += 4)
//    {
//        raw.data[i + 0] = 255;
//        raw.data[i + 1] = 255;
//        raw.data[i + 2] = 255;
//        raw.data[i + 3] = 255;
//    }
//
//    return raw;
//}
//
//int main()
//{
//    sf::Vector2i windowSize(800, 600);
//    sf::RenderWindow window(sf::VideoMode(windowSize.x, windowSize.y), "Fractal");
//
//    RawImage raw = generateRawImage(windowSize);
//
//    sf::Texture tex;
//    tex.create(raw.size.x, raw.size.y);
//    tex.update(raw.data.data());
//
//    sf::Sprite s(tex);
//
//    while (window.isOpen())
//    {
//        pollEvents(window);
//
//        window.clear(sf::Color::Black);
//        window.draw(s);
//        window.display();
//    }
//
//    return 0;
//}