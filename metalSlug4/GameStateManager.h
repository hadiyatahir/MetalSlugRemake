#pragma once
#include "GameState.h"
#include <SFML/Graphics.hpp>

// ============================================================
// GameStateManager.h
// ============================================================
// Manages which GameState is currently running.
// Uses a fixed raw array of GameState pointers and an integer
// 'count' to track how many states are currently loaded.
//
// The CURRENT (active) state is always states[count - 1].
// States below it are paused but alive (e.g. Play under Pause).
//
// Three operations:
//   addState()    -- load a new state on top of the current one
//   removeState() -- destroy the current state, resume the one below
//   switchState() -- destroy ALL states, load a single fresh one
// ============================================================


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
