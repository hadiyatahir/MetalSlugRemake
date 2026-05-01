// LevelManager.cpp
#include "LevelManager.h"

LevelManager::LevelManager()
    : currentIndex(-1), totalLevels(4)
{
    // Create all levels — dimensions from your project spec
    // cell_size = 64, level 1 uses your existing 110 wide x 14 tall
    levels[0] = new Level(110, 14, 64);   // Level 1 — your current grid
    levels[1] = new Level(160, 20, 64);   // Level 2
    levels[2] = new Level(200, 25, 64);   // Level 3
    levels[3] = new Level(220, 30, 64);   // Boss level
}

LevelManager::~LevelManager()
{
    // Unload current level before destroying
    if (currentIndex >= 0)
        levels[currentIndex]->unload();

    for (int i = 0; i < totalLevels; i++)
        delete levels[i];
}

void LevelManager::loadLevel(int index)
{
    if (index < 0 || index >= totalLevels) return;

    // Unload current
    if (currentIndex >= 0)
        levels[currentIndex]->unload();

    currentIndex = index;
    levels[currentIndex]->load();
}

void LevelManager::nextLevel()
{
    loadLevel(currentIndex + 1);
}

Level* LevelManager::getCurrentLevel()
{
    if (currentIndex < 0) return nullptr;
    return levels[currentIndex];
}

bool LevelManager::isLastLevel() const
{
    return currentIndex >= totalLevels - 1;
}

void LevelManager::update(float dt)
{
    if (currentIndex >= 0)
        levels[currentIndex]->update(dt);
}

void LevelManager::render(sf::RenderWindow& window)
{
    if (currentIndex >= 0)
        levels[currentIndex]->render(window);
}