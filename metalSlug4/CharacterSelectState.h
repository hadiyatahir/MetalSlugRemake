#pragma once
#include "GameState.h"
#include <SFML/Audio.hpp>
#include <SFML/System.hpp>
using namespace sf;


class CharacterSelectState : public GameState {
private:
	int selectedCharacter;
    Sprite selectSprite;
    Texture selectTex;
    Music menuMusic;
    Sprite p1;
    Sprite p2;
    Sprite p3;
    Sprite p4;
    Texture p1tex;
    Texture p2tex;
    Texture p3tex;
    Texture p4tex;
    Font menu;
    bool enterPressed;

public:
    CharacterSelectState();
    void enter() override;
    void exit() override;
    void handleInput(sf::RenderWindow& window) override;
    void update(float dt) override;
    void draw(sf::RenderWindow& window) override;

    int getSelectedCharacter() const;
};