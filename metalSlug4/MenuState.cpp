//#include "MenuState.h"
//#include "CharacterSelectState.h"
//#include <SFML/System.hpp>
//#include <SFML/Graphics.hpp>
//#include "PlayState.h"
//
//MenuState::MenuState()
//{
//
//}
//
//void MenuState::enter() {
//    //font.loadFromFile("Fonts/font.ttf");
//    //menuSprite.setTexture(menuTex);
//    menuTex.loadFromFile("Sprites/Menu/menu1.png");
//    menuSprite.setTexture(menuTex);
//
//
//    /*titleText.setFont(font);
//    titleText.setString("METAL SLUG");
//    titleText.setCharacterSize(72);
//    titleText.setFillColor(sf::Color::White);
//    titleText.setPosition(500.f, 300.f);
//
//    promptText.setFont(font);
//    promptText.setString("Press SPACE to start");
//    promptText.setCharacterSize(32);
//    promptText.setFillColor(sf::Color::Yellow);
//    promptText.setPosition(560.f, 450.f);*/
//
//    menuSprite.setScale(3.2f, 3.2f);
//    menuSprite.setPosition(0.f, 0.f);
//
//
//    if (menuMusic.openFromFile("Sprites/Audio/audio_menu_bg.ogg"))
//    {
//        menuMusic.setLoop(true);   
//        menuMusic.setVolume(30.f); 
//        menuMusic.play();
//    }
//
//
//}
//
//void MenuState::exit()
//{
//    menuMusic.stop();
//}
//
//void MenuState::handleInput(sf::RenderWindow& window) 
//{
//   if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter) && !enterPressed) {
//        nextState = new CharacterSelectState();
//        isDone = true;
//        enterPressed = true;
//    }
//
//    if (Keyboard::isKeyPressed(Keyboard::Escape))
//        	{
//        			window.close();
//        	}
//
//    //if (ev.type == sf::Event::KeyPressed)
//    //{
//    //    if (ev.key.code == sf::Keyboard::Enter && !enterPressed)
//    //    {
//    //        enterPressed = true;  // block any further triggers
//    //        gsm->setState(new CharacterSelectState(gsm));
//    //    }
//    //}
//
//
//}
//
//
//
//void MenuState::draw(sf::RenderWindow& window)
//{
//    /*window.draw(titleText);
//    window.draw(promptText);*/
//    window.draw(menuSprite);
//}

#include "MenuState.h"
#include "CharacterSelectState.h"
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include "PlayState.h"

MenuState::MenuState()
{

}

void MenuState::enter() {
    //font.loadFromFile("Fonts/font.ttf");
    //menuSprite.setTexture(menuTex);
    menuTex.loadFromFile("Sprites/Menu/menu1.png");
    menuSprite.setTexture(menuTex);


    /*titleText.setFont(font);
    titleText.setString("METAL SLUG");
    titleText.setCharacterSize(72);
    titleText.setFillColor(sf::Color::White);
    titleText.setPosition(500.f, 300.f);

    promptText.setFont(font);
    promptText.setString("Press SPACE to start");
    promptText.setCharacterSize(32);
    promptText.setFillColor(sf::Color::Yellow);
    promptText.setPosition(560.f, 450.f);*/

    menuSprite.setScale(3.2f, 3.2f);
    menuSprite.setPosition(0.f, 0.f);


    font.loadFromFile("Sprites/Font/metal-slug.ttf");

    campaignText.setFont(font);
    campaignText.setString("CAMPAIGN MODE");
    campaignText.setCharacterSize(20);
    campaignText.setFillColor(sf::Color::White);
    campaignText.setPosition(300.f, 690.f);

    survivalText.setFont(font);
    survivalText.setString("SURVIVAL MODE");
    survivalText.setCharacterSize(20);
    survivalText.setFillColor(sf::Color::White);
    //survivalText.setPosition(500.f, 450.f);
    survivalText.setPosition(300.f, 730.f);

    if (menuMusic.openFromFile("Sprites/Audio/audio_menu_bg.ogg"))
    {
        menuMusic.setLoop(true);
        menuMusic.setVolume(70.f);
        menuMusic.play();
    }


}

void MenuState::exit()
{
    menuMusic.stop();
}
/*
void MenuState::handleInput(sf::RenderWindow& window)
{
   if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter) && !enterPressed) {
        nextState = new CharacterSelectState();
        isDone = true;
        enterPressed = true;
    }

    if (Keyboard::isKeyPressed(Keyboard::Escape))
            {
                    window.close();
            }

    //if (ev.type == sf::Event::KeyPressed)
    //{
    //    if (ev.key.code == sf::Keyboard::Enter && !enterPressed)
    //    {
    //        enterPressed = true;  // block any further triggers
    //        gsm->setState(new CharacterSelectState(gsm));
    //    }
    //}


}*/

void MenuState::handleInput(sf::RenderWindow& window)
{
    static bool upPrev = false;
    static bool downPrev = false;
    static bool enterPrev = false;

    bool upNow = sf::Keyboard::isKeyPressed(sf::Keyboard::Up);
    bool downNow = sf::Keyboard::isKeyPressed(sf::Keyboard::Down);
    bool enterNow = sf::Keyboard::isKeyPressed(sf::Keyboard::Enter);

    if (upNow && !upPrev)
        selected = 0;

    if (downNow && !downPrev)
        selected = 1;

    if (enterNow && !enterPrev)
    {
        nextState = new CharacterSelectState();
        isDone = true;
    }

    upPrev = upNow;
    downPrev = downNow;
    enterPrev = enterNow;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
        window.close();
}

/*
void MenuState::draw(sf::RenderWindow& window)
{
    window.draw(menuSprite);
    window.draw(campaignText);
    window.draw(survivalText);
}*/

void MenuState::draw(sf::RenderWindow& window)
{
    window.draw(menuSprite);

    if (selected == 0)
    {
        campaignText.setFillColor(sf::Color::Yellow);
        survivalText.setFillColor(sf::Color::White);
    }
    else
    {
        campaignText.setFillColor(sf::Color::White);
        survivalText.setFillColor(sf::Color::Yellow);
    }

    window.draw(campaignText);
    window.draw(survivalText);
}