//#include "GameStateManager.h"
//
//GameStateManager::GameStateManager()
//{
//    currentState = nullptr;
//}
//
//void GameStateManager::setState(GameState* newState)
//{
//    if (currentState != nullptr)
//    {
//        currentState->exit();
//        delete currentState;
//    }
//
//    currentState = newState;
//
//    if (currentState != nullptr)
//    {
//        currentState->enter();
//    }
//}
//
//void GameStateManager::handleEvent(sf::Event& ev)
//{
//    if (currentState)
//        currentState->handleEvent(ev);
//}
//
//void GameStateManager::update(float dt)
//{
//    if (currentState)
//        currentState->update(dt);
//}
//
//void GameStateManager::draw(sf::RenderWindow& window)
//{
//    if (currentState)
//        currentState->render(window);
//}

#include "GameStateManager.h"

GameStateManager::GameStateManager()
    : currentState(nullptr),
    pendingState(nullptr),
    hasPending(false)
{
}

GameStateManager::~GameStateManager()
{
    if (currentState)
    {
        currentState->exit();
        delete currentState;                   
    }
    if (pendingState)
        delete pendingState;
}

void GameStateManager::setState(GameState* newState)
{
   
    if (pendingState)
        delete pendingState;             //adding pedning state to delay the switch between the states
                                                //this is to avoid crashes of implemtnign logic on deleted objects
    pendingState = newState;
    hasPending = true;
}

void GameStateManager::handleEvent(sf::Event& ev)
{
    if (currentState)
        currentState->handleEvent(ev);
}

void GameStateManager::update(float dt)
{
    if (hasPending)
    {
        if (currentState)
        {
            currentState->exit();
            delete currentState;
        }
        currentState = pendingState;
        pendingState = nullptr;
        hasPending = false;

        if (currentState)
            currentState->enter();
    }

    if (currentState)
        currentState->update(dt);
}

void GameStateManager::draw(sf::RenderWindow& window)
{
    if (currentState)
        currentState->render(window);
}