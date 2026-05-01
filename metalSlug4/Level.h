#pragma once
#ifndef LEVEL_H
#define LEVEL_H

class Level {
    char** grid;
    int width, height, cellSize;
    sf::Texture wallTex;
    sf::Sprite wallSprite;
public:
    Level(int w, int h, int cellSize);
    ~Level();
    void draw(sf::RenderWindow& window);
    char getCell(int row, int col) const;
    void setCell(int row, int col, char val);
    int getWidth() const;
    int getHeight() const;
    int getCellSize() const;
};



#endif // !LEVEL_H
