#pragma once
#ifndef GAMESTATE_H
#define GAMESTATE_H
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

class GameState {    //pure virtual fucntion for playstate etc to implement
public:
    virtual void enter() = 0;
    virtual void exit() = 0;

    virtual void handleEvent(sf::Event& ev) = 0;
    virtual void update(float dt) = 0;
    virtual void render(sf::RenderWindow& window) = 0;

    virtual ~GameState() {}




};



#endif // !GAMESTATE_H
