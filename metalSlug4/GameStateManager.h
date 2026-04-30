#pragma once
#ifndef GAMESTATEMANAGER_H
#define GAMESTATEMANAGER_H
#include "GameState.h"
#include "PlayState.h"

class GameStateManager {
private:
    GameState* currentState;      //composition

public:
    GameStateManager() : currentState(nullptr) {}

    void changeState(GameState* newState) {
        if (currentState != nullptr) {
            currentState->exit();
            delete currentState;
        }
        currentState = newState;
        currentState->enter();
    }

    void update() {
        if (currentState)
            currentState->update();
    }

    void render() {
        if (currentState)
            currentState->render();
    }
};


#endif // !GAMESTATEMANAGER_H
