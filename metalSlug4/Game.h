#pragma once
#ifndef GAME_H
#define GAME_H

#include "GameStateManager.h"

class Game {
private:
    GameStateManager gsm;
    bool running;

public:
    Game() : running(true) {}

    void init() {
        gsm.changeState(new PlayState());
    }

    void run() {
        while (running) {
            gsm.update();
            gsm.render();
        }
    }
};




#endif // !GAME_H
