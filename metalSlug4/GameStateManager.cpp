#include "GameStateManager.h"


GameStateManager::GameStateManager() : count(0)
{
    for (int i = 0; i < max_states; i++)
        states[i] = nullptr;
}


GameStateManager::~GameStateManager()
{
    for (int i = 0; i < max_states; i++)
    {
        if (states[i] != nullptr)
        {
            delete states[i];
            states[i] = nullptr;
        }
    }
    count = 0;
}

void GameStateManager::addState(GameState* newState)
{
    if (count >= max_states) return;

    states[count] = newState;
    states[count]->enter();  
    count++;
}

void GameStateManager::removeState()
{
    if (count == 0)
        return;

    states[count - 1]->exit();

    delete states[count - 1];
    states[count - 1] = nullptr;

    count--;
}


void GameStateManager::switchState(GameState* newState)
{
    // delete all loaded states
    while (count > 0)
        removeState();

    addState(newState);
}


bool GameStateManager::isEmpty() const {
    return count == 0; 
}

void GameStateManager::handleInput(sf::RenderWindow& window)
{
    if (count == 0) return;
    states[count - 1]->handleInput(window);
}

void GameStateManager::update(float dt)
{
    /*if (count == 0) return;

    states[count - 1]->update(dt);

   
    if (states[count - 1]->isDone)
        removeState();*/

    //if (count == 0) return;

    //GameState* current = states[count - 1];
    //current->update(dt);

    //if (current->isDone)
    //{
    //    GameState* next = current->nextState;

    //    removeState(); // removes current

    //    if (next)
    //        addState(next);
    //}


    if (count == 0) return;

    GameState* current = states[count - 1];
    current->update(dt);

    // only mark transition, don't delete yet
   /* if (current->isDone)
    {
        GameState* next = current->nextState;

        current->exit();
        removeState();

        if (next)
        {
            addState(next);
        }
    }*/

    if (current->isDone)
    {
        GameState* next = current->nextState;
        current->nextState = nullptr;  // detach so removeState doesn't delete it
        removeState();                 // this calls exit() and deletes current
        if (next)
            addState(next);            // this calls enter() on the new state
    }


}

void GameStateManager::draw(sf::RenderWindow& window)
{
    if (count == 0) return;
    states[count - 1]->draw(window);
}
