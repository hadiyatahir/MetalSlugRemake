//#pragma once
//#ifndef GAME_H
//#define GAME_H
//
//#include "GameStateManager.h"
//
//class Game {
//private:
// /*   GameStateManager gsm;
//    bool running;
//
//public:
//    Game() : running(true) {}
//
//    void init() {
//        gsm.changeState(new PlayState());
//    }
//
//    void run() {
//        while (running) {
//            gsm.update();
//            gsm.render();
//        }
//    }*/
//
////private:
////    sf::RenderWindow window;
////    sf::Clock clock;
////    GameStateManager gsm;
////
////    // Your existing constants move here
////    static const int SCREEN_X = 1600;
////    static const int SCREEN_Y = 900;
////
////public:
////    Game();
////    void run();
////
////private:
////    void processEvents();
////    void update(float dt);
////    void render();
//
//    sf::RenderWindow window;
//    GameStateManager stateManager;
//public:
//    Game();
//    void run();
//
//
//
//};
//
//
//
//
//#endif // !GAME_H

#pragma once
#include <SFML/Graphics.hpp>
#include "GameStateManager.h"
using namespace sf;

class Game
{
private:
    RenderWindow window;
    Clock clock;      // for real time difference
    GameStateManager stateManager;

public:
    Game();
    void run();
};