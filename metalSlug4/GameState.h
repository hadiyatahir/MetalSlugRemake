#pragma once
#include <SFML/Graphics.hpp>


class GameState
{
public:
    virtual ~GameState() {}

    virtual void handleInput(sf::RenderWindow& window) = 0;
    virtual void update(float dt) = 0;
    virtual void draw(sf::RenderWindow& window) = 0;
    virtual void enter() = 0;
    virtual void exit() = 0;
    bool isDone = false;  //state would set this as true when it is going to be removed nad gamestate manager will check it after each updatet
    GameState* nextState = nullptr;
};
