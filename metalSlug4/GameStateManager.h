#pragma once
#include "GameState.h"
#include <SFML/Graphics.hpp>



//this is responsible for knwoing which gamestate is currently running
class GameStateManager
{
    static const int max_states = 8;
    GameState* states[max_states];
    int count;   // how many states are currently loaded

public:
    GameStateManager();
    ~GameStateManager();

    // loading a new state
    void addState(GameState* newState);

    // destroying the current state
    void removeState();

    //switching to a new state while destroying the previous one
    void switchState(GameState* newState);

    bool isEmpty() const;

    void handleInput(sf::RenderWindow& window);
    void update(float dt);
    void draw(sf::RenderWindow& window);
};
