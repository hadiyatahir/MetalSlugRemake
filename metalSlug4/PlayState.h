#pragma once
#include "GameState.h"
#include "Soldier.h"
#include "LevelManager.h"
#include "EntityManager.h"
#include "Enemy.h"
#include "Collectible.h"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>




class PlayState : public GameState
{

public:
    static const int max = 50;
    static const int maxCollectibles = 50;
    static const int maxPrisoners = 16;
    Prisoner prisoners[maxPrisoners];
    int      prisonerCount = 0;

private:
    LevelManager* levelManager;

    PlayerSoldier* players[4];
    int playerCount;
    int  activePlayer;
    int biomeId;
    int levelEndX;

    int mScore = 0;
    sf::Font mFont;
    sf::Text mScoreText;
    sf::Text mLivesText[4];     // one per player
    sf::RectangleShape mRedOverlay;
    sf::RectangleShape mBlueOverlay;
    

   // static const int maxCollectibles = 50;

    sf::Sprite  collectibles[maxCollectibles];
    sf::Texture collectibleTex[3];

    float collectiblePos[maxCollectibles * 2];  // x,y pairs
    bool  collectibleActive[maxCollectibles];
    int   collectibleType[maxCollectibles];

    int collectibleCount;


    /*const int max = 50;*/

    Enemies* enemies[max];
    int            enemyCount;
    bool isLoading;
    sf::Texture loadingText;
    sf::Sprite loadingSprite;

    // level grid passed to soldier/enemy update
    char** lvl;
    int lvlWidth;
    int lvlHeight;
    int cellSize;

    int screenWidth;
    int screenHeight;

public:
    PlayState(int screenW, int screenH, int id, int numPlayers = 1);
    ~PlayState();

    //for collectibes
    void spawnCollectibles(int biomeId);


    void enter() override;
    void exit()  override;
    void handleInput(sf::RenderWindow& window) override;
    void update(float dt)                      override;
    void draw(sf::RenderWindow& window)        override;
    void checkProjectileEnemyCollisions();
    void checkEnemyPlayerCollisions();

    void spawnPlayers(int biomeId);
    void spawnEnemies(int biomeId);
};