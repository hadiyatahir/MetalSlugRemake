#include "Game.h"
#include "MenuState.h"
#include "PlayState.h"

Game::Game()
    : screenWidth(1600), screenHeight(900)   //game is responsible for creating the window, basically the game starts from here
{
    window.create(sf::VideoMode(screenWidth, screenHeight), "Metal Slug", sf::Style::Close);
    window.setVerticalSyncEnabled(true);
    window.setFramerateLimit(60);

    stateManager = new GameStateManager();
    stateManager->addState(new MenuState());
}

Game::~Game()
{
    delete stateManager;
    stateManager = nullptr;
}

//main run loop that will run until the window closes
void Game::run()
{
    while (window.isOpen())
    {
        float dt = clock.restart().asSeconds();     //dt is basically the seconds since last frame, so that it is frame and system independent
        if (dt > 0.05f) dt = 0.05f;  // cap to prevent infinite loop

        sf::Event ev;
        while (window.pollEvent(ev))
        {
            if (ev.type == sf::Event::Closed)
                window.close();
        }

    
        //if (stateManager->isEmpty())     //if statemanager is emoty that means that menu state is no longer present and gameplay must proceed
        //    stateManager->switchState(new PlayState(screenWidth, screenHeight));

        stateManager->handleInput(window);
        stateManager->update(dt);

        window.clear(sf::Color::Black);
        stateManager->draw(window);
        window.display();

        //if (stateManager->isEmpty())
        //    window.close();
    }
}
