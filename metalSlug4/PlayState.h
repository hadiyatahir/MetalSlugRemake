#pragma once
#include "GameState.h"
#include "Soldier.h"
#include "LevelManager.h"
#include "EntityManager.h"
#include "Enemy.h"
#include <SFML/Graphics.hpp>

//
////owns level manager and entity manager
//class PlayState : public GameState
//{
//    //LevelManager* levelManager;
//    //EntityManager* entityManager;
//
//    //int screenWidth;
//    //int screenHeight;
//
//    //// Temporary: track player position for camera
//    //// Later this comes from the player entity itself
//    //float playerX;
//    //float playerY;
//
//    LevelManager* levelManager;
//
//    PlayerSoldier* players[4];
//    int playerCount;
//
//   // Enemies* enemies[20];
//    int enemyCount;
//
//    int activePlayer;
//
//public:
//    PlayState(int screenW, int screenH);
//    ~PlayState();
//
//    void exit() override;
//
//    void enter() override;
//    void handleInput(sf::RenderWindow& window) override;
//    void update(float dt) override;
//    void draw(sf::RenderWindow& window) override;
//};



class PlayState : public GameState
{
    LevelManager* levelManager;

    PlayerSoldier* players[4];
    int            playerCount;
    int            activePlayer;

    Enemies* enemies[20];
    int            enemyCount;

    // level grid passed to soldier/enemy update
    char** lvl;
    int            lvlWidth;
    int            lvlHeight;
    int            cellSize;

    int            screenWidth;
    int            screenHeight;

public:
    PlayState(int screenW, int screenH);
    ~PlayState();

    void enter()       override;
    void exit()        override;
    void handleInput(sf::RenderWindow& window) override;
    void update(float dt)                      override;
    void draw(sf::RenderWindow& window)        override;
};