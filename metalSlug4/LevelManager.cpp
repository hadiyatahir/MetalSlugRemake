#include "LevelManager.h"

LevelManager::LevelManager(int screenW, int screenH)
    : level(nullptr), camX(0), camY(0),
    screenWidth(screenW), screenHeight(screenH)
{
}

LevelManager::~LevelManager()
{
    delete level;
    level = nullptr;
}

// ============================================================
// loadLevel
// ============================================================
// Creates the Level and fills in the tile grid.
// RIGHT NOW this is hardcoded — later you'll load from a file
// or use procedural generation.
//
// Level size: 110 wide, 14 tall, 64px per tile
// (matches your original main.cpp)
// ============================================================
void LevelManager::loadLevel()
{
    // Delete old level if one exists
    delete level;
    level = new Level(110, 14, 64);
    level->loadTextures();

    // --------------------------------------------------------
    // Populate the grid
    // This is where your level design lives for now.
    // Each call sets one tile at (row, col).
    // --------------------------------------------------------

    // A ground floor at row 11
    for (int col = 0; col < 110; col++)
        level->setTile(11, col, 'g');

    // A small raised platform
    level->setTile(9, 20, 'g');
    level->setTile(9, 21, 'g');
    level->setTile(9, 22, 'g');
}

// ============================================================
// update — camera follow
// ============================================================
// We want the player centered horizontally on screen.
//
//   Ideal camX = playerX - screenWidth/2
//
// But we must clamp:
//   - camX >= 0            (can't see before the level starts)
//   - camX <= levelPixelW - screenWidth  (can't see past the end)
//
// Same logic vertically for camY.
// ============================================================
void LevelManager::update(float playerWorldX, float playerWorldY)
{
    if (level == nullptr) return;

    // Desired camera position (player centered)
    int desiredX = (int)playerWorldX - screenWidth / 2;
    int desiredY = (int)playerWorldY - screenHeight / 2;

    // Clamp horizontally
    int maxCamX = level->getPixelWidth() - screenWidth;
    if (desiredX < 0)        desiredX = 0;
    if (desiredX > maxCamX)  desiredX = maxCamX;
    if (maxCamX < 0)         desiredX = 0;   // level narrower than screen

    // Clamp vertically
    int maxCamY = level->getPixelHeight() - screenHeight;
    if (desiredY < 0)        desiredY = 0;
    if (desiredY > maxCamY)  desiredY = maxCamY;
    if (maxCamY < 0)         desiredY = 0;

    camX = desiredX;
    camY = desiredY;
}

void LevelManager::draw(sf::RenderWindow& window)
{
    if (level == nullptr) return;
    level->draw(window, camX, camY, screenWidth, screenHeight);
}
