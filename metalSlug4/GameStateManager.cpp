#include "GameStateManager.h"

GameStateManager::GameStateManager()
{
    currentState = nullptr;
}

void GameStateManager::setState(GameState* newState)
{
    if (currentState != nullptr)
    {
        currentState->exit();
        delete currentState;
    }

    currentState = newState;

    if (currentState != nullptr)
    {
        currentState->enter();
    }
}

void GameStateManager::handleEvent(sf::Event& ev)
{
    if (currentState)
        currentState->handleEvent(ev);
}

void GameStateManager::update(float dt)
{
    if (currentState)
        currentState->update(dt);
}

void GameStateManager::draw(sf::RenderWindow& window)
{
    if (currentState)
        currentState->render(window);
}