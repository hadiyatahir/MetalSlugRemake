#pragma once
// Tile.h
#pragma once

enum class TileType
{
    AIR,
    GROUND,
    BEDROCK    // indestructible bottom row
};

struct Tile
{
    TileType type;
    bool     isSolid;
    bool     isDestructible;

    Tile() : type(TileType::AIR), isSolid(false), isDestructible(false) {}

    Tile(TileType t)
    {
        type = t;
        switch (t)
        {
        case TileType::GROUND:
            isSolid = true;
            isDestructible = true;
            break;
        case TileType::BEDROCK:
            isSolid = true;
            isDestructible = false;
            break;
        default:
            isSolid = false;
            isDestructible = false;
            break;
        }
    }
};