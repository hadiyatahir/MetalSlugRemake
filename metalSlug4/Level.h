#pragma once
#include <SFML/Graphics.hpp>



//owns the char** tile grid for one level

class Level
{
    char** grid;      // [row][col] — row = y, col = x
    int    width;     //in tiles (columns)
    int    height;    //in tiles (rows)
    int    cellSize;  //pixels per tile (64)

    // add these members
    sf::Texture aerialBgTex;
    sf::Texture plainsBgTex;
    sf::Texture aquaticBgTex;
    sf::Sprite  bgSprite;

    sf::Texture grassTex;
    sf::Texture waterTex;
    sf::Texture stoneTex;
    sf::Sprite  tileSprite;  //same tile spirte reused for each tile

public:
    Level(int width, int height, int cellSize);
    ~Level();

    char getTile(int row, int col) const;
    void setTile(int row, int col, char tile);

    void draw(sf::RenderWindow& window, int camX, int camY, int screenW, int screenH);  //rendering tiles within the camera rect

    bool isSolid(float worldX, float worldY) const;   //collision helper function

    int getPixelWidth()  const { return width * cellSize; }
    int getPixelHeight() const { return height * cellSize; }
    int getCellSize()    const { return cellSize; }
    int getWidth()       const { return width; }
    int getHeight()      const { return height; }


    // add this method declaration
    void drawBackground(sf::RenderWindow& window, int camX, int camY, int screenW, int screenH);

    void loadTextures();
};
