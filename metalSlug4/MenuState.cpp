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


}



void MenuState::draw(sf::RenderWindow& window)
{
    /*window.draw(titleText);
    window.draw(promptText);*/
    window.draw(menuSprite);
}