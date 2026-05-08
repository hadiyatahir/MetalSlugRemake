#include "CharacterSelectState.h"
#include "PlayState.h"


CharacterSelectState::CharacterSelectState()
{
    this->selectedCharacter = 0;
}

void CharacterSelectState::enter() {
   
        //font.loadFromFile("Fonts/font.ttf");
        //menuSprite.setTexture(menuTex);
        selectTex.loadFromFile("Sprites/Menu/characterbg.jpg");
        selectSprite.setTexture(selectTex);

        selectSprite.setScale(4.f, 3.9f);
        selectSprite.setPosition(0.f, 0.f);


        //p1
        p1tex.loadFromFile("Sprites/MarcoRossi/pistol/idleRight.png");
        p1.setTexture(p1tex);

        p1.setScale(2.f, 2.f);
        p1.setPosition(900, 200);



        if (menuMusic.openFromFile("Sprites/Audio/audio_menu_bg.ogg"))
        {
            menuMusic.setLoop(true);
            menuMusic.setVolume(50.f);
            menuMusic.play();
        }


}

void CharacterSelectState::exit() {
    menuMusic.stop();
}


void CharacterSelectState::handleInput(sf::RenderWindow& window) {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter) && !enterPressed) {
        nextState = new PlayState(1600, 900);
        isDone = true;
        enterPressed = true;
    }

    if (Keyboard::isKeyPressed(Keyboard::Escape))
    {
        window.close();
    }
}

void CharacterSelectState::update(float dt) {

}

void CharacterSelectState::draw(sf::RenderWindow& window) {
    window.draw(selectSprite);
    window.draw(p1);
}


int CharacterSelectState::getSelectedCharacter() const { 
    return selectedCharacter; 
}