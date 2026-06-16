#pragma once
#include <SFML/Graphics.hpp>





class Level
{
    char** grid;    
    int width;   
    int height;    
    int cellSize;  
    int biomeId;

    sf::Texture aerialBgTex;
    sf::Texture plainsBgTex;
    sf::Texture aquaticBgTex;
    sf::Texture currentBackgroundTexture;
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

    int getPixelWidth()  const { 
        return width * cellSize;
    }

    int getPixelHeight() const {
        return height * cellSize;
    }

    int getCellSize() const {
        return cellSize;
    }

    int getWidth() const {
        return width;
    }

    int getHeight() const {
        return height;
    }

    char** getGrid() const {
        return grid;
    }

    int getBiome() const { return biomeId; }

    sf::Vector2u getBgSize() const { return currentBackgroundTexture.getSize(); }

    void drawBackground(sf::RenderWindow& window, int camX, int camY, int screenW, int screenH);
    void setBiome(int biome);

    void loadTextures();
};
