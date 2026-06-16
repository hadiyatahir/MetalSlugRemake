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

   

    menuTex.loadFromFile("Sprites/Menu/loadingsc.png");
    titleTex.loadFromFile("Sprites/Menu/title2.png");
    bgTex.loadFromFile("Sprites/Menu/bg.png");

    menuSprite.setTexture(menuTex);
    titleSprite.setTexture(titleTex);
    bgSprite.setTexture(bgTex);
   // menuSprite.setScale();


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

    menuSprite.setScale(1.05f, 1.05f);
    titleSprite.setScale(4.0f, 4.0f);
    titleSprite.setPosition(200.f, -10.f);
    menuSprite.setPosition(900.f, 200.f);
    bgSprite.setPosition(-10.f,-370.f);
    bgSprite.setScale(1.5f,1.5f);

    font.loadFromFile("Sprites/Font/metal-slug.ttf");
    // --- CAMPAIGN TEXT ---
    campaignText.setFont(font);
    campaignText.setString("CAMPAIGN MODE");
    campaignText.setCharacterSize(30);
    campaignText.setFillColor(sf::Color::Red);
    campaignText.setPosition(100.f, 700.f);

    // --- SURVIVAL TEXT ---
    survivalText.setFont(font);
    survivalText.setString("SURVIVAL MODE");
    survivalText.setCharacterSize(30);
    survivalText.setFillColor(sf::Color::Red);
    survivalText.setPosition(100.f, 750.f);

    // --- SCOREBOARD TEXT (Fixed Typo Here) ---
    scoreText.setFont(font);
    scoreText.setString("SCOREBOARD"); // Fixed
    scoreText.setCharacterSize(30);    // Fixed
    scoreText.setFillColor(sf::Color::Red); // Fixed
    scoreText.setPosition(100.f, 800.f);


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

    // Cycle Upwards
    if (upNow && !upPrev) {
        selected--;
        if (selected < 0) {
            selected = 2; // Loop back to the bottom option (Scoreboard)
        }
    }

    // Cycle Downwards
    if (downNow && !downPrev) {
        selected++;
        if (selected > 2) {
            selected = 0; // Loop back to the top option (Campaign)
        }
    }

    // Handle Selection Execution
    if (enterNow && !enterPrev)
    {
        if (selected == 0)
        {
            nextState = new CharacterSelectState(); // Or whatever starts your Campaign
            isDone = true;
        }
        else if (selected == 1)
        {
            // nextState = new SurvivalState(); // Uncomment when you create this state
            // isDone = true;
        }
        else if (selected == 2)
        {
            // nextState = new ScoreboardState(); // Uncomment when you create this state
            // isDone = true;

            // Temporary debug line to make sure it works:
            std::cout << "Scoreboard Selected!" << std::endl;
        }
    }

    // Save previous button states
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
    window.draw(bgSprite);
    window.draw(titleSprite);

    // Reset sizes and colors to defaults first
    campaignText.setFillColor(sf::Color::White);
    campaignText.setCharacterSize(20);

    survivalText.setFillColor(sf::Color::White);
    survivalText.setCharacterSize(20);

    scoreText.setFillColor(sf::Color::White);
    scoreText.setCharacterSize(20);

    // Apply Highlight to the active option
    if (selected == 0)
    {
        campaignText.setFillColor(sf::Color::Yellow);
        campaignText.setCharacterSize(22);
    }
    else if (selected == 1)
    {
        survivalText.setFillColor(sf::Color::Yellow);
        survivalText.setCharacterSize(22);
    }
    else if (selected == 2)
    {
        scoreText.setFillColor(sf::Color::Yellow);
        scoreText.setCharacterSize(22);
    }

    // Draw all text items to the window
    window.draw(campaignText);
    window.draw(survivalText);
    window.draw(scoreText); // Added this so it actually renders!
}