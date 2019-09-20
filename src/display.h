#ifndef FRACTAL_DISPLAY_H
#define FRACTAL_DISPLAY_H

#include <vector>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>

#include "general.h"

class Display
{
public:
    Display() : window(sf::VideoMode(windowSize.x * 4, windowSize.y * 4), "DMG")
    {
        tex.create(160, 144);
    }

    void print(std::vector<uint8> &image)
    {
        tex.update(image.data());
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

public:
    void pollEvents()
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
    }
};


#endif //FRACTAL_DISPLAY_H
