#pragma once
#include "Level.h"
#include <SFML/Graphics.hpp>



class LevelManager
{
    Level* level;       //composition

    int camX;           // world pixels scrolled horizontally
    int camY;           // world pixels scrolled vertically

    int screenWidth;
    int screenHeight;

public:
    LevelManager(int screenW, int screenH);
    ~LevelManager();

    void loadLevel(int biomeId);

    void update(float playerWorldX, float playerWorldY);

    void draw(sf::RenderWindow& window);

    
    Level* getLevel() { 
        return level; 
    }

    int getCamX() const { 
        return camX;
    }

    int getCamY() const {
        return camY;
    }
};
