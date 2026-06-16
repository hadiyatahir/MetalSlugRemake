




#pragma once
#include <SFML/Graphics.hpp>
#include "Animation.h"
#include "TransformationState.h"
#include "Weapon.h"
#include <SFML/Audio.hpp>

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
    SWIM_SHOOT,
    MELEE,
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


/////////////////////////////////////////////////SOLDIER/////////////////////////////////


class Soldier
{
public:
    Soldier(float spd, int hp);

    virtual void update(float dt, char** lvl, int cell_size, int mapWidth, int mapHeight, int biomeId);
    virtual void equipWeapon(Weapon* w);
    Weapon* getWeapon() { return m_weapon; }

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
    //protected:
    TransformationState* mTransformState = nullptr;

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

enum class DamageState { HEALTHY, INJURED, CRITICAL };


/////////////////////////////////////////PLAYERSOLDIER////////////////////////////////////////


class PlayerSoldier : public Soldier
{
public:
    PlayerSoldier(int index);
    void update(float dt, char** lvl, int cell_size, int mapWidth, int mapHeight, int biomeId) override;
    void equipWeapon(Weapon* w) override;
    bool isWeaponLoaded() const { return mWeaponTextureLoaded; }
    bool isShooting() const { return mIsShooting; }

    void        takeDamage();
    DamageState getDamageState() const { return mDamageState; }
    int         getLives()       const { return mLives; }
    bool        isPlayerAlive()  const { return mIsAlive; }
    void        respawn();
    void setActive(bool active) { mIsActive = active; }
    void restoreNormalForm();
    void applyUndeadVisuals();
    void applyMummyVisuals();
    void forcePistol();
    void disarm();
    int  getSaturation()  const { return mSaturation; }
    void addSaturation(int amount);
    void receiveCrate(int weaponSlot, int grenadeCount, bool fireBomb);

    void setTransformState(TransformationState* newState)
    {
        if (mTransformState) { mTransformState->exit(*this); delete mTransformState; }
        mTransformState = newState;
        if (mTransformState) mTransformState->enter(*this);
    }

    void drawDevMode(sf::RenderWindow& window);

    void applyUndead() { setTransformState(new UndeadState()); }
    void applyMummy() { setTransformState(new MummyState()); }

private:
    int mplayerIndex;
    bool isImmortal;
    sf::Font devFont;
    sf::Text devText;
    sf::Music devMusic;

    DamageState mDamageState = DamageState::HEALTHY;
    float       mDamageTimer = 0.f;
    int         mLives = 2;
    bool        mIsAlive = true;
    bool mIsActive = false;
    int mSaturation = 10;
    int mMaxSaturation = 10;
    int mGrenadeCount = 0;
    float mTransformTimer = 0.f; 
    float aimAngle = 0.f;
    float mSwimScale = 0.25f;
    sf::Texture mswim;
    bool inwater = false;

    // Textures owned here so RocketLauncher / HandGrenade always have a valid ref
   // Texture mRocketTex;
   // Texture mGrenadeTex;

    sf::Texture mAmmoTex;     // replaces mRocketTex
    sf::Texture mGrenadeTex;
    sf::Texture mUndeadTexture;   // ← add
    sf::Texture mMummyTexture;

    int         mMapWidth = 0;
    int         mCellSize = 0;

    // Current weapon slot 0=Pistol 1=HMG 2=Flame 3=Laser 4=Rocket
    int  mWeaponSlot = 0;
    bool mPrevCycleKey = false;   

    bool mPrevFireKey = false;   
    bool rawFireKey = false;

    void handleInput(float dt);
    void loadPlayerData();
    void loadProjectileTextures();   // called once inside loadPlayerData()


    void setupPlayer3SwimAnimations();

    void setupMaleUndeadAnimations();
    void setupFemaleUndeadAnimations();
    void setupMaleMummyAnimations();
    void setupFemaleMummyAnimations();

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