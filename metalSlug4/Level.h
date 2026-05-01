//#pragma once
//#ifndef LEVEL_H
//#define LEVEL_H
//
//class Level {
//    char** grid;
//    int width, height, cellSize;
//    sf::Texture wallTex;
//    sf::Sprite wallSprite;
//public:
//    Level(int w, int h, int cellSize);
//    ~Level();
//    void draw(sf::RenderWindow& window);
//    char getCell(int row, int col) const;
//    void setCell(int row, int col, char val);
//    int getWidth() const;
//    int getHeight() const;
//    int getCellSize() const;
//};
//
//
//
//#endif // !LEVEL_H


// Level.h
#pragma once
#include <SFML/Graphics.hpp>
#include "Tile.h"

class Level
{
private:
    // ── Grid — this is your char** lvl, now properly typed ────
    Tile** tileGrid;
    int     width;
    int     height;
    int     cellSize;

    // ── Textures for tiles ────────────────────────────────────
    // Animation system replaces this later — for now we own it
    sf::Texture grassTexture;
    sf::Sprite  tileSprite;

    // ── Camera / scroll ───────────────────────────────────────
    float cameraX;   // how far we've scrolled horizontally

public:
    Level(int width, int height, int cellSize);
    ~Level();

    void load();    // builds the grid, loads textures
    void unload();  // frees grid memory

    void update(float dt);
    void render(sf::RenderWindow& window);

    // ── Tile queries — EntityManager / Soldier call these ─────
    bool isSolid(int tileX, int tileY)   const;
    bool isInBounds(int tileX, int tileY) const;

    // ── For backward compatibility with your Soldier.h ────────
    // Your Soldier::update takes char** lvl — we provide that view
    // Eventually Soldier will call isSolid() directly instead
    char getTileChar(int tileX, int tileY) const;
    char** getRawGrid() const;  // temporary bridge — remove later

    // ── Getters ───────────────────────────────────────────────
    int getWidth()    const { return width; }
    int getHeight()   const { return height; }
    int getCellSize() const { return cellSize; }

    sf::Vector2f getPlayerStartPosition() const;
};
