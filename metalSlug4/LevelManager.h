// LevelManager.h
#pragma once
#include "Level.h"

class LevelManager
{
private:
    Level* levels[4];    // 4 levels: 3 survival + 1 boss
    int     currentIndex;
    int     totalLevels;

public:
    LevelManager();
    ~LevelManager();

    void loadLevel(int index);
    void nextLevel();

    Level* getCurrentLevel();
    bool   isLastLevel() const;

    void update(float dt);
    void render(sf::RenderWindow& window);
};