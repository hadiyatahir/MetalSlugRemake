#include "Game.h"
#include "PlayState.h"

Game::Game()
    : window(sf::VideoMode(1600, 900), "Metal Slug", sf::Style::Close)
{
    window.setVerticalSyncEnabled(true);
    window.setFramerateLimit(60);

    // Initial state
    stateManager.setState(new PlayState());
}

void Game::run()
{
    sf::Event ev;

    while (window.isOpen())
    {
        // 🔹 Event handling
        while (window.pollEvent(ev))
        {
            if (ev.type == sf::Event::Closed)
                window.close();

            stateManager.handleEvent(ev);
        }

        // 🔹 Global input (optional)
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
        {
            window.close();
        }

        float dt = 1.0f / 60.0f;

        // 🔹 Update
        stateManager.update(dt);

        // 🔹 Render
        window.clear();
        stateManager.draw(window);
        window.display();
    }
}