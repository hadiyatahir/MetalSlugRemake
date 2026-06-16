




#pragma once
#include "GameState.h"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>


class MenuState : public GameState
{
private:
    sf::Font font;
    sf::Text campaignText;
    sf::Text survivalText;
    sf::Text scoreText;

    sf::Sprite menuSprite;
    sf::Texture menuTex;
    sf::Sprite bgSprite;
    sf::Texture bgTex;
    sf::Texture titleTex;
    sf::Sprite titleSprite;

    sf::Music menuMusic;
    bool enterPressed;
    int selected = 0;
public:
    MenuState();

    void exit() override;

    void enter() override;

    void handleInput(sf::RenderWindow& window) override;

    void update(float dt) override {}

    void draw(sf::RenderWindow& window) override;
};
