
#include "PlayState.h"
#include "Soldier.h"
#include "Enemy.h"
#include <SFML/Window/Keyboard.hpp>
#include "LoadingState.h"

PlayState::PlayState(int screenW, int screenH, int id, int numPlayers)
    : screenWidth(screenW), screenHeight(screenH),
    playerCount(numPlayers), enemyCount(0), activePlayer(0),
    cellSize(64), lvlWidth(150), lvlHeight(40), biomeId(id), isLoading(false)
{
    levelManager = new LevelManager(screenW, screenH);
    levelManager->loadLevel(id);
    levelEndX = 5000;
    levelManager->getLevel()->setBiome(biomeId);
   
    if (biomeId == 0)
        levelEndX = 4500;

    else if (biomeId == 1)
        levelEndX = 4500;

    else
        levelEndX = 3500;

    /*spawnPlayers(id);
    spawnEnemies(id);*/


    lvl = levelManager->getLevel()->getGrid();
    cellSize = levelManager->getLevel()->getCellSize();
    lvlWidth = levelManager->getLevel()->getWidth();
    lvlHeight = levelManager->getLevel()->getHeight();

    spawnPlayers(id);
    spawnEnemies(id);
    spawnCollectibles(id);

    mFont.loadFromFile("Sprites/Font/metal-slug.ttf");   // swap for your font path

    // score text — top center
    mScoreText.setFont(mFont);
    mScoreText.setCharacterSize(28);
    mScoreText.setFillColor(sf::Color::Yellow);
    mScoreText.setPosition((float)(screenWidth / 2 - 80), 8.f);

    // lives text — one line per player, top left
    for (int i = 0; i < 4; i++)
    {
        mLivesText[i].setFont(mFont);
        mLivesText[i].setCharacterSize(20);
        mLivesText[i].setFillColor(sf::Color::White);
        mLivesText[i].setPosition(10.f, 10.f + i * 26.f);
    }

    collectibleTex[0].loadFromFile("Sprites/Collectibles/fruit.png");
    collectibleTex[1].loadFromFile("Sprites/Collectibles/supplyCrate.png");
    collectibleTex[2].loadFromFile("Sprites/Collectibles/turkey.png");
    //spawnCollectibles(id);


    // red damage overlay — full screen, drawn conditionally
    mRedOverlay.setSize(sf::Vector2f((float)screenWidth, (float)screenHeight));

    
}



//////////////////////////////////////spawning players////////////////////////////////////////////////
/*
void PlayState::spawnPlayers(int biomeId) {
    players[0] = new PlayerSoldier(0);
    players[1] = new PlayerSoldier(1);
    players[2] = new PlayerSoldier(2);
    players[3] = new PlayerSoldier(3);

    players[0]->setPosition(600.f, 500.f);
    players[1]->setPosition(650.f, 500.f);
    players[2]->setPosition(700.f, 500.f);
    players[3]->setPosition(750.f, 500.f);
}*/


void PlayState::spawnPlayers(int biomeId)
{
    for (int i = 0; i < playerCount; i++)
    {
        players[i] = new PlayerSoldier(i);
        players[i]->setPosition(600.f + i * 50.f, 500.f);
    }
}


/////////////////////////////////spawning enemies///////////////////////////////////////////////////

void PlayState::spawnEnemies(int biomeId) {

    ///BIOME 0

    if (biomeId == 0) {


        //batch of infantry

    //////rebel soldier

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::RebelSoldier);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(760.f, 500.f);
                enemyCount++;
            }
        }



        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::RebelSoldier);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(900.f, 500.f);
                enemyCount++;
            }
        }
        

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::RebelSoldier);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(1200.f, 500.f);
                enemyCount++;
            }
        }

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::RebelSoldier);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(1600.f, 500.f);
                enemyCount++;
            }
        }

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::RebelSoldier);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(2000.f, 500.f);
                enemyCount++;
            }
        }


        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::RebelSoldier);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(3000.f, 500.f);
                enemyCount++;
            }
        }


        //////shielded soldier


        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::ShieldedSoldier);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(820.f, 500.f);
                enemyCount++;
            }
        }

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::ShieldedSoldier);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(1200.f, 500.f);
                enemyCount++;
            }
        }

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::ShieldedSoldier);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(1600.f, 500.f);
                enemyCount++;
            }
        }

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::ShieldedSoldier);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(2000.f, 500.f);
                enemyCount++;
            }
        }

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::ShieldedSoldier);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(2500.f, 500.f);
                enemyCount++;
            }
        }

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::ShieldedSoldier);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(3000.f, 500.f);
                enemyCount++;
            }
        }

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::ShieldedSoldier);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(3500.f, 500.f);
                enemyCount++;
            }
        }



        /////grenade soldier
       
        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::GrenadeSoldier);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(600.f, 500.f);
                enemyCount++;
            }
        }


        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::GrenadeSoldier);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(800.f, 500.f);
                enemyCount++;
            }
        }
        
        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::GrenadeSoldier);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(900.f, 500.f);
                enemyCount++;
            }
        }

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::GrenadeSoldier);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(1500.f, 500.f);
                enemyCount++;
            }
        }

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::GrenadeSoldier);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(2500.f, 500.f);
                enemyCount++;
            }
        }


        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::GrenadeSoldier);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(2500.f, 500.f);
                enemyCount++;
            }
        }

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::GrenadeSoldier);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(3500.f, 500.f);
                enemyCount++;
            }
        }

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::GrenadeSoldier);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(3500.f, 500.f);
                enemyCount++;
            }
        }

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::GrenadeSoldier);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(3900.f, 500.f);
                enemyCount++;
            }
        }

        ////bazooka solldier

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::BazookaSoldier);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(600.f, 500.f);
                enemyCount++;
            }
        }

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::BazookaSoldier);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(800.f, 500.f);
                enemyCount++;
            }
        }

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::BazookaSoldier);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(1200.f, 500.f);
                enemyCount++;
            }
        }
        

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::BazookaSoldier);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(1800.f, 500.f);
                enemyCount++;
            }
        }

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::BazookaSoldier);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(2000.f, 500.f);
                enemyCount++;
            }
        }

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::BazookaSoldier);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(2600.f, 500.f);
                enemyCount++;
            }
        }

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::BazookaSoldier);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(1700.f, 500.f);
                enemyCount++;
            }
        }

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::BazookaSoldier);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(3854.f, 500.f);
                enemyCount++;
            }
        }

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::BazookaSoldier);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(2467.f, 500.f);
                enemyCount++;
            }
        }
        
        ////batch of undead
        
        //////////zombie

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::Zombie);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(600.f, 500.f);
                enemyCount++;
            }
        }

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::Zombie);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(800.f, 500.f);
                enemyCount++;
            }
        }
        
        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::Zombie);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(1300.f, 500.f);
                enemyCount++;
            }
        }

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::Zombie);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(1700.f, 500.f);
                enemyCount++;
            }
        }

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::Zombie);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(2500.f, 500.f);
                enemyCount++;
            }
        }

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::Zombie);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(3456.f, 500.f);
                enemyCount++;
            }
        }

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::Zombie);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(2368.f, 500.f);
                enemyCount++;
            }
        }

        //////mummy

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::Mummy);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(670.f, 500.f);
                enemyCount++;
            }
        }

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::Mummy);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(800.f, 500.f);
                enemyCount++;
            }
        }

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::Mummy);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(900.f, 500.f);
                enemyCount++;
            }
        }

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::Mummy);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(1400.f, 500.f);
                enemyCount++;
            }
        }

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::Mummy);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(1900.f, 500.f);
                enemyCount++;
            }
        }

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::Mummy);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(2500.f, 500.f);
                enemyCount++;
            }
        }

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::Mummy);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(3400.f, 500.f);
                enemyCount++;
            }
        }
        /*enemies[enemyCount] = Enemies::createEnemy(EnemyType::Paratrooper);
        enemies[enemyCount]->setPosition(900.f, -100.f);
        enemyCount++;*/

    } //for plain



     ///BIOME 1

    if (biomeId == 1) {


        //batch of infantry

    //////rebel soldier

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::RebelSoldier);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(760.f, 500.f);
                enemyCount++;
            }
        }



        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::RebelSoldier);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(900.f, 500.f);
                enemyCount++;
            }
        }


        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::RebelSoldier);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(1200.f, 500.f);
                enemyCount++;
            }
        }

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::RebelSoldier);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(1600.f, 500.f);
                enemyCount++;
            }
        }

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::RebelSoldier);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(2000.f, 500.f);
                enemyCount++;
            }
        }


        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::RebelSoldier);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(3000.f, 500.f);
                enemyCount++;
            }
        }


        //////shielded soldier


        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::ShieldedSoldier);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(820.f, 500.f);
                enemyCount++;
            }
        }

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::ShieldedSoldier);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(1200.f, 500.f);
                enemyCount++;
            }
        }

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::ShieldedSoldier);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(1600.f, 500.f);
                enemyCount++;
            }
        }

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::ShieldedSoldier);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(2000.f, 500.f);
                enemyCount++;
            }
        }

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::ShieldedSoldier);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(2500.f, 500.f);
                enemyCount++;
            }
        }

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::ShieldedSoldier);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(3000.f, 500.f);
                enemyCount++;
            }
        }

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::ShieldedSoldier);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(3500.f, 500.f);
                enemyCount++;
            }
        }



        /////grenade soldier

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::GrenadeSoldier);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(600.f, 500.f);
                enemyCount++;
            }
        }


        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::GrenadeSoldier);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(800.f, 500.f);
                enemyCount++;
            }
        }

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::GrenadeSoldier);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(900.f, 500.f);
                enemyCount++;
            }
        }

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::GrenadeSoldier);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(1500.f, 500.f);
                enemyCount++;
            }
        }

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::GrenadeSoldier);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(2500.f, 500.f);
                enemyCount++;
            }
        }


        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::GrenadeSoldier);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(2500.f, 500.f);
                enemyCount++;
            }
        }

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::GrenadeSoldier);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(3500.f, 500.f);
                enemyCount++;
            }
        }

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::GrenadeSoldier);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(3500.f, 500.f);
                enemyCount++;
            }
        }

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::GrenadeSoldier);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(3900.f, 500.f);
                enemyCount++;
            }
        }

        ////bazooka solldier

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::BazookaSoldier);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(600.f, 500.f);
                enemyCount++;
            }
        }

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::BazookaSoldier);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(800.f, 500.f);
                enemyCount++;
            }
        }

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::BazookaSoldier);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(1200.f, 500.f);
                enemyCount++;
            }
        }


        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::BazookaSoldier);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(1800.f, 500.f);
                enemyCount++;
            }
        }

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::BazookaSoldier);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(2000.f, 500.f);
                enemyCount++;
            }
        }

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::BazookaSoldier);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(2600.f, 500.f);
                enemyCount++;
            }
        }

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::BazookaSoldier);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(1700.f, 500.f);
                enemyCount++;
            }
        }

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::BazookaSoldier);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(3854.f, 500.f);
                enemyCount++;
            }
        }

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::BazookaSoldier);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(2467.f, 500.f);
                enemyCount++;
            }
        }

        ////batch of undead

        //////////zombie

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::Marine);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(600.f, 500.f);
                enemyCount++;
            }
        }

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::Marine);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(800.f, 500.f);
                enemyCount++;
            }
        }

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::Marine);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(1300.f, 500.f);
                enemyCount++;
            }
        }

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::Marine);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(1700.f, 500.f);
                enemyCount++;
            }
        }

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::Marine);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(2500.f, 500.f);
                enemyCount++;
            }
        }

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::Marine);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(3456.f, 500.f);
                enemyCount++;
            }
        }

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::Marine);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(2368.f, 500.f);
                enemyCount++;
            }
        }

        //////mummy

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::Paratrooper);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(670.f, 500.f);
                enemyCount++;
            }
        }

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::Paratrooper);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(800.f, 500.f);
                enemyCount++;
            }
        }

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::Paratrooper);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(900.f, 500.f);
                enemyCount++;
            }
        }

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::Paratrooper);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(1400.f, 500.f);
                enemyCount++;
            }
        }

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::Paratrooper);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(1900.f, 500.f);
                enemyCount++;
            }
        }

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::Paratrooper);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(2500.f, 500.f);
                enemyCount++;
            }
        }

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::Paratrooper);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(3400.f, 500.f);
                enemyCount++;
            }
        }
        /*enemies[enemyCount] = Enemies::createEnemy(EnemyType::Paratrooper);
        enemies[enemyCount]->setPosition(900.f, -100.f);
        enemyCount++;*/

    } //for aerial

  ///////////////////BIOME 2

    if (biomeId == 2) {

        //batch of infantry

    //////rebel soldier

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::RebelSoldier);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(760.f, 500.f);
                enemyCount++;
            }
        }



        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::RebelSoldier);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(900.f, 500.f);
                enemyCount++;
            }
        }


        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::RebelSoldier);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(1200.f, 500.f);
                enemyCount++;
            }
        }

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::RebelSoldier);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(1600.f, 500.f);
                enemyCount++;
            }
        }

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::RebelSoldier);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(2000.f, 500.f);
                enemyCount++;
            }
        }


        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::RebelSoldier);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(3000.f, 500.f);
                enemyCount++;
            }
        }


        //////shielded soldier


        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::ShieldedSoldier);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(820.f, 500.f);
                enemyCount++;
            }
        }

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::ShieldedSoldier);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(1200.f, 500.f);
                enemyCount++;
            }
        }

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::ShieldedSoldier);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(1600.f, 500.f);
                enemyCount++;
            }
        }

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::ShieldedSoldier);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(2000.f, 500.f);
                enemyCount++;
            }
        }

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::ShieldedSoldier);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(2500.f, 500.f);
                enemyCount++;
            }
        }

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::ShieldedSoldier);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(3000.f, 500.f);
                enemyCount++;
            }
        }

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::ShieldedSoldier);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(3500.f, 500.f);
                enemyCount++;
            }
        }



        /////grenade soldier

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::GrenadeSoldier);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(600.f, 500.f);
                enemyCount++;
            }
        }


        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::GrenadeSoldier);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(800.f, 500.f);
                enemyCount++;
            }
        }

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::GrenadeSoldier);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(900.f, 500.f);
                enemyCount++;
            }
        }

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::GrenadeSoldier);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(1500.f, 500.f);
                enemyCount++;
            }
        }

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::GrenadeSoldier);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(2500.f, 500.f);
                enemyCount++;
            }
        }


        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::GrenadeSoldier);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(2500.f, 500.f);
                enemyCount++;
            }
        }

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::GrenadeSoldier);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(3500.f, 500.f);
                enemyCount++;
            }
        }

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::GrenadeSoldier);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(3500.f, 500.f);
                enemyCount++;
            }
        }

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::GrenadeSoldier);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(3900.f, 500.f);
                enemyCount++;
            }
        }

        ////bazooka solldier

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::BazookaSoldier);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(600.f, 500.f);
                enemyCount++;
            }
        }

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::BazookaSoldier);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(800.f, 500.f);
                enemyCount++;
            }
        }

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::BazookaSoldier);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(1200.f, 500.f);
                enemyCount++;
            }
        }


        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::BazookaSoldier);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(1800.f, 500.f);
                enemyCount++;
            }
        }

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::BazookaSoldier);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(2000.f, 500.f);
                enemyCount++;
            }
        }

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::BazookaSoldier);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(2600.f, 500.f);
                enemyCount++;
            }
        }

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::BazookaSoldier);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(1700.f, 500.f);
                enemyCount++;
            }
        }

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::BazookaSoldier);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(3854.f, 500.f);
                enemyCount++;
            }
        }

        if (enemyCount < max)
        {
            enemies[enemyCount] = Enemies::createEnemy(EnemyType::BazookaSoldier);

            if (enemies[enemyCount])
            {
                enemies[enemyCount]->setPosition(2467.f, 500.f);
                enemyCount++;
            }
        }




    }  //for aquatic






}  //spwning enemies





PlayState::~PlayState()
{
    for (int i = 0; i < playerCount; i++)
    {
        delete players[i];
        players[i] = nullptr;
    }

    for (int i = 0; i < enemyCount; i++)
    {
        delete enemies[i];
        enemies[i] = nullptr;
    }


    delete levelManager;
    levelManager = nullptr;
}



void PlayState::handleInput(sf::RenderWindow& window)
{
   
    static bool zWasPressed = false;
    bool zNow = sf::Keyboard::isKeyPressed(sf::Keyboard::Z);
    if (zNow && !zWasPressed)
    {
        activePlayer = (activePlayer + 1) % playerCount;
    }
    zWasPressed = zNow;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
        isDone = true;
}



void PlayState::update(float dt)
{
    // set active flags
    for (int i = 0; i < playerCount; i++)
        players[i]->setActive(i == activePlayer);

    // only update the active player
    if (players[activePlayer]->isPlayerAlive())
        players[activePlayer]->update(dt, lvl, cellSize, lvlWidth, lvlHeight, biomeId);

    for (int i = 0; i < enemyCount; i++)
    {
        if (!enemies[i]->isAlive()) continue;
        enemies[i]->setTarget(players[activePlayer]->getSprite().getPosition());
        enemies[i]->update(dt, lvl, cellSize, lvlWidth, lvlHeight, biomeId);
    }

    //bool playerDead = true;

    //for (int i = 0; i < playerCount; i++)
    //{
    //    if (players[i]->isPlayerAlive())
    //    {
    //        playerDead = false;
    //        break;
    //    }
    //}

    //if (playerDead)
    //{
    //    //making a new playstate when player dies
    //    *this = PlayState(screenWidth, screenHeight, biomeId);
    //    return;
    //}


    checkProjectileEnemyCollisions();
    checkEnemyPlayerCollisions();

    for (int i = 0; i < enemyCount; i++)
    {
        if (!enemies[i]->isAlive()) continue;
        Enemies* replacement = enemies[i]->getLandedReplacement();
        if (replacement != nullptr)
        {
            delete enemies[i];
            enemies[i] = replacement;
        }
    }

    if (players[activePlayer]->getSprite().getPosition().x > levelEndX)
    {
        nextState = new LoadingState(biomeId + 1);
        isDone = true;
    }

    // if active player died, switch to next alive one
    if (!players[activePlayer]->isPlayerAlive())
    {
        for (int i = 0; i < playerCount; i++)
        {
            if (players[i]->isPlayerAlive())
            {
                activePlayer = i;
                break;
            }
        }
    }

    levelManager->update(
        players[activePlayer]->getSprite().getPosition().x,
        players[activePlayer]->getSprite().getPosition().y
    );
}




///////////////////////////////////////////////////////SPAWN COLLECTIBLES///////////////////////////////////////////////////////////////////////////

void PlayState::spawnCollectibles(int biomeId)
{
   /* collectibleCount = 0;

    for (int i = 0; i < maxCollectibles; i++)
        collectibleActive[i] = false;

    int count = 10 + rand() % 10;

    for (int i = 0; i < count; i++)
    {
        int idx = collectibleCount;   

        collectibleActive[idx] = true;
        collectibleType[idx] = rand() % 3;

        collectiblePos[idx * 2] = 500.f + rand() % 4000;
        collectiblePos[idx * 2 + 1] = 300.f + rand() % 200;

        collectibles[idx].setTexture(collectibleTex[collectibleType[idx]]);
        collectibles[idx].setScale(1.2f, 1.2f);

        collectibleCount++;
    }*/


    collectibleCount = 0;

    for (int i = 0; i < maxCollectibles; i++)
        collectibleActive[i] = false;

    int count = 10 + rand() % 10;

    for (int i = 0; i < count && i < maxCollectibles; i++)
    {
        collectibleActive[i] = true;

        collectibleType[i] = rand() % 3;

        // spread more naturally in world
        collectiblePos[i * 2] = 300.f + (rand() % 3500);
        collectiblePos[i * 2 + 1] = 200.f + (rand() % 300);

        collectibles[i].setTexture(collectibleTex[collectibleType[i]]);
        collectibles[i].setScale(1.2f, 1.2f);

        collectibleCount++;
    }
}












/*void PlayState::update(float dt)
{
    // ---- MUST be its own separate loop, BEFORE the update loop ----
    for (int i = 0; i < playerCount; i++)
        players[i]->setActive(i == activePlayer);

    // ---- now update all players ----
   /* for (int i = 0; i < playerCount; i++)
    {
        if (!players[i]->isPlayerAlive()) continue;
        players[i]->update(dt, lvl, cellSize, lvlWidth, lvlHeight, biomeId);
    }*/

/*

    for (int i = 0; i < enemyCount; i++)
    {
        if (!enemies[i]->isAlive()) continue;
        enemies[i]->setTarget(players[activePlayer]->getSprite().getPosition());
        enemies[i]->update(dt, lvl, cellSize, lvlWidth, lvlHeight, biomeId);
    }

    checkProjectileEnemyCollisions();
    checkEnemyPlayerCollisions();
   
    for (int i = 0; i < enemyCount; i++)
    {
        if (!enemies[i]->isAlive()) continue;
        Enemies* replacement = enemies[i]->getLandedReplacement();
        if (replacement != nullptr)
        {
            delete enemies[i];
            enemies[i] = replacement;
        }
    }

   
    if (players[activePlayer]->getSprite().getPosition().x > levelEndX)
    {
        /*isDone = true;

        if (biomeId < 2)
        {
            nextState = new LoadingState(
                new PlayState(screenWidth,
                    screenHeight,
                    biomeId + 1)
            );
        }*/

       /* nextState = new LoadingState(biomeId + 1);
        isDone = true;
    }

    // if active player just died, switch to next alive one
    if (!players[activePlayer]->isPlayerAlive())
    {
        for (int i = 0; i < playerCount; i++)
        {
            if (players[i]->isPlayerAlive())
            {
                activePlayer = i;
                break;
            }
        }
    }


    levelManager->update(
        players[activePlayer]->getSprite().getPosition().x,
        players[activePlayer]->getSprite().getPosition().y
    );
}*/

//void PlayState::draw(sf::RenderWindow& window)
//{
//    levelManager->draw(window);
//
//    for (int i = 0; i < playerCount; i++)
//    {
//        window.draw(players[i]->getSprite());
//        if (players[i]->isWeaponLoaded())
//            window.draw(players[i]->getWeaponSprite());
//    }
//
//    for (int i = 0; i < enemyCount; i++)
//        window.draw(enemies[i]->getSprite());
//}
//

//void PlayState::draw(sf::RenderWindow& window)
//{
//
//
//
//  
//
//    levelManager->draw(window);
//
//    int camX = levelManager->getCamX();
//    int camY = levelManager->getCamY();
//
//    levelManager->getLevel()->drawBackground(
//        window,
//        camX,
//        camY,
//        screenWidth,
//        screenHeight
//    );
//
//
//
//    for (int i = 0; i < playerCount; i++)
//    {
//        sf::Sprite& s = players[i]->getSprite();
//
//        // store world position as plain floats
//        float worldX = s.getPosition().x;
//        float worldY = s.getPosition().y;
//
//        // set screen position for drawing
//        s.setPosition(worldX - camX, worldY - camY);
//        window.draw(s);
//
//        if (players[i]->isWeaponLoaded())
//        {
//            sf::Sprite& ws = players[i]->getWeaponSprite();
//            ws.setPosition(worldX - camX, worldY - camY);
//            window.draw(ws);
//        }
//
//        // restore world position so physics works next frame
//        s.setPosition(worldX, worldY);
//    }
//
//    for (int i = 0; i < enemyCount; i++)
//    {
//        sf::Sprite& s = enemies[i]->getSprite();
//
//        float worldX = s.getPosition().x;
//        float worldY = s.getPosition().y;
//
//        s.setPosition(worldX - camX, worldY - camY);
//        window.draw(s);
//
//        // restore
//        s.setPosition(worldX, worldY);
//    }
//}


void PlayState::checkProjectileEnemyCollisions()
{
    for (int p = 0; p < playerCount; p++)
    {
        Weapon* w = players[p]->getWeapon();
        if (!w) continue;
        w->resolveHits(enemies, enemyCount, cellSize, mScore);
    }
}

void PlayState::checkEnemyPlayerCollisions()
{
    for (int i = 0; i < enemyCount; i++)
    {
        if (!enemies[i]->isAlive()) continue;
        if (enemies[i]->getSprite().getGlobalBounds()
            .intersects(players[activePlayer]->getSprite().getGlobalBounds()))
        {
            players[activePlayer]->takeDamage();
        }
    }
}


void PlayState::draw(sf::RenderWindow& window)
{
    int camX = levelManager->getCamX();
    int camY = levelManager->getCamY();

  
    levelManager->getLevel()->drawBackground(
        window,
        camX,
        camY,
        screenWidth,
        screenHeight
    );

    
    levelManager->draw(window);  //drawing tiles

   
    for (int i = 0; i < playerCount; i++)
    {
        if (!players[i]->isPlayerAlive()) continue;
        sf::Sprite& s = players[i]->getSprite();

        float worldX = s.getPosition().x;
        float worldY = s.getPosition().y;

        s.setPosition(worldX - camX, worldY - camY);

        window.draw(s);


        if (players[i]->isWeaponLoaded())
        {
            sf::Sprite& ws = players[i]->getWeaponSprite();

            // Apply camera offset via a view so weapon->draw() lands in screen space
            sf::View camView = window.getDefaultView();
            camView.move((float)camX, (float)camY);
            window.setView(camView);
            players[i]->getWeapon()->draw(window);       // ← now offset correctly
            window.setView(window.getDefaultView());      // ← restore

            ws.setPosition(worldX - camX, worldY - camY);
            window.draw(ws);
        }

        players[activePlayer]->drawDevMode(window);
       /* if (players[i]->isWeaponLoaded())
        {
            sf::Sprite& ws = players[i]->getWeaponSprite();

            players[i]->getWeapon()->draw(window);

            ws.setPosition(worldX - camX, worldY - camY);

            window.draw(ws);
        }*/

        s.setPosition(worldX, worldY);
    }

   
    for (int i = 0; i < enemyCount; i++)
    {
        if (!enemies[i]->isAlive()) continue;
        sf::Sprite& s = enemies[i]->getSprite();

        float worldX = s.getPosition().x;
        float worldY = s.getPosition().y;

        s.setPosition(worldX - camX, worldY - camY);

        window.draw(s);

        s.setPosition(worldX, worldY);
    }

    //for (int i = 0; i < collectibleCount; i++)
    //{
    //    if (!collectibleActive[i]) continue;

    //    collectibles[i].setPosition(
    //        collectiblePos[i * 2],
    //        collectiblePos[i * 2 + 1]
    //    );

    //    window.draw(collectibles[i]);
    //}


    for (int i = 0; i < collectibleCount; i++)
    {
        if (!collectibleActive[i]) continue;

        collectibles[i].setPosition(
            collectiblePos[i * 2],
            collectiblePos[i * 2 + 1]
        );

        window.draw(collectibles[i]);
    }

    

    // ---- red damage overlay for active player ----
    DamageState ds = players[activePlayer]->getDamageState();
    if (ds == DamageState::INJURED)
    {
        mRedOverlay.setFillColor(sf::Color(255, 0, 0, 60));
        window.draw(mRedOverlay);
    }
    else if (ds == DamageState::CRITICAL)
    {
        mRedOverlay.setFillColor(sf::Color(255, 0, 0, 130));
        window.draw(mRedOverlay);
    }

    // ---- score HUD ----
    char scoreBuf[64];
    snprintf(scoreBuf, sizeof(scoreBuf), "SCORE: %d", mScore);
    mScoreText.setString(scoreBuf);
    window.draw(mScoreText);

    // ---- lives HUD — one line per player ----
    const char* names[4] = { "P1", "P2", "P3", "P4" };
    for (int i = 0; i < playerCount; i++)
    {
        char livesBuf[32];
        if (players[i]->isPlayerAlive())
            snprintf(livesBuf, sizeof(livesBuf),
                "%s  LIVES: %d", names[i], players[i]->getLives());
        else
            snprintf(livesBuf, sizeof(livesBuf), "%s  DEAD", names[i]);

        // highlight the active player
        if (i == activePlayer)
            mLivesText[i].setFillColor(sf::Color::Yellow);
        else
            mLivesText[i].setFillColor(sf::Color(180, 180, 180));

        mLivesText[i].setString(livesBuf);
        window.draw(mLivesText[i]);
    }
}










void PlayState::enter() {}
void PlayState::exit() {}