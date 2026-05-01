// Level.cpp
#include "Level.h"

Level::Level(int w, int h, int cs)
    : width(w), height(h), cellSize(cs),
    tileGrid(nullptr), cameraX(0.f)
{
}

Level::~Level()
{
    unload();
}

void Level::load()
{
    // ── Build tile grid ───────────────────────────────────────
    // This is your main() code:
    //   lvl = new char*[height];
    //   for(i) lvl[i] = new char[width]{'\0'};
    //   for(i<110) lvl[11][i] = 'g';
    // Translated to Tile system:

    tileGrid = new Tile * [height];
    for (int i = 0; i < height; i++)
        tileGrid[i] = new Tile[width];

    // Ground row — your lvl[11][i] = 'g'
    // Row 11 in a 14-high grid = height - 3
    int groundRow = height - 3;
    for (int x = 0; x < width; x++)
    {
        tileGrid[groundRow][x] = Tile(TileType::GROUND);
        tileGrid[groundRow + 1][x] = Tile(TileType::GROUND);
        tileGrid[groundRow + 2][x] = Tile(TileType::GROUND);
    }

    // Bottom row = BEDROCK — indestructible per project spec
    for (int x = 0; x < width; x++)
        tileGrid[height - 1][x] = Tile(TileType::BEDROCK);

    // ── Load textures ─────────────────────────────────────────
    // This is your main() code:
    //   wallTex1.loadFromFile("Sprites/blocks/grass_block_side.png");
    //   wallSprite1.setTexture(wallTex1);

    grassTexture.loadFromFile("Sprites/blocks/grass_block_side.png");
    tileSprite.setTexture(grassTexture);
}

void Level::unload()
{
    if (tileGrid)
    {
        for (int i = 0; i < height; i++)
            delete[] tileGrid[i];
        delete[] tileGrid;
        tileGrid = nullptr;
    }
}

void Level::update(float dt)
{
    // Rain, crater fill, animated tiles later
    // Empty for now — but the function MUST exist
    // so PlayState can call it without checking
}

void Level::render(sf::RenderWindow& window)
{
    // ── This is your display_level() function ─────────────────
    // Your original:
    //   for(i) for(j) if(lvl[i][j]=='g') { sprite.setPos; draw; }

    if (!tileGrid) return;

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            TileType t = tileGrid[i][j].type;
            if (t == TileType::AIR) continue;

            // Position in world space, offset by camera scroll
            float worldX = (float)(j * cellSize) - cameraX;
            float worldY = (float)(i * cellSize);

            // Only draw tiles visible on screen (optimization)
            if (worldX < -cellSize || worldX > 1600) continue;

            tileSprite.setPosition(worldX, worldY);
            window.draw(tileSprite);
        }
    }
}

bool Level::isInBounds(int x, int y) const
{
    return x >= 0 && x < width && y >= 0 && y < height;
}

bool Level::isSolid(int x, int y) const
{
    if (!isInBounds(x, y)) return true;  // treat edges as walls
    return tileGrid[y][x].isSolid;
}

char Level::getTileChar(int x, int y) const
{
    // Bridge for your existing Soldier.h which uses char comparison
    // Soldier calls: if(lvl[row][col] == 'g') ...
    // This gives that same answer without exposing the grid
    if (!isInBounds(x, y)) return '\0';
    switch (tileGrid[y][x].type)
    {
    case TileType::GROUND:  return 'g';
    case TileType::BEDROCK: return 'b';
    default:                return '\0';
    }
}

char** Level::getRawGrid() const
{
    // TEMPORARY — lets your existing Soldier.update(dt, lvl, ...)
    // keep working without changes right now
    // You will remove this once Soldier uses isSolid() instead
    if (!tileGrid) return nullptr;

    // Build a char** on the fly that mirrors the tile grid
    // This is called once per frame — acceptable for now
    char** raw = new char* [height];
    for (int i = 0; i < height; i++)
    {
        raw[i] = new char[width];
        for (int j = 0; j < width; j++)
            raw[i][j] = getTileChar(j, i);
    }
    return raw;
}

sf::Vector2f Level::getPlayerStartPosition() const
{
    // Row 11 * 64 = 704, minus player height ~94 = 610
    // Matches your original player_y = 610
    int groundRow = height - 3;
    return sf::Vector2f(380.f, (float)(groundRow * cellSize - 94));
}