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

void LevelManager::loadLevel(int biomeId)
{
    //delete level;
    //level = new level(110, 14, 64);
    //level->loadtextures();

   

    //// a ground floor at row 11
    //for (int col = 0; col < 110; col++)
    //    level->settile(11, col, 'g');

    //// a small raised platform
    //level->settile(9, 20, 'g');
   // //level->settile(9, 21, 'g');
    //level->settile(9, 22, 'g');
    
        delete level;
        level = new Level(150, 50, 64);
        //level->loadTextures();
        level->setBiome(biomeId);

        // aerial biome — cols 0-36, high ground around row 4
        //for (int col = 0; col < 37; col++)
        //    level->setTile(4, col, 'g');

        //// plains biome — cols 37-73, ground at row 11
        //for (int col = 37; col < 74; col++)
        //    level->setTile(11, col, 'g');

        //// aquatic biome — cols 74-110, sea level row 8, water below
        //for (int col = 74; col < 110; col++)
        //{
        //    level->setTile(8, col, 'g');   // sea floor
        //    for (int row = 9; row < 14; row++)
        //        level->setTile(row, col, 'w');  // water
        //}
 

        //    for (int col = 0; col < 150; col++)
        //        level->setTile(10, col, 'g');


        if (biomeId == 0) {

            for (int col = 0; col < 43; col++)
            {
                level->setTile(12, col, 'g');
                //level->setTile(13, col, 'g');
            }
            for (int col = 42; col < 150; col++)
            {
                level->setTile(14, col, 'g');
                //level->setTile(13, col, 'g');
            }//this is valid


        /*    for (int col = 8; col < 15; col++)
                level->setTile(17, col, 'g');

            for (int col = 15; col < 20; col++)
                level->setTile(16, col, 'g');

            for (int col = 32; col < 38; col++)
                level->setTile(17, col, 'g');

            for (int col = 38; col < 42; col++)
                level->setTile(16, col, 'g');

            for (int col = 12; col < 18; col++)
                level->setTile(12, col, 'g');

            for (int col = 25; col < 33; col++)
                level->setTile(10, col, 'g');

            for (int col = 45; col < 52; col++)
                level->setTile(13, col, 'g');

            for (int col = 60; col < 68; col++)
                level->setTile(9, col, 'g');

            for (int col = 80; col < 88; col++)
                level->setTile(14, col, 'g');*/
        }


        if (biomeId == 1) {
            for (int col = 0; col < 8; col++)
            {
                level->setTile(17, col, 'g');
                //level->setTile(13, col, 'g');
            }
            for (int col = 7; col < 15; col++)
            {
                level->setTile(16, col, 'g');
                //level->setTile(13, col, 'g');
            }
            for (int col = 12; col < 15; col++)
            {
                level->setTile(15, col, 'g');
                //level->setTile(13, col, 'g');
            }
            for (int col = 15; col < 20; col++)
            {
                level->setTile(14, col, 'g');
                //level->setTile(13, col, 'g');
            }
            for (int col = 20; col < 23; col++)
            {
                level->setTile(13, col, 'g');
                //level->setTile(13, col, 'g');
            }
            for (int col = 23; col < 32; col++)
            {
                level->setTile(12, col, 'g');
                //level->setTile(13, col, 'g');
            }
            for (int col = 32; col < 33; col++)
            {
                level->setTile(11, col, 'g');
                //level->setTile(13, col, 'g');
            }
            for (int col = 33; col < 34; col++)
            {
                level->setTile(10, col, 'g');
                //level->setTile(13, col, 'g');
            }
            for (int col = 34; col < 35; col++)
            {
                level->setTile(9, col, 'g');
                //level->setTile(13, col, 'g');
            }
            for (int col = 35; col < 36; col++)
            {
                level->setTile(8, col, 'g');
                //level->setTile(13, col, 'g');
            }
            for (int col = 36; col < 37; col++)
            {
                level->setTile(8, col, 'g');
                //level->setTile(13, col, 'g');
            }
            for (int col = 37; col < 38; col++)
            {
                level->setTile(8, col, 'g');
                //level->setTile(13, col, 'g');
            }
            for (int col = 36; col < 55; col++)
            {
                level->setTile(16, col, 'g');
                //level->setTile(13, col, 'g');
            }
            for (int col = 39; col < 45; col++)
            {
                level->setTile(15, col, 'g');
                //level->setTile(13, col, 'g');
            }
            for (int col = 45; col < 55; col++)
            {
                level->setTile(14, col, 'g');
                //level->setTile(13, col, 'g');
            }
            for (int col = 47; col < 55; col++)
            {
                level->setTile(13, col, 'g');
                //level->setTile(13, col, 'g');
            }
            for (int col = 55; col < 67; col++)
            {
                level->setTile(12, col, 'g');
                //level->setTile(13, col, 'g');
            }
            for (int col = 67; col < 68; col++)
            {
                level->setTile(13, col, 'g');
                //level->setTile(13, col, 'g');
            }
            for (int col = 68; col < 69; col++)
            {
                level->setTile(14, col, 'g');
                //level->setTile(13, col, 'g');
            }
            for (int col = 69; col < 70; col++)
            {
                level->setTile(15, col, 'g');
                //level->setTile(13, col, 'g');
            }
            for (int col = 70; col < 71; col++)
            {
                level->setTile(16, col, 'g');
                //level->setTile(13, col, 'g');
            }

            for (int col = 71; col < 72; col++)
            {
                level->setTile(17, col, 'g');
                //level->setTile(13, col, 'g');
            }
            for (int col = 72; col < 100; col++)
            {
                level->setTile(18, col, 'g');
                //level->setTile(13, col, 'g');
            }

        }


        if (biomeId == 2) {

        }
       

 

}

void LevelManager::update(float playerWorldX, float playerWorldY)
{
   /* if (level == nullptr) return;

    int desiredX = (int)playerWorldX - screenWidth / 2;
    int desiredY = (int)playerWorldY - screenHeight / 4;

    int maxCamX = level->getPixelWidth() - screenWidth;
    //int maxCamY = level->getPixelHeight() - screenHeight;

    // safety check — if level is narrower than screen don't scroll
    if (maxCamX < 0) maxCamX = 0;
   // if (maxCamY < 0) maxCamY = 0;

    if (desiredX < 0)       desiredX = 0;
    if (desiredX > maxCamX) desiredX = maxCamX;

    if (desiredY < 0)       desiredY = 0;
    //if (desiredY > maxCamY) desiredY = maxCamY;

    int minCamY = 0;    // locked near row 14
    int maxCamY = 100; // only 50px of drift allowed

    if (desiredY < minCamY) desiredY = minCamY;
    if (desiredY > maxCamY) desiredY = maxCamY;

   // camY = desiredY;

    camX = desiredX;
    camY = desiredY;*/

    if (level == nullptr) return;

    int desiredX = (int)playerWorldX - screenWidth / 2;
    int desiredY = (int)playerWorldY - screenHeight / 2;

    int maxCamX = level->getPixelWidth() - screenWidth;

    if (level->getBiome() == 1)
    {
        
        if (desiredX < 0)       desiredX = 0;
        if (desiredX > maxCamX) desiredX = maxCamX;
        int minCamY = 0;
        int maxCamY = level->getPixelHeight() - screenHeight;
        if (maxCamY < 0) maxCamY = 0;
        if (desiredY < minCamY) desiredY = minCamY;
        if (desiredY > maxCamY) desiredY = maxCamY;
    }
    else
    {
        if (desiredX < 0)       desiredX = 0;
        if (desiredX > maxCamX) desiredX = maxCamX;
        int minCamY = 0;
        int maxCamY = 100; // original locked behaviour for other biomes
        if (desiredY < minCamY) desiredY = minCamY;
        if (desiredY > maxCamY) desiredY = maxCamY;
    }

    camX = desiredX;  // ← was missing
    camY = desiredY;
}

/*
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
}*/

void LevelManager::draw(sf::RenderWindow& window)
{
    if (level == nullptr) return;
    level->draw(window, camX, camY, screenWidth, screenHeight);
}
