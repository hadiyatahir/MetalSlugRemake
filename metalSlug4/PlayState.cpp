//#include "PlayState.h"
//#include "Soldier.h"
//#include <SFML/Window/Keyboard.hpp>
//
//// ============================================================
//// Constructor
//// ============================================================
//// Create the managers, load the level, spawn initial entities.
//// ============================================================
//PlayState::PlayState(int screenW, int screenH)
//    : screenWidth(screenW), screenHeight(screenH),
//    playerX(380.f), playerY(610.f)
//{
//    levelManager = new LevelManager(screenW, screenH);
//    entityManager = new EntityManager();
//
//    levelManager->loadLevel();
//
//    // Later: entityManager->addEntity(new PlayerSoldier(...));
//}
//
//PlayState::~PlayState()
//{
//    delete levelManager;
//   // delete entityManager;
//    levelManager = nullptr;
//    entityManager = nullptr;
//}
//
//// ============================================================
//// handleInput
//// ============================================================
//// For now: move playerX/Y directly with arrow keys.
//// Later: pass input to the player entity instead.
//// ============================================================
//void PlayState::handleInput(sf::RenderWindow& window)
//{
//    float speed = 5.f;
//
//    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) playerX += speed;
//    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))  playerX -= speed;
//    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) isDone = true;
//}
//
//// ============================================================
//// update
//// ============================================================
//// Order matters:
////   1. Update all entities (physics, AI, etc.)
////   2. Update camera to follow player
//// Camera MUST come after entities so it follows the new position,
//// not the old one (would cause 1-frame lag).
//// ============================================================
//void PlayState::update(float dt)
//{
//    //// 1. Update all entities
//    //entityManager->update(dt, levelManager->getLevel());
//
//    //// 2. Move camera to follow player
//    //levelManager->update(playerX, playerY);
//
//    for (int i = 0; i < playerCount; i++)
//        players[i]->update(dt, lvl, cellSize, width, height);
//
//    for (int i = 0; i < enemyCount; i++)
//    {
//        enemies[i]->setTarget(players[activePlayer]->getSprite().getPosition());
//        enemies[i]->update(dt, lvl, cellSize, width, height);
//    }
//
//    levelManager->update(
//        players[activePlayer]->getSprite().getPosition().x,
//        players[activePlayer]->getSprite().getPosition().y
//    );
//
//}
//
//// ============================================================
//// draw
//// ============================================================
//// Order matters — draw back to front:
////   1. Level tiles (background)
////   2. Entities (on top of tiles)
//// ============================================================
//void PlayState::draw(sf::RenderWindow& window)
//{
//    // 1. Draw the level tiles
//    levelManager->draw(window);
//
//    // 2. Draw all entities
//    int camX = levelManager->getCamX();
//    int camY = levelManager->getCamY();
//    entityManager->draw(window, camX, camY);
//}
//
//
//
//void PlayState::exit() {
//
//}
//
//void PlayState::enter() {
//
//}

#include "PlayState.h"
#include "Soldier.h"
#include "Enemy.h"
#include <SFML/Window/Keyboard.hpp>

PlayState::PlayState(int screenW, int screenH)
    : screenWidth(screenW), screenHeight(screenH),
    playerCount(4), enemyCount(0), activePlayer(0),
    cellSize(64), lvlWidth(110), lvlHeight(14)
{
    levelManager = new LevelManager(screenW, screenH);
    levelManager->loadLevel();

    // build the same char** grid your soldiers expect
    lvl = new char* [lvlHeight];
    for (int i = 0; i < lvlHeight; i++)
    {
        lvl[i] = new char[lvlWidth];
        for (int j = 0; j < lvlWidth; j++)
            lvl[i][j] = '\0';
    }
    // ground floor at row 11 — matches LevelManager::loadLevel()
    for (int i = 0; i < lvlWidth; i++)
        lvl[11][i] = 'g';

    // spawn players
    players[0] = new PlayerSoldier(0);
    players[1] = new PlayerSoldier(1);
    players[2] = new PlayerSoldier(2);
    players[3] = new PlayerSoldier(3);

    players[0]->setPosition(600.f, 500.f);
    players[1]->setPosition(650.f, 500.f);
    players[2]->setPosition(700.f, 500.f);
    players[3]->setPosition(750.f, 500.f);

    // spawn enemies — add yours here exactly like main did
    enemies[enemyCount] = Enemies::createEnemy(EnemyType::RebelSoldier);
    enemies[enemyCount]->setPosition(760.f, 500.f);
    enemyCount++;

    enemies[enemyCount] = Enemies::createEnemy(EnemyType::ShieldedSoldier);
    enemies[enemyCount]->setPosition(820.f, 500.f);
    enemyCount++;

    enemies[enemyCount] = Enemies::createEnemy(EnemyType::GrenadeSoldier);
    enemies[enemyCount]->setPosition(880.f, 500.f);
    enemyCount++;

    enemies[enemyCount] = Enemies::createEnemy(EnemyType::BazookaSoldier);
    enemies[enemyCount]->setPosition(940.f, 500.f);
    enemyCount++;

    enemies[enemyCount] = Enemies::createEnemy(EnemyType::Zombie);
    enemies[enemyCount]->setPosition(1000.f, 500.f);
    enemyCount++;

    enemies[enemyCount] = Enemies::createEnemy(EnemyType::Mummy);
    enemies[enemyCount]->setPosition(1060.f, 500.f);
    enemyCount++;

    enemies[enemyCount] = Enemies::createEnemy(EnemyType::Paratrooper);
    enemies[enemyCount]->setPosition(900.f, -100.f);
    enemyCount++;
}

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

    for (int i = 0; i < lvlHeight; i++)
        delete[] lvl[i];
    delete[] lvl;

    delete levelManager;
    levelManager = nullptr;
}

void PlayState::handleInput(sf::RenderWindow& window)
{
    // Z key switches active player
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
    for (int i = 0; i < playerCount; i++)
        players[i]->update(dt, lvl, cellSize, lvlWidth, lvlHeight);

    for (int i = 0; i < enemyCount; i++)
    {
        enemies[i]->setTarget(players[activePlayer]->getSprite().getPosition());
        enemies[i]->update(dt, lvl, cellSize, lvlWidth, lvlHeight);
    }

   
    for (int i = 0; i < enemyCount; i++)
    {
        Enemies* replacement = enemies[i]->getLandedReplacement();
        if (replacement != nullptr)
        {
            delete enemies[i];
            enemies[i] = replacement;
        }
    }


    levelManager->update(
        players[activePlayer]->getSprite().getPosition().x,
        players[activePlayer]->getSprite().getPosition().y
    );
}

void PlayState::draw(sf::RenderWindow& window)
{
    levelManager->draw(window);

    for (int i = 0; i < playerCount; i++)
    {
        window.draw(players[i]->getSprite());
        if (players[i]->isWeaponLoaded())
            window.draw(players[i]->getWeaponSprite());
    }

    for (int i = 0; i < enemyCount; i++)
        window.draw(enemies[i]->getSprite());
}

void PlayState::enter() {}
void PlayState::exit() {}