#pragma once
#include <SFML/Graphics.hpp>
#include "GameStateManager.h"


class Game
{
    sf::RenderWindow window;
    sf::Clock clock;
    GameStateManager* stateManager;

    int screenWidth;
    int screenHeight;
    //int selectedCharacter;

public:
    Game();
    ~Game();
    void run();  // blocks until the window is closed
};
