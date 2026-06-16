

#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
using namespace sf;

class Enemies; // forward declaration


///////////////////////////////////WEAPON/////////////////////////////////

class Weapon
{
public:
    Weapon(const char* name, int damage, float fireRate, int maxAmmo, bool infiniteAmmo);
    virtual ~Weapon() = default;

    virtual void fire(float originX, float originY, bool facingRight, float aimAngle = 0.f) = 0;
    virtual void update(float dt) {}
    virtual void draw(RenderWindow& window) {}
    virtual void resolveHits(Enemies** enemies, int count, int cellSize, int& score) {}

    bool  canFire()     const { return mFireCooldown <= 0.f; }
    bool  isEmpty()     const { return !mInfiniteAmmo && mAmmo <= 0; }
    bool  isInfinite()  const { return mInfiniteAmmo; }
    int   getDamage()   const { return mDamage; }
    int   getAmmo()     const { return mAmmo; }
    float getFireRate() const { return mFireRate; }
    void  setFireRate(float fr) { mFireRate = fr; }
    void  addAmmo(int amt) { mAmmo += amt; }
    const char* getName() const { return mName; }

protected:
    char  mName[32];
    int   mDamage;
    float mFireRate;
    float mFireCooldown = 0.f;
    int   mAmmo;
    int   mMaxAmmo;
    bool  mInfiniteAmmo;
};


/// /////////////////////////////////////////////////////PROEJCTILE WEAPON/////////////////////////////////////

class ProjectileWeapon : public Weapon
{
public:
    ProjectileWeapon(const char* name, int damage, float fireRate, int maxAmmo, bool infiniteAmmo);
    void update(float dt) override;
    void draw(RenderWindow& window) override = 0;

protected:
    virtual void advanceProjectiles(float dt) = 0;
};



/////////////////////////////////////////PSITOL/////////////////////////////////////////////

static const int MAX_PISTOL_BULLETS = 10;

struct PistolBullet
{
    Sprite sprite;
    float  vx = 0.f;
    float vy = 0.f;
    bool   active = false;
    int    damage = 0;
    int    currentFrame = 0;
    float  frameTimer = 0.f;
};

class Pistol : public ProjectileWeapon
{
public:
    Pistol(Texture& tex);
    void fire(float originX, float originY, bool facingRight, float aimAngle = 0.f) override;
    void draw(RenderWindow& window) override;
    void resolveHits(Enemies** enemies, int count, int cellSize, int& score) override;

protected:
    void advanceProjectiles(float dt) override;

private:
    Texture& mTexture;
    PistolBullet mBullets[MAX_PISTOL_BULLETS];
    float    mBulletSpeed = 15.f;

    static const int BULLET_FRAMES = 3;
    IntRect  mBulletFrames[BULLET_FRAMES];
};



///////////////////////////////////////HEAVY MACHINE GUN////////////////////////////////////////

static const int MAX_BULLETS = 20;

struct Bullet
{
    Sprite sprite;
    float  vx = 0.f;
    float vy = 0.f;
    bool   active = false;
    int    damage = 0;
    int    currentFrame = 0;
    float  frameTimer = 0.f;
};

class HeavyMachineGun : public ProjectileWeapon
{
public:
    HeavyMachineGun(Texture& tex);
    void fire(float originX, float originY, bool facingRight, float aimAngle = 0.f) override;
    void draw(RenderWindow& window) override;
    virtual void resolveHits(Enemies** enemies, int count, int cellSize, int& score) override;

    Bullet* getBullets() { return mBullets; }

protected:
    void advanceProjectiles(float dt) override;

private:
    Texture& mTexture;
    Bullet   mBullets[MAX_BULLETS];
    float    mBulletSpeed = 15.f;

    static const int BULLET_FRAMES = 3;  // adjust to your frame count
    IntRect  mBulletFrames[BULLET_FRAMES];
};

//////////////////////////////ROCKETLAUNCGER/////////////////////////////////////////

static const int MAX_ROCKETS = 5;

struct Rocket
{
    Sprite sprite;
    float  vx = 0.f;
    float  vy = 0.f;
    bool   active = false;
    int    damage = 0;
    float  blastRadius = 3.f;
    int    currentFrame = 0;
    float  frameTimer = 0.f;
};

class RocketLauncher : public ProjectileWeapon
{
public:
    RocketLauncher(Texture& tex);
    void fire(float originX, float originY, bool facingRight, float aimAngle = 0.f) override;
    void draw(RenderWindow& window) override;
    void resolveHits(Enemies** enemies, int count, int cellSize, int& score) override;

    Rocket* getRockets() { return mRockets; }
    int     getMaxRockets() { return MAX_ROCKETS; }

protected:
    void advanceProjectiles(float dt) override;

private:
    Texture& mTexture;
    Rocket   mRockets[MAX_ROCKETS];
    float    mRocketSpeed = 10.f;

    static const int ROCKET_FRAMES = 3;  // adjust to your frame count
    IntRect  mRocketFrames[ROCKET_FRAMES];
};


//////////////////////////////////////FLAMESHOT//////////////////////////////////////////

class FlameShot : public Weapon
{
public:
    FlameShot(Texture& tex);
    void fire(float originX, float originY, bool facingRight, float aimAngle = 0.f) override;
    void update(float dt) override;
    void draw(RenderWindow& window) override;
    void resolveHits(Enemies** enemies, int count, int cellSize, int& score) override;

    bool  isActive()  const { 
        return mFlameActive;
    }

    float getOriginX() const { 
        return mOriginX;
    }

    float getOriginY()    const { 
        return mOriginY;
    }

    bool  isFacingRight() const {
        return mFacingRight; 
    }

    void  stopFire() {
        mFlameActive = false;
    }

    private:
    Texture& mTexture;
    Sprite mSprite;
    float mFrameTimer = 0.f;
    int mCurrentFrame = 0;

    static const int FLAME_FRAMES = 9;  
    IntRect  mFrames[FLAME_FRAMES];

    bool  mFlameActive = false;
    float mOriginX = 0.f;
    float mOriginY = 0.f;
    bool  mFacingRight = true;
    float mRange = 5.f;
};


///////////////////////////////////////LASER GUN///////////////////////////////////////

class LaserGun : public Weapon
{
public:
    LaserGun(Texture& tex);
    void fire(float originX, float originY, bool facingRight, float aimAngle = 0.f) override;
    void update(float dt) override;
    void draw(RenderWindow& window) override;
    void resolveHits(Enemies** enemies, int count, int cellSize, int& score) override;

    bool  beamActive()    const { return mBeamActive; }
    float getOriginX()    const { return mOriginX; }
    float getOriginY()    const { return mOriginY; }
    bool  isFacingRight() const { return mFacingRight; }
    void setMapWidth(float pixelWidth) { mMapPixelWidth = pixelWidth; }

private:
    Texture& mTexture;
    Sprite   mSprite;
    float mMapPixelWidth = 10000.f;

    static const int LASER_FRAMES = 6;  // adjust to your frame count
    IntRect  mFrames[LASER_FRAMES];

    bool  mBeamActive = false;
    float mBeamTimer = 0.f;
    float mBeamDuration = 0.1f;
    float mOriginX = 0.f;
    float mOriginY = 0.f;
    bool  mFacingRight = true;
};




 ///////////////////////////HANDGRENADE///////////////////////////////////////////////

static const int MAX_GRENADES = 10;

struct Grenade
{
    Sprite sprite;
    float  vx = 0.f;
    float  vy = 0.f;
    float  gravity = 0.4f;
    bool   active = false;
    int    damage = 0;
    float  blastRadius = 3.f;
};

class HandGrenade : public ProjectileWeapon
{
public:
    explicit HandGrenade(Texture& tex);
    void fire(float originX, float originY, bool facingRight, float aimAngle = 0.f) override;
    void draw(RenderWindow& window) override;
    void resolveHits(Enemies** enemies, int count, int cellSize, int& score) override {}

    Grenade* getGrenades() { return mGrenades; }
    int      getMaxGrenades() { return MAX_GRENADES; }

protected:
    void advanceProjectiles(float dt) override;

    Texture& mTexture;
    Grenade  mGrenades[MAX_GRENADES];
    float    mThrowSpeedX = 6.f;
    float    mThrowSpeedY = -10.f;
};


////////////////////////////////////FIREBOMB GRENADE///////////////////////////////////////

static const int MAX_FIRE_POOLS = 5;

struct FirePool
{
    float x = 0.f;
    float y = 0.f;
    float radius = 3.f;
    float timer = 10.f;
    bool  active = false;
    float tickTimer = 0.f;
};

class FireBombGrenade : public HandGrenade
{
public:
    FireBombGrenade(Texture& tex);
    void fire(float originX, float originY, bool facingRight, float aimAngle = 0.f) override;
    void update(float dt) override;
    void draw(RenderWindow& window) override;
    void resolveHits(Enemies** enemies, int count, int cellSize, int& score) override {}

    FirePool* getFirePools() { return mFirePools; }
    int       getMaxPools() { return MAX_FIRE_POOLS; }
    void      spawnFirePool(float x, float y);

private:
    FirePool mFirePools[MAX_FIRE_POOLS];
};