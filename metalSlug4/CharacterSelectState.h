/*#pragma once
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

*/

#pragma once
#include "GameState.h"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

class CharacterSelectState : public GameState
{
    int selectedCharacter;   // number of players: 1-4

    sf::Texture    selectTex;
    sf::Sprite     selectSprite;

    sf::Font       mFont;
    sf::Text       mTitleText;
    sf::Text mPlayerText[4];
    //sf::Text       mPlayerText;
    sf::Text       mInstructionText;

    sf::Music      menuMusic;

    int currentPlayer;
    int selectedCharacters[4];

    // debounce flags — one per key
    bool mLeftWasPressed = false;
    bool mRightWasPressed = false;
    bool mEnterWasPressed = true;

public:
    CharacterSelectState();



    void enter()                           override;
    void exit()                            override;
    void handleInput(sf::RenderWindow& w)  override;
    void update(float dt)                  override;
    void draw(sf::RenderWindow& window)    override;

    int getSelectedCharacter() const;
};