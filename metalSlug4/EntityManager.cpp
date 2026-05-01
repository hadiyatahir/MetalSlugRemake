// EntityManager.cpp
#include "EntityManager.h"
#include "Level.h"

EntityManager::EntityManager()
    : players(nullptr),
    playerCount(4),
    activePlayerIndex(0),
    currentLevel(nullptr)
{
}

EntityManager::~EntityManager()
{
    clear();
}

void EntityManager::init(Level* level)
{
    currentLevel = level;

    // ── This replaces your main() player setup ────────────────
    // Your code:
    //   PlayerSoldier* players = new PlayerSoldier[4]{0,1,2,3};
    //   players[0].setPosition(600, 500); ...

    players = new PlayerSoldier[playerCount]{
        PlayerSoldier(0),
        PlayerSoldier(1),
        PlayerSoldier(2),
        PlayerSoldier(3)
    };

    // Use level's defined start position
    sf::Vector2f startPos = level->getPlayerStartPosition();
    players[0].setPosition(startPos.x, startPos.y);
    players[1].setPosition(startPos.x + 50.f, startPos.y);
    players[2].setPosition(startPos.x + 100.f, startPos.y);
    players[3].setPosition(startPos.x + 150.f, startPos.y);
}

void EntityManager::setLevel(Level* level)
{
    currentLevel = level;
}

void EntityManager::handleEvent(sf::Event& ev)
{
    // Z key switches active player — per project spec
    if (ev.type == sf::Event::KeyPressed)
    {
        if (ev.key.code == sf::Keyboard::Z)
        {
            int tries = 0;
            do {
                activePlayerIndex = (activePlayerIndex + 1) % playerCount;
                tries++;
            } while (players[activePlayerIndex].isDead()
                && tries < playerCount);
        }
    }
}

void EntityManager::update(float dt)
{
    if (!players || !currentLevel) return;

    // ── This replaces your main() update loop ─────────────────
    // Your code:
    //   for(int i=0;i<4;i++)
    //     players[i].update(dt, lvl, cell_size, width, height);

    // Get the raw grid for backward compat with your Soldier.h
    // Once Soldier uses Level::isSolid() this goes away
    char** raw = currentLevel->getRawGrid();

    for (int i = 0; i < playerCount; i++)
    {
        // Pass which player is active so only that one reads input
        players[i].setActive(i == activePlayerIndex);
        players[i].update(dt, raw,
            currentLevel->getCellSize(),
            currentLevel->getWidth(),
            currentLevel->getHeight());
    }

    // Free the temporary raw grid
    if (raw)
    {
        for (int i = 0; i < currentLevel->getHeight(); i++)
            delete[] raw[i];
        delete[] raw;
    }
}

void EntityManager::render(sf::RenderWindow& window)
{
    if (!players) return;

    // ── This replaces your main() draw loop ───────────────────
    // Your code:
    //   for(int i=0;i<4;i++) window.draw(players[i].getSprite());

    for (int i = 0; i < playerCount; i++)
        window.draw(players[i].getSprite());
}

bool EntityManager::allPlayersDead() const
{
    if (!players) return false;
    for (int i = 0; i < playerCount; i++)
        if (!players[i].isDead()) return false;
    return true;
}

void EntityManager::clear()
{
    delete[] players;
    players = nullptr;
}