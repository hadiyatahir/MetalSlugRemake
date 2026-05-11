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
        level = new Level(150, 40, 64);
        level->loadTextures();

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
            for (int col = 0; col < 150; col++)
            {
                level->setTile(18, col, 'g');
                level->setTile(19, col, 'g');
            }

            for (int col = 8; col < 15; col++)
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
                level->setTile(14, col, 'g');
        }


        if (biomeId == 1) {
            for (int col = 0; col < 150; col++)
            {
                level->setTile(18, col, 's');
                level->setTile(19, col, 's');
            }

            for (int col = 8; col < 15; col++)
                level->setTile(17, col, 's');

            for (int col = 15; col < 20; col++)
                level->setTile(16, col, 's');

            for (int col = 32; col < 38; col++)
                level->setTile(17, col, 's');

            for (int col = 38; col < 42; col++)
                level->setTile(16, col, 's');

            for (int col = 12; col < 18; col++)
                level->setTile(12, col, 's');

            for (int col = 25; col < 33; col++)
                level->setTile(10, col, 's');

            for (int col = 45; col < 52; col++)
                level->setTile(13, col, 's');

            for (int col = 60; col < 68; col++)
                level->setTile(9, col, 's');

            for (int col = 80; col < 88; col++)
                level->setTile(14, col, 's');




           /* for (int col = 0; col < 150; col++)
            {
                level->setTile(18, col, 'g');
                level->setTile(19, col, 'g');
            }

            for (int col = 8; col < 15; col++)
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


        if (biomeId == 2) {

        }
       

 

}





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
