//#pragma once
//#ifndef PLAYSTATE_H
//#define PLAYSTATE_H
//#include "EntityManager.h"
//#include "LevelManager.h"
//
//class PlayState : public GameState {
////public:
////    void enter() override {
////        // initialize gameplay
////    }
////
////    void update() override {
////        // update player 
////    }
////
////    void render() override {
////        // draw game (later your partner connects animation)
////    }
////
////    void exit() override {
////        // cleanup
////    }
//
//private:
//    EntityManager entityManager;
//    LevelManager levelManager;
//public:
//    PlayState();
//    void handleEvent(sf::Event& ev) override;
//    void update(float dt) override;
//    void draw(sf::RenderWindow& window) override;
//};
//
//
//
//
//
//
//
//#endif // !PLAYSTATE_H


#pragma once
#include "GameState.h"
#include "LevelManager.h"
#include "EntityManager.h"

class PlayState : public GameState
{
private:
    LevelManager levelManager;
    EntityManager entityManager;

    int score;
    bool levelComplete;

public:
    PlayState();
    ~PlayState() override;

    void enter() override;
    void exit() override;
    void handleEvent(sf::Event& ev) override;
    void update(float dt) override;
    void render(sf::RenderWindow& window) override;
};