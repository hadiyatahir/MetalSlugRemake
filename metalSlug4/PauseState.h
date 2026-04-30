#pragma once
#ifndef PAUSESTATE_H
#define PAUSESTATE_H
#include "GameState.h"

class PauseState : public GameState {
public:
    void enter() override {
        // show pause menu
    }

    void update() override {
        // check resume input
    }

    void render() override {
        // draw pause screen
    }

    void exit() override {
        // end puase state menu
    }


};

#endif // !PAUSESTATE_H
