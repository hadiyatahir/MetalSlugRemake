#pragma once
#include "GameState.h"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

// ============================================================
// MenuState.h / MenuState.cpp  (combined for brevity)
// ============================================================
// A simple title screen. When Space is pressed, sets isDone=true.
// PlayState notices and the manager pops to reveal PlayState
// (or GameStateManager::changeState swaps to PlayState).
// ============================================================

class MenuState : public GameState
{
private:
   /* sf::Font font;
    sf::Text titleText;
    sf::Text promptText;*/
    sf::Sprite menuSprite;
    sf::Texture menuTex;
    sf::Music menuMusic;
    bool enterPressed;

public:
    MenuState();

    void exit() override;

    void enter() override;

    void handleInput(sf::RenderWindow& window) override;

    void update(float dt) override {}

    void draw(sf::RenderWindow& window) override;
};
