/*#pragma once
#include <SFML/Graphics.hpp>
#include "Animation.h"
#include "Weapon.h"

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
    ANIM_COUNT,

};

// ---------------- SOLDIER BASE ----------------

class Soldier
{
public:
    Soldier(float spd, int hp);

    virtual void update(float dt, char** lvl, int cell_size, int mapWidth, int mapHeight);
    //virtual void update(float dt, char** lvl, int cell_size);
    //virtual void update(float dt);
public:
    virtual void equipWeapon(Weapon* w);

    Sprite& getSprite();

protected:
    Sprite msprite;
    Texture mtexture;

    Animation manimations[ANIM_COUNT];
    AnimType mcurrentAnim;

    float speed;
    int health;
    Weapon* m_weapon = nullptr;

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
public:
    void equipWeapon(Weapon* w) override;

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

#pragma once
#include <SFML/Graphics.hpp>
#include "Animation.h"
#include "Weapon.h"

using namespace sf;

// ---------------- BODY ANIMATION TYPES ----------------
// These drive the base-body spritesheet (Group 5/8/9/10.png)

enum AnimType
{
    IDLE,
    WALK,
    JUMP_IDLE,
    JUMP_RUN,
    DIE,
    SWIM_IDLE,
    SWIM,
    ANIM_COUNT
};

// ---------------- WEAPON ANIMATION TYPES ----------------
// These drive the weapon-overlay spritesheet (loaded per weapon/player).
// Layout mirrors AnimType so the same movement-state logic selects them.

enum WeaponAnimType
{
    W_IDLE,         // standing still, weapon holstered or aimed
    W_WALK,         // walking with weapon
    W_JUMP,         // airborne with weapon
    W_SHOOT_IDLE,   // firing while standing
    W_SHOOT_WALK,   // firing while walking
    W_SHOOT_JUMP,   // firing while airborne
    W_MELEE,        // melee swing (pistol butt, knife, etc.)
    W_ANIM_COUNT
};

// ---------------- SOLDIER BASE ----------------

class Soldier
{
public:
    Soldier(float spd, int hp);

    virtual void update(float dt, char** lvl, int cell_size, int mapWidth, int mapHeight);

    virtual void equipWeapon(Weapon* w);

    Sprite& getSprite();

    // Returns the weapon overlay sprite (position is kept in sync with body sprite)
    Sprite& getWeaponSprite();

protected:
    // ---- body ----
    Sprite   msprite;
    Texture  mtexture;
    Animation manimations[ANIM_COUNT];
    AnimType  mcurrentAnim;

    // ---- weapon overlay ----
    Sprite    mWeaponSprite;
    Texture   mWeaponTexture;
    Animation mWeaponAnims[W_ANIM_COUNT];
    WeaponAnimType mCurrentWeaponAnim;
    bool      mWeaponTextureLoaded;   // guard – no texture = no weapon sprite drawn

    void setAnimation(AnimType type);
    void setWeaponAnimation(WeaponAnimType type);

    // Update weapon sprite to match body position + facing
    void syncWeaponSprite();

    float speed;
    int   health;
    Weapon* m_weapon = nullptr;

protected:
    float velocityX = 0.f;
    float velocityY = 0.f;

    float gravity = 0.5f;
    float jumpStrength = -12.f;

    bool  onGround = false;
    bool  isJumping = false;
    bool  facingRight = true;

    float collisionWidth = 60.f;
    float collisionHeight = 90.f;
    float collisionOffsetX = 0.f;
    float collisionOffsetY = 0.f;
    float baseScale = 1.f;

public:
    void  setPosition(float x, float y);
    float coyoteTime = 0.f;
    float coyoteMax = 0.1f;
};


// ---------------- PLAYER SOLDIER ----------------

class PlayerSoldier : public Soldier
{
public:
    PlayerSoldier(int index);
    void update(float dt, char** lvl, int cell_size, int mapWidth, int mapHeight) override;

    void equipWeapon(Weapon* w) override;

private:
    int mplayerIndex;

    bool mIsShooting = false;   // true while fire key is held this frame

    void handleInput(float dt);
    void loadPlayerData();

    // ---- body animation setup (pistol / no-weapon sheets) ----
    void setupPlayer1Animations();
    void setupPlayer2Animations();
    void setupPlayer3Animations();
    void setupPlayer4Animations();

    // ---- weapon overlay setup per player × weapon ----
    // Player 1 (Marco)
    void setupPlayer1Pistol();
    void setupPlayer1HMG();
    void setupPlayer1Flameshot();
    void setupPlayer1Laser();
    void setupPlayer1RL();

    // Player 2 (Tarma)
    void setupPlayer2Pistol();
    void setupPlayer2HMG();
    void setupPlayer2Flameshot();
    void setupPlayer2Laser();
    void setupPlayer2RL();

    // Player 3 (Eri)
    void setupPlayer3Pistol();
    void setupPlayer3HMG();
    void setupPlayer3Flameshot();
    void setupPlayer3Laser();
    void setupPlayer3RL();

    // Player 4 (Fio)
    void setupPlayer4Pistol();
    void setupPlayer4HMG();
    void setupPlayer4Flameshot();
    void setupPlayer4Laser();
    void setupPlayer4RL();
};
*/

#pragma once
#include <SFML/Graphics.hpp>
#include "Animation.h"
#include "Weapon.h"
#include "Entity.h"

using namespace sf;

enum AnimType
{
    IDLE,
    WALK,
    JUMP_IDLE,
    JUMP_RUN,
    DIE,
    SWIM_IDLE,
    SWIM,
    ANIM_COUNT
};

enum WeaponAnimType
{
    W_IDLE,
    W_WALK,
    W_JUMP,
    W_SHOOT_IDLE,
    W_MELEE,
    W_COUNT,
    W_NONE
};

class Soldier
{
public:
    Soldier(float spd, int hp);

    virtual void update(float dt, char** lvl, int cell_size, int mapWidth, int mapHeight);
    virtual void equipWeapon(Weapon* w);

    Sprite& getSprite();
    Sprite& getWeaponSprite();

protected:
    Sprite    msprite;
    Texture   mtexture;
    Animation manimations[ANIM_COUNT];
    AnimType  mcurrentAnim;

    Sprite         mWeaponSprite;
    Texture        mWeaponTexture;
    Animation      mWeaponAnims[W_COUNT];
    WeaponAnimType mCurrentWeaponAnim;
    bool           mWeaponTextureLoaded;

    float   speed;
    int     health;
    Weapon* m_weapon = nullptr;

    bool  mIsShooting = false;
    float velocityX = 0.f;
    float velocityY = 0.f;
    float gravity = 0.5f;
    float jumpStrength = -12.f;
    bool  onGround = false;
    bool  isJumping = false;
    bool  facingRight = true;

    float collisionWidth = 60.f;
    float collisionHeight = 90.f;
    float collisionOffsetX = 0.f;
    float collisionOffsetY = 0.f;
    float baseScale = 1.f;
    float mWeaponScale = 1.f;

    void setAnimation(AnimType type);
    void setWeaponAnimation(WeaponAnimType type);
    void syncWeaponSprite();

public:
    void  setPosition(float x, float y);
    float coyoteTime = 0.f;
    float coyoteMax = 0.1f;
};

class PlayerSoldier : public Soldier
{
public:
    PlayerSoldier(int index);
    void update(float dt, char** lvl, int cell_size, int mapWidth, int mapHeight) override;
    void equipWeapon(Weapon* w) override;
    bool isWeaponLoaded() const { return mWeaponTextureLoaded; }
    bool isShooting() const { return mIsShooting; }

private:
    int mplayerIndex;

    // Textures owned here so RocketLauncher / HandGrenade always have a valid ref
    Texture mRocketTex;
    Texture mGrenadeTex;

    // Current weapon slot: 0=Pistol 1=HMG 2=Flame 3=Laser 4=Rocket
    int  mWeaponSlot = 0;
    bool mPrevCycleKey = false;   // debounce for the cycle key

    void handleInput(float dt);
    void loadPlayerData();
    void loadProjectileTextures();   // called once inside loadPlayerData()

    void setupPlayer1Animations();
    void setupPlayer2Animations();
    void setupPlayer3Animations();
    void setupPlayer4Animations();

    void setupPlayer1Pistol();    void setupPlayer1HMG();
    void setupPlayer1Flameshot(); void setupPlayer1Laser();  void setupPlayer1RL();

    void setupPlayer2Pistol();    void setupPlayer2HMG();
    void setupPlayer2Flameshot(); void setupPlayer2Laser();  void setupPlayer2RL();

    void setupPlayer3Pistol();    void setupPlayer3HMG();
    void setupPlayer3Flameshot(); void setupPlayer3Laser();  void setupPlayer3RL();

    void setupPlayer4Pistol();    void setupPlayer4HMG();
    void setupPlayer4Flameshot(); void setupPlayer4Laser();  void setupPlayer4RL();
};