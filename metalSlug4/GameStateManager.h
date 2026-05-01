//#pragma once
//#ifndef GAMESTATEMANAGER_H
//#define GAMESTATEMANAGER_H
//#include "GameState.h"
//#include "PlayState.h"
//
//class GameStateManager {
////private:
////    GameState* currentState;      //composition
////
////public:
////    GameStateManager() : currentState(nullptr) {}
////
////    void changeState(GameState* newState) {
////        if (currentState != nullptr) {
////            currentState->exit();
////            delete currentState;
////        }
////        currentState = newState;
////        currentState->enter();
////    }
////
////    void update() {
////        if (currentState)
////            currentState->update();
////    }
////
////    void render() {
////        if (currentState)
////            currentState->render();
////    }
//
//private:
//    GameState* currentState;
//    GameState* pendingState;
//    bool hasPending;
//
//public:
//    GameStateManager();
//    ~GameStateManager();
//
//    void setState(GameState* newState);
//
//    void handleEvent(sf::Event& ev);
//    void update(float dt);
//    void draw(sf::RenderWindow& window);
//
//
//
//    /*GameState* currentState;
//public:
//    void setState(GameState* newState);
//    void handleEvent(sf::Event& ev);
//    void update(float dt);
//    void draw(sf::RenderWindow& window);*/
//};
//
//
//#endif // !GAMESTATEMANAGER_H



// GameStateManager.h
#pragma once
#include <SFML/Graphics.hpp>
#include "GameState.h"

class GameStateManager
{
private:
    GameState* currentState;    //switching bw playstate, menu state or pause state
    GameState* pendingState;
    bool hasPending;

public:
    GameStateManager();
    ~GameStateManager();

    void setState(GameState* newState);

    void handleEvent(sf::Event& ev);
    void update(float dt);
    void draw(sf::RenderWindow& window);
};