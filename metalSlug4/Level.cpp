#include "Level.h"

Level::Level(int w, int h, int cs)
    : width(w), height(h), cellSize(cs), grid(nullptr)
{
    //allocating the array of row pointers
    grid = new char* [height];

    //allocating each row then filling it wiht null char
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
    grassTex.loadFromFile("Sprites/blocks/grass_block_side.png");
    waterTex.loadFromFile("Sprites/blocks/water.png");
    stoneTex.loadFromFile("Sprites/blocks/stone.png");
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
        return;  // silently ignore out-of-bounds writes
    grid[row][col] = tile;
}

// ============================================================
// draw
// ============================================================
// The core optimization: only draw tiles that the camera
// can actually see.
//
// camX / camY = world-pixel position of the camera's top-left.
// (e.g. camX=192 means the camera has scrolled 192px right)
//
// We convert from pixel to tile index by dividing by cellSize.
// startCol = first tile column that could be visible
// endCol   = last tile column that could be visible (+2 buffer)
//
// The tile's SCREEN position = (tile world pos) - (camera pos)
//   screen_x = col * cellSize - camX
//   screen_y = row * cellSize - camY
// ============================================================
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
            float screenX = (float)(col * cellSize - camX);
            float screenY = (float)(row * cellSize - camY);

            tileSprite.setPosition(screenX, screenY);
            window.draw(tileSprite);
        }
    }
}

// ============================================================
// isSolid
// ============================================================
// Convert a world-pixel coordinate to a tile index,
// then check if that tile is a solid block.
// Used by Entity physics: "can I move here?"
// ============================================================
bool Level::isSolid(float worldX, float worldY) const
{
    int col = (int)(worldX / cellSize);
    int row = (int)(worldY / cellSize);

    char tile = getTile(row, col);

    // Add more solid tiles here as you create them
    return (tile == 'g' || tile == 's');
}
