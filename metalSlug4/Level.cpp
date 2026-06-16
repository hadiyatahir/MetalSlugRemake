#include "Level.h"

Level::Level(int w, int h, int cs)
    : width(w), height(h), cellSize(cs), grid(nullptr)
{

    loadTextures();
    
    grid = new char* [height];

    
    for (int row = 0; row < height; row++)
    {
        grid[row] = new char[width];
        for (int col = 0; col < width; col++)
            grid[row][col] = '\0';
    }
}

Level::~Level()  //opp of constructor
{
    if (grid == nullptr) return;

    for (int row = 0; row < height; row++)
    {
        delete[] grid[row];
        grid[row] = nullptr;
    }
    delete[] grid;
    grid = nullptr;
}

void Level::loadTextures()
{
    grassTex.loadFromFile("Sprites/blocks/grass.png");
    //waterTex.loadFromFile("Sprites/blocks/water.png");
   stoneTex.loadFromFile("Sprites/blocks/grass_block_side.png");
    plainsBgTex.loadFromFile("Sprites/Biomes/plainssss.png");
    aerialBgTex.loadFromFile("Sprites/Biomes/aerialbiome.png");
    aquaticBgTex.loadFromFile("Sprites/Biomes/aquatic.png");
}

char Level::getTile(int row, int col) const
{
    if (row < 0 || row >= height || col < 0 || col >= width)
        return '\0';  // treat out-of-bounds as empty
    return grid[row][col];
}

void Level::setTile(int row, int col, char tile)
{
    if (row < 0 || row >= height || col < 0 || col >= width)
        return;  
    grid[row][col] = tile;
}


void Level::draw(sf::RenderWindow& window,
    int camX, int camY,
    int screenW, int screenH)
{
    // Calculate visible tile range — integer division gives us indices
    int startCol = camX / cellSize;
    int endCol = (camX + screenW) / cellSize + 1;

    int startRow = camY / cellSize;
    int endRow = (camY + screenH) / cellSize + 1;

    // Clamp to valid grid bounds
    if (startCol < 0)      startCol = 0;
    if (endCol >= width) endCol = width - 1;
    if (startRow < 0)      startRow = 0;
    if (endRow >= height)endRow = height - 1;

    for (int row = startRow; row <= endRow; row++)
    {
        for (int col = startCol; col <= endCol; col++)
        {
            char tile = grid[row][col];
            if (tile == '\0') continue; // air — skip

            // Pick which texture to use for this tile type
            if (tile == 'g') tileSprite.setTexture(grassTex);
            else if (tile == 'w') tileSprite.setTexture(waterTex);
            else if (tile == 's') tileSprite.setTexture(stoneTex);
            else continue; // unknown tile, skip

            // Convert world position to screen position
            float screenX = (float)(col * cellSize);
            float screenY = (float)(row * cellSize);
            if (biomeId == 0) {
                screenX = screenX;
                screenY = screenY;
            }

            else if (biomeId == 1) {
                screenX -= camX;
                screenY -= camY;
            }

            tileSprite.setPosition(screenX, screenY);
            window.draw(tileSprite);
        }
    }
}




bool Level::isSolid(float worldX, float worldY) const
{
    int col = (int)(worldX / cellSize);
    int row = (int)(worldY / cellSize);

    char tile = getTile(row, col);

    return (tile == 'g' || tile == 's');



}



//void Level::drawBackground(sf::RenderWindow& window, int camX, int camY, int screenW, int screenH)
//{
//    //int centerCol = (camX + screenW / 2) / cellSize;
//    //if (centerCol < 0)      centerCol = 0;
//    //if (centerCol >= width) centerCol = width - 1;
//
//    //// adjust column ranges to match your biome layout
//    //if (centerCol < 37)
//    //    bgSprite.setTexture(plainsBgTex);
//    //else if (centerCol < 74)
//    //    bgSprite.setTexture(aerialBgTex);
//    //else
//    //    bgSprite.setTexture(aquaticBgTex);
//    //if (centerCol < 37)
//    //{
//    //   /* bgSprite.setTexture(plainsBgTex);
//    //    float scaleX = (float)screenW / plainsBgTex.getSize().x;
//    //    float scaleY = (float)screenH / plainsBgTex.getSize().y;
//    //    bgSprite.setScale(scaleX, scaleY);*/
//
//    //    bgSprite.setTexture(plainsBgTex);
//
//    //    bgSprite.setTextureRect(
//    //        sf::IntRect(camX, 0, screenW, screenH)
//    //    );
//
//    //    bgSprite.setPosition(0.f, 0.f);
//
//    //    window.draw(bgSprite);
//
//
//    //}
//    //else if (centerCol < 74)
//    //{
//    //   /* bgSprite.setTexture(aerialBgTex);
//    //    float scaleX = (float)screenW / aerialBgTex.getSize().x;
//    //    float scaleY = (float)screenH / aerialBgTex.getSize().y;
//    //    bgSprite.setScale(scaleX, scaleY);*/
//    //    bgSprite.setTexture(plainsBgTex);
//
//    //    int bgWidth = plainsBgTex.getSize().x;
//
//    //    int bgCamX = camX;
//
//    //    if (bgCamX < 0)
//    //        bgCamX = 0;
//
//    //    if (bgCamX > bgWidth - screenW)
//    //        bgCamX = bgWidth - screenW;
//
//
//    //    bgSprite.setTextureRect(
//    //        sf::IntRect(bgCamX, 0, screenW, screenH)
//    //    );
//
//    //    bgSprite.setPosition(0.f, 0.f);
//
//    //    window.draw(bgSprite);
//
//    //}
//    //else
//    //{
//    //   /* bgSprite.setTexture(aquaticBgTex);
//    //    float scaleX = (float)screenW / aquaticBgTex.getSize().x;
//    //    float scaleY = (float)screenH / aquaticBgTex.getSize().y;
//    //    bgSprite.setScale(scaleX, scaleY);*/
//    //    bgSprite.setTexture(plainsBgTex);
//
//    //    bgSprite.setTextureRect(
//    //        sf::IntRect(camX, 0, screenW, screenH)
//    //    );
//
//    //    bgSprite.setPosition(0.f, 0.f);
//
//    //    window.draw(bgSprite);
//
//
//    int centerCol = (camX + screenW / 2) / cellSize;
//
//    if (centerCol < 0) centerCol = 0;
//    if (centerCol >= width) centerCol = width - 1;
//
//    // choose texture
//    if (centerCol < 37)
//        bgSprite.setTexture(plainsBgTex);
//    else if (centerCol < 74)
//        bgSprite.setTexture(aerialBgTex);
//    else
//        bgSprite.setTexture(aquaticBgTex);
//
//    // MANUAL texture size (NO Vector2u)
//    int texWidth = bgSprite.getTexture()->getSize().x;
//
//    int bgCamX = camX;
//
//    // clamp camera so we don't go outside texture
//    int maxX = texWidth - screenW;
//    if (maxX < 0)
//        maxX = 0;
//
//    if (bgCamX < 0)
//        bgCamX = 0;
//
//    if (bgCamX > maxX)
//        bgCamX = maxX;
//
//    bgSprite.setTextureRect(sf::IntRect(bgCamX, 0, screenW, screenH));
//
//    bgSprite.setPosition(0.f, 0.f);
//
//    window.draw(bgSprite);
//
//    
//
//   /* bgSprite.setPosition(0.f, 0.f);
//    window.draw(bgSprite);*/
//}




void Level::drawBackground(sf::RenderWindow& window, int camX, int camY, int screenW, int screenH)
{
    //int centerCol = (camX + screenW / 2) / cellSize;

    //if (centerCol < 0) centerCol = 0;
    //if (centerCol >= width) centerCol = width - 1;

    //sf::Texture* tex;

    //if (centerCol < 37)
    //    tex = nullptr;
    //else if (centerCol < 74)
    //    tex = nullptr;
    //else
    //    tex = nullptr;

    //// we avoid pointers completely → direct selection
    //if (centerCol < 37)
    //    bgSprite.setTexture(plainsBgTex);
    //else if (centerCol < 74)
    //    bgSprite.setTexture(aerialBgTex);
    //else
    //    bgSprite.setTexture(aquaticBgTex);

    //int texWidth = bgSprite.getTexture()->getSize().x;

    //int offsetX = camX % texWidth;

    //// first draw
    //bgSprite.setTextureRect(sf::IntRect(offsetX, 0, screenW, screenH));
    //bgSprite.setPosition(0.f, 0.f);
    //window.draw(bgSprite);

    //// wrap draw
    //bgSprite.setTextureRect(sf::IntRect(0, 0, screenW, screenH));
    //bgSprite.setPosition((float)(texWidth - offsetX), 0.f);
    //window.draw(bgSprite);


   /* bgSprite.setTexture(currentBackgroundTexture);

    int texWidth = currentBackgroundTexture.getSize().x;

    int offsetX = camX;

    bgSprite.setTextureRect(sf::IntRect(offsetX, 0, screenW, screenH));
    bgSprite.setPosition(0.f, 0.f);

    window.draw(bgSprite);*/

    //if (biomeId == 0)
    //    bgSprite.setTexture(plainsBgTex);

    //else if (biomeId == 1)
    //    bgSprite.setTexture(aerialBgTex);

    //else
    //    bgSprite.setTexture(aquaticBgTex);




    //bgSprite.setPosition(-camX, -camY);

  
    //window.draw(bgSprite);
    if (biomeId == 0) {
        bgSprite.setTexture(plainsBgTex);

        bgSprite.setScale(4.15f, 4.15f);

        bgSprite.setPosition(-camX, -490);
        
       
    }
    else if (biomeId == 1) {
        bgSprite.setTexture(aerialBgTex);
        bgSprite.setScale(3.25f, 3.25f);
        bgSprite.setPosition(-camX, -camY + 5);
    }
    else {
        bgSprite.setTexture(aquaticBgTex);
        bgSprite.setScale(1.25, 1.25);
        bgSprite.setPosition(-camX - 50, -camY - 90);
    }
   

    window.draw(bgSprite);
     //window.draw(bgSprite);



}



void Level::setBiome(int biome)
{
    biomeId = biome;

    if (biome == 0)
        currentBackgroundTexture = plainsBgTex;
    else if (biome == 1)
        currentBackgroundTexture = aerialBgTex;
    else
        currentBackgroundTexture = aquaticBgTex;

   // bgSprite.setTexture(currentBackgroundTexture);
   // window.draw(bgSprite);
}