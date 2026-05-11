//#pragma once
//#include <SFML/Graphics.hpp>
//
//
//class GameState
//{
//public:
//    virtual ~GameState() {}
//
//    virtual void handleInput(sf::RenderWindow& window) = 0;
//    virtual void update(float dt) = 0;
//    virtual void draw(sf::RenderWindow& window) = 0;
//    virtual void enter() = 0;
//    virtual void exit() = 0;
//    bool isDone = false;  //state would set this as true when it is going to be removed nad gamestate manager will check it after each updatet
//    GameState* nextState = nullptr;
//};


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

class GameOverState : public GameState
{
private:
    sf::Texture gameOverTex;
    sf::Sprite gameOverSprite;

    sf::Font font;
    sf::Text scoreText;

    int finalScore;

    //int finalScore;

public:
    GameOverState(int score);
    void enter() override;
    // void handleInput(sf::RenderWindow& window) override;
    void handleInput(sf::RenderWindow& window) override {}
    void update(float dt) override {}
    void draw(sf::RenderWindow& window) override;
    void exit() override {}
};