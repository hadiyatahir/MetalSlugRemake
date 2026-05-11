#pragma once
#include "Level.h"
#include <SFML/Graphics.hpp>

// ============================================================
// LevelManager.h
// ============================================================
// Owns the Level and controls the camera (scroll offset).
//
// The camera is just two integers: camX and camY.
// They represent how many pixels the view has scrolled
// from the top-left corner of the level.
//
// "Follow the player" means:
//   We want the player to stay horizontally centered.
//   Desired camX = playerX - screenWidth/2
//   But we clamp: camX can't be < 0 (left wall)
//   and camX + screenWidth can't exceed level pixel width.
// ============================================================

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

    // Create and populate the level grid
    // Call this once at the start of PlayState
    void loadLevel(int biomeId);

    // Called every frame with the player's current world position
    // Moves camX/camY to follow the player, clamped to level bounds
    void update(float playerWorldX, float playerWorldY);

    void draw(sf::RenderWindow& window);

    // EntityManager and collision checks need access to the level
    Level* getLevel() { return level; }

    int getCamX() const { return camX; }
    int getCamY() const { return camY; }
};
