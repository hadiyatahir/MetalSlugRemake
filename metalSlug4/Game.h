#pragma once
#include <SFML/Graphics.hpp>
#include "GameStateManager.h"

// ============================================================
// Game.h
// ============================================================
// The outermost shell of the application.
// Owns: the window, the clock, the state manager.
// Does: creates the initial state, then loops forever.
//
// Game::run() is the ONLY game loop. It:
//   1. Measures delta time (time since last frame)
//   2. Polls SFML window events (close button, etc.)
//   3. Delegates input/update/draw to the state manager
// ============================================================

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
