#pragma once
#include <SFML/Graphics.hpp>
#include "Animation.h"

using namespace sf;

// ---------------- ANIMATION TYPES ----------------

enum AnimType
{
    IDLE,
    WALK,
    // SHOOT_IDLE,
    // SHOOT_RUN,
    JUMP_IDLE,
    JUMP_RUN,
    DIE,
    SWIM_IDLE,
    SWIM,
    ANIM_COUNT
};

// ---------------- SOLDIER BASE ----------------

class Soldier
{
public:
    Soldier(float spd, int hp);

    virtual void update(float dt, char** lvl, int cell_size, int mapWidth, int mapHeight);
    //virtual void update(float dt, char** lvl, int cell_size);
    //virtual void update(float dt);

    Sprite& getSprite();

protected:
    Sprite msprite;
    Texture mtexture;

    Animation manimations[ANIM_COUNT];
    AnimType mcurrentAnim;

    float speed;
    int health;

    void setAnimation(AnimType type);

protected:
    float velocityX = 0.f;
    float velocityY = 0.f;

    float gravity = 0.5f;
    float jumpStrength = -12.f;

    bool onGround = false;
    bool isJumping = false;
    bool facingRight = true;
    float collisionWidth = 60.f;
    float collisionHeight = 90.f;
    float collisionOffsetX = 0.f;
    float collisionOffsetY = 0.f;
    float baseScale = 1.f;
    //float offset = collisionWidth - 10.f;
    //bool prevFacingRight = true;

public:
    void setPosition(float x, float y);
    float coyoteTime = 0.f;
    float coyoteMax = 0.1f; // seconds — tune to taste (0.08–0.12 is typical)


};



class PlayerSoldier : public Soldier
{
public:

    PlayerSoldier(int index);
    void update(float dt, char** lvl, int cell_size, int mapWidth, int mapHeight) override;
    //void update(float dt, char** lvl, int cell_size) override;
    //void update(float dt) override;

private:
    int mplayerIndex;

    // ---------------- INPUT ----------------
    void handleInput(float dt);

    // ---------------- LOAD PLAYER DATA ----------------
    void loadPlayerData();

    // ---------------- ANIMATIONS ----------------
    void setupPlayer1Animations();
    void setupPlayer2Animations();
    void setupPlayer3Animations();
    void setupPlayer4Animations();
};