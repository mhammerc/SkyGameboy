#ifndef FRACTAL_DISPLAY_H
#define FRACTAL_DISPLAY_H

#include <vector>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>

#include "../general.h"
#include "interfaces/i_display.h"

using namespace EmulatorConstants;

class Display : public IDisplay
{
public:
    Display() : window(sf::VideoMode(windowSize.x * 4, windowSize.y * 4), "DMG")
    {
        tex.create(160, 144);
    }

    void newFrameIsReady(const std::vector<uint8> &frame) override
    {
        pollEvents();

        for (size_t i = 0, j = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT * 3; i += 3, j += 4)
        {
            RGBAFrame[j + 0] = frame[i + 0];
            RGBAFrame[j + 1] = frame[i + 1];
            RGBAFrame[j + 2] = frame[i + 2];
            RGBAFrame[j + 3] = 255;
        }

        tex.update(RGBAFrame.data());
        sf::Sprite s(tex);
        s.scale(4, 4);
        window.clear(sf::Color::Black);
        window.draw(s);
        window.display();
    }

private:
    sf::Vector2i windowSize = sf::Vector2i(160, 144);
    sf::RenderWindow window;
    sf::Texture tex;
    // Pixels as RGBA
    std::vector<uint8> RGBAFrame = std::vector<uint8>(SCREEN_WIDTH * SCREEN_HEIGHT * 4);

    void pollEvents()
    {
        sf::Event event{};
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
    }
};


#endif //FRACTAL_DISPLAY_H
