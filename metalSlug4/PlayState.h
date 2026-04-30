#pragma once
#ifndef PLAYSTATE_H
#define PLAYSTATE_H

class PlayState : public GameState {
public:
    void enter() override {
        // initialize gameplay
    }

    void update() override {
        // update player 
    }

    void render() override {
        // draw game (later your partner connects animation)
    }

    void exit() override {
        // cleanup
    }
};







#endif // !PLAYSTATE_H
