/*#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
using namespace sf;

// ================================================================
//  BASE WEAPON
// ================================================================
class Weapon
{
public:
    Weapon(int damage, float fireRate, int maxAmmo, bool infiniteAmmo);
    virtual ~Weapon() = default;

    virtual void fire(float originX, float originY, bool facingRight, float aimAngle = 0.f) = 0;
    virtual void update(float dt) {}
    virtual void draw(RenderWindow& window) {}

    bool  canFire()        const { return mFireCooldown <= 0.f; }
    int   getDamage()      const { return mDamage; }
    int   getAmmo()        const { return mAmmo; }
    bool  isInfinite()     const { return mInfiniteAmmo; }
    bool  isEmpty()        const { return !mInfiniteAmmo && mAmmo <= 0; }
    void  addAmmo(int amt) { mAmmo += amt; }
    float getFireRate()    const { return mFireRate; }
    void  setFireRate(float fr) { mFireRate = fr; }

protected:
    int   mDamage;
    float mFireRate;
    float mFireCooldown = 0.f;
    int   mAmmo;
    int   mMaxAmmo;
    bool  mInfiniteAmmo;
};

// ================================================================
//  PISTOL  —  default, infinite ammo, 3 damage, 4 shots/sec
// ================================================================
class Pistol : public Weapon
{
public:
    Pistol();
    void fire(float originX, float originY, bool facingRight, float aimAngle = 0.f) override;
    void update(float dt) override;
};

// ================================================================
//  HEAVY MACHINE GUN  —  held fire, double pistol rate, 3 damage
// ================================================================
class HeavyMachineGun : public Weapon
{
public:
    HeavyMachineGun();
    void fire(float originX, float originY, bool facingRight, float aimAngle = 0.f) override;
    void update(float dt) override;
};

// ================================================================
//  ROCKET LAUNCHER  —  5 AOE damage, 3-block radius, 2s reload
// ================================================================
static const int MAX_ROCKETS = 5;

struct Rocket
{
    Sprite sprite;
    float  vx = 0.f;
    float  vy = 0.f;
    bool   active = false;
    int    damage = 0;
    float  blastRadius = 3.f; // in blocks
};

class RocketLauncher : public Weapon
{
public:
    RocketLauncher(Texture& tex);
    void fire(float originX, float originY, bool facingRight, float aimAngle = 0.f) override;
    void update(float dt) override;
    void draw(RenderWindow& window) override;

    Rocket* getRockets() { return mRockets; }
    int     getMaxRockets() { return MAX_ROCKETS; }

private:
    Texture& mTexture;
    Rocket   mRockets[MAX_ROCKETS];
    float    mRocketSpeed = 10.f;
};

// ================================================================
//  FLAME SHOT  —  held fire, 5-block stream, 2 HP/sec
// ================================================================
class FlameShot : public Weapon
{
public:
    FlameShot();
    void fire(float originX, float originY, bool facingRight, float aimAngle = 0.f) override;
    void update(float dt) override;
    void draw(RenderWindow& window) override;

    bool  isActive()     const { return mFlameActive; }
    float getOriginX()   const { return mOriginX; }
    float getOriginY()   const { return mOriginY; }
    bool  isFacingRight()const { return mFacingRight; }
    void  stopFire() { mFlameActive = false; }

private:
    bool  mFlameActive = false;
    float mOriginX = 0.f;
    float mOriginY = 0.f;
    bool  mFacingRight = true;
    float mRange = 5.f;   // in blocks
};

// ================================================================
//  LASER GUN  —  instant beam to screen edge, kills on impact
// ================================================================
class LaserGun : public Weapon
{
public:
    LaserGun();
    void fire(float originX, float originY, bool facingRight, float aimAngle = 0.f) override;
    void update(float dt) override;
    void draw(RenderWindow& window) override;

    bool  beamActive()   const { return mBeamActive; }
    float getOriginX()   const { return mOriginX; }
    float getOriginY()   const { return mOriginY; }
    bool  isFacingRight()const { return mFacingRight; }

private:
    bool  mBeamActive = false;
    float mBeamTimer = 0.f;
    float mBeamDuration = 0.1f;
    float mOriginX = 0.f;
    float mOriginY = 0.f;
    bool  mFacingRight = true;
};

// ================================================================
//  HAND GRENADE  —  arc throw, 5 damage, 3-block blast radius
// ================================================================
static const int MAX_GRENADES = 10;

struct Grenade
{
    Sprite sprite;
    float  vx = 0.f;
    float  vy = 0.f;
    float  gravity = 0.4f;
    bool   active = false;
    int    damage = 0;
    float  blastRadius = 3.f;  // blocks
};

class HandGrenade : public Weapon
{
public:
    HandGrenade(Texture& tex);
    void fire(float originX, float originY, bool facingRight, float aimAngle = 0.f) override;
    void update(float dt) override;
    void draw(RenderWindow& window) override;

    Grenade* getGrenades() { return mGrenades; }
    int      getMaxGrenades() { return MAX_GRENADES; }

protected:
    Texture& mTexture;
    Grenade  mGrenades[MAX_GRENADES];
    float    mThrowSpeedX = 6.f;
    float    mThrowSpeedY = -10.f;
};

// ================================================================
//  FIRE BOMB GRENADE  —  Eri's grenade; explodes + fire pool
//  Inherits HandGrenade, overrides fire() to spawn fire pool
// ================================================================
static const int MAX_FIRE_POOLS = 5;

struct FirePool
{
    float x = 0.f;
    float y = 0.f;
    float radius = 3.f;   // blocks
    float timer = 10.f;  // seconds active
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

    FirePool* getFirePools() { return mFirePools; }
    int       getMaxPools() { return MAX_FIRE_POOLS; }

    // called when a grenade lands — spawns a fire pool at that position
    void spawnFirePool(float x, float y);

private:
    FirePool mFirePools[MAX_FIRE_POOLS];
};

#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
using namespace sf;

// ================================================================
//  WEAPON ANIMATION INDICES  (index into Soldier::mWeaponAnims[])
//  Stored here so both Weapon and Soldier share one definition.
// ================================================================
enum WeaponAnimType
{
    PISTOL_SHOOT = 0,
    MELEE_ATTACK = 1,
    HMG_IDLE = 2,
    HMG_WALK = 3,
    HMG_JUMP = 4,
    HMG_SHOOT = 5,
    FLAME_IDLE = 6,
    FLAME_WALK = 7,
    FLAME_JUMP = 8,
    FLAME_SHOOT = 9,
    LASER_IDLE = 10,
    LASER_WALK = 11,
    LASER_JUMP = 12,
    LASER_SHOOT = 13,
    ROCKET_IDLE = 14,
    ROCKET_WALK = 15,
    ROCKET_JUMP = 16,
    ROCKET_SHOOT = 17,
    WA_COUNT = 18
};

// ================================================================
//  BASE WEAPON
// ================================================================
class Weapon
{
public:
    Weapon(int damage, float fireRate, int maxAmmo, bool infiniteAmmo);
    virtual ~Weapon() = default;

    virtual void fire(float originX, float originY, bool facingRight, float aimAngle = 0.f) = 0;
    virtual void update(float dt) {}
    virtual void draw(RenderWindow& window) {}

    // ---------- identity ----------
    // Each concrete class returns a compile-time name string.
    // Used by PlayerSoldier::equipWeapon() to pick the right animation setup.
    virtual const char* getName() const = 0;

    // ---------- animation contract ----------
    // If overridesMovementAnims() returns true the soldier will use the weapon
    // spritesheet for IDLE / WALK / JUMP as well as SHOOT.
    // Pistol returns false — the base movement sheet is used for locomotion
    // and only the shoot frame switches to the weapon sheet.
    virtual bool          overridesMovementAnims() const { return false; }
    virtual WeaponAnimType getIdleAnim()           const { return PISTOL_SHOOT; }
    virtual WeaponAnimType getWalkAnim()           const { return PISTOL_SHOOT; }
    virtual WeaponAnimType getJumpAnim()           const { return PISTOL_SHOOT; }
    virtual WeaponAnimType getShootAnim()          const { return PISTOL_SHOOT; }

    // ---------- state queries ----------
    bool  canFire()        const { return mFireCooldown <= 0.f; }
    int   getDamage()      const { return mDamage; }
    int   getAmmo()        const { return mAmmo; }
    bool  isInfinite()     const { return mInfiniteAmmo; }
    bool  isEmpty()        const { return !mInfiniteAmmo && mAmmo <= 0; }
    void  addAmmo(int amt) { mAmmo += amt; }
    float getFireRate()    const { return mFireRate; }
    void  setFireRate(float fr) { mFireRate = fr; }

protected:
    int   mDamage;
    float mFireRate;
    float mFireCooldown = 0.f;
    int   mAmmo;
    int   mMaxAmmo;
    bool  mInfiniteAmmo;
};

// ================================================================
//  PROJECTILE WEAPON  (UML: abstract intermediate for all weapons
//  that spawn discrete projectiles — Pistol, HMG, Rocket, Laser)
// ================================================================
class ProjectileWeapon : public Weapon
{
public:
    ProjectileWeapon(int damage, float fireRate, int maxAmmo, bool infiniteAmmo)
        : Weapon(damage, fireRate, maxAmmo, infiniteAmmo) {
    }
    // Subclasses provide concrete fire() / update() / draw() logic.
};

// ================================================================
//  PISTOL  —  default, infinite ammo, 3 damage, 4 shots/sec
// ================================================================
class Pistol : public ProjectileWeapon
{
public:
    Pistol();
    void fire(float originX, float originY, bool facingRight, float aimAngle = 0.f) override;
    void update(float dt) override;

    const char* getName()   const override { return "pistol"; }
    // Pistol uses the base movement sheet; only the shoot frame is on weapon sheet
    bool overridesMovementAnims() const override { return false; }
    WeaponAnimType getShootAnim() const override { return PISTOL_SHOOT; }
};

// ================================================================
//  HEAVY MACHINE GUN  —  held fire, double pistol rate, 3 damage
// ================================================================
class HeavyMachineGun : public ProjectileWeapon
{
public:
    HeavyMachineGun();
    void fire(float originX, float originY, bool facingRight, float aimAngle = 0.f) override;
    void update(float dt) override;

    const char* getName()   const override { return "hmg"; }
    bool overridesMovementAnims() const override { return true; }
    WeaponAnimType getIdleAnim()  const override { return HMG_IDLE; }
    WeaponAnimType getWalkAnim()  const override { return HMG_WALK; }
    WeaponAnimType getJumpAnim()  const override { return HMG_JUMP; }
    WeaponAnimType getShootAnim() const override { return HMG_SHOOT; }
};

// ================================================================
//  ROCKET LAUNCHER  —  5 AOE damage, 3-block radius, 2s reload
// ================================================================
static const int MAX_ROCKETS = 5;

struct Rocket
{
    Sprite sprite;
    float  vx = 0.f;
    float  vy = 0.f;
    bool   active = false;
    int    damage = 0;
    float  blastRadius = 3.f;
};

class RocketLauncher : public ProjectileWeapon
{
public:
    RocketLauncher(Texture& tex);
    void fire(float originX, float originY, bool facingRight, float aimAngle = 0.f) override;
    void update(float dt) override;
    void draw(RenderWindow& window) override;

    Rocket* getRockets() { return mRockets; }
    int     getMaxRockets() { return MAX_ROCKETS; }

    const char* getName()   const override { return "rocket"; }
    bool overridesMovementAnims() const override { return true; }
    WeaponAnimType getIdleAnim()  const override { return ROCKET_IDLE; }
    WeaponAnimType getWalkAnim()  const override { return ROCKET_WALK; }
    WeaponAnimType getJumpAnim()  const override { return ROCKET_JUMP; }
    WeaponAnimType getShootAnim() const override { return ROCKET_SHOOT; }

private:
    Texture& mTexture;
    Rocket   mRockets[MAX_ROCKETS];
    float    mRocketSpeed = 10.f;
};

// ================================================================
//  FLAME SHOT  —  held fire, 5-block stream, 2 HP/sec
//  Not a ProjectileWeapon — it's an area-effect beam.
// ================================================================
class FlameShot : public Weapon
{
public:
    FlameShot();
    void fire(float originX, float originY, bool facingRight, float aimAngle = 0.f) override;
    void update(float dt) override;
    void draw(RenderWindow& window) override;

    bool  isActive()      const { return mFlameActive; }
    float getOriginX()    const { return mOriginX; }
    float getOriginY()    const { return mOriginY; }
    bool  isFacingRight() const { return mFacingRight; }
    void  stopFire() { mFlameActive = false; }

    const char* getName()   const override { return "flameshot"; }
    bool overridesMovementAnims() const override { return true; }
    WeaponAnimType getIdleAnim()  const override { return FLAME_IDLE; }
    WeaponAnimType getWalkAnim()  const override { return FLAME_WALK; }
    WeaponAnimType getJumpAnim()  const override { return FLAME_JUMP; }
    WeaponAnimType getShootAnim() const override { return FLAME_SHOOT; }

private:
    bool  mFlameActive = false;
    float mOriginX = 0.f;
    float mOriginY = 0.f;
    bool  mFacingRight = true;
    float mRange = 5.f;
};

// ================================================================
//  LASER GUN  —  instant beam to screen edge, kills on impact
// ================================================================
class LaserGun : public ProjectileWeapon
{
public:
    LaserGun();
    void fire(float originX, float originY, bool facingRight, float aimAngle = 0.f) override;
    void update(float dt) override;
    void draw(RenderWindow& window) override;

    bool  beamActive()    const { return mBeamActive; }
    float getOriginX()    const { return mOriginX; }
    float getOriginY()    const { return mOriginY; }
    bool  isFacingRight() const { return mFacingRight; }

    const char* getName()   const override { return "laserbeam"; }
    bool overridesMovementAnims() const override { return true; }
    WeaponAnimType getIdleAnim()  const override { return LASER_IDLE; }
    WeaponAnimType getWalkAnim()  const override { return LASER_WALK; }
    WeaponAnimType getJumpAnim()  const override { return LASER_JUMP; }
    WeaponAnimType getShootAnim() const override { return LASER_SHOOT; }

private:
    bool  mBeamActive = false;
    float mBeamTimer = 0.f;
    float mBeamDuration = 0.1f;
    float mOriginX = 0.f;
    float mOriginY = 0.f;
    bool  mFacingRight = true;
};

// ================================================================
//  HAND GRENADE  —  arc throw, 5 damage, 3-block blast radius
// ================================================================
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

class HandGrenade : public Weapon
{
public:
    HandGrenade(Texture& tex);
    void fire(float originX, float originY, bool facingRight, float aimAngle = 0.f) override;
    void update(float dt) override;
    void draw(RenderWindow& window) override;

    Grenade* getGrenades() { return mGrenades; }
    int      getMaxGrenades() { return MAX_GRENADES; }

    const char* getName()   const override { return "grenade"; }
    // Grenade uses the melee/throw anim slot (non-looping one-shot)
    bool overridesMovementAnims() const override { return false; }
    WeaponAnimType getShootAnim() const override { return MELEE_ATTACK; }

protected:
    Texture& mTexture;
    Grenade  mGrenades[MAX_GRENADES];
    float    mThrowSpeedX = 6.f;
    float    mThrowSpeedY = -10.f;
};

// ================================================================
//  FIRE BOMB GRENADE  —  Eri's grenade; explodes + fire pool
// ================================================================
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

    FirePool* getFirePools() { return mFirePools; }
    int       getMaxPools() { return MAX_FIRE_POOLS; }
    void      spawnFirePool(float x, float y);

    const char* getName() const override { return "firebomb"; }

private:
    FirePool mFirePools[MAX_FIRE_POOLS];
};

#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
using namespace sf;

// ================================================================
//  BASE WEAPON
//  Abstract base. Non-projectile weapons (FlameShot, LaserGun)
//  inherit directly from here.
//  Projectile weapons go through ProjectileWeapon (see below).
// ================================================================
class Weapon
{
public:
    Weapon(const char* name, int damage, float fireRate, int maxAmmo, bool infiniteAmmo);
    virtual ~Weapon() = default;

    virtual void fire(float originX, float originY, bool facingRight, float aimAngle = 0.f) = 0;
    virtual void update(float dt) {}
    virtual void draw(RenderWindow& window) {}

    bool  canFire()     const { return mFireCooldown <= 0.f; }
    bool  isEmpty()     const { return !mInfiniteAmmo && mAmmo <= 0; }
    bool  isInfinite()  const { return mInfiniteAmmo; }
    int   getDamage()   const { return mDamage; }
    int   getAmmo()     const { return mAmmo; }
    float getFireRate() const { return mFireRate; }
    void  setFireRate(float fr) { mFireRate = fr; }
    void  addAmmo(int amt) { mAmmo += amt; }

    // Returns a short identifier string used by equipWeapon() to pick
    // the correct weapon-overlay spritesheet.  Never nullptr.
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

// ================================================================
//  PROJECTILE WEAPON  (intermediate abstract class – see UML)
//  Owns projectile state and shared launch helpers.
//  Pistol, HeavyMachineGun, and RocketLauncher inherit from this.
// ================================================================
class ProjectileWeapon : public Weapon
{
public:
    ProjectileWeapon(const char* name, int damage, float fireRate, int maxAmmo, bool infiniteAmmo);

    // Subclasses still must implement fire() and draw().
    // update() is provided here and calls advanceProjectiles().

    void update(float dt) override;
    void draw(RenderWindow& window) override = 0;

protected:
    // Override this in each subclass to move / collide your projectiles.
    virtual void advanceProjectiles(float dt) = 0;
};

// ================================================================
//  PISTOL  —  default weapon, infinite ammo, 3 damage, 4 shots/sec
// ================================================================
class Pistol : public ProjectileWeapon
{
public:
    Pistol();
    void fire(float originX, float originY, bool facingRight, float aimAngle = 0.f) override;
    void draw(RenderWindow& window) override;

protected:
    void advanceProjectiles(float dt) override;
};

// ================================================================
//  HEAVY MACHINE GUN  —  held fire, 8 shots/sec, 3 damage, 200 ammo
// ================================================================
class HeavyMachineGun : public ProjectileWeapon
{
public:
    HeavyMachineGun();
    void fire(float originX, float originY, bool facingRight, float aimAngle = 0.f) override;
    void draw(RenderWindow& window) override;

protected:
    void advanceProjectiles(float dt) override;
};

// ================================================================
//  ROCKET LAUNCHER  —  5 AOE damage, 3-block radius, 2 s reload
// ================================================================
static const int MAX_ROCKETS = 5;

struct Rocket
{
    Sprite sprite;
    float  vx = 0.f;
    float  vy = 0.f;
    bool   active = false;
    int    damage = 0;
    float  blastRadius = 3.f;
};

class RocketLauncher : public ProjectileWeapon
{
public:
    RocketLauncher(Texture& tex);
    void fire(float originX, float originY, bool facingRight, float aimAngle = 0.f) override;
    void draw(RenderWindow& window) override;

    Rocket* getRockets() { return mRockets; }
    int     getMaxRockets() { return MAX_ROCKETS; }

protected:
    void advanceProjectiles(float dt) override;

private:
    Texture& mTexture;
    Rocket   mRockets[MAX_ROCKETS];
    float    mRocketSpeed = 10.f;
};

// ================================================================
//  FLAME SHOT  —  held fire, 5-block stream, 2 HP/sec
//  Inherits Weapon directly (not projectile-based).
// ================================================================
class FlameShot : public Weapon
{
public:
    FlameShot();
    void fire(float originX, float originY, bool facingRight, float aimAngle = 0.f) override;
    void update(float dt) override;
    void draw(RenderWindow& window) override;

    bool  isActive()      const { return mFlameActive; }
    float getOriginX()    const { return mOriginX; }
    float getOriginY()    const { return mOriginY; }
    bool  isFacingRight() const { return mFacingRight; }
    void  stopFire() { mFlameActive = false; }

private:
    bool  mFlameActive = false;
    float mOriginX = 0.f;
    float mOriginY = 0.f;
    bool  mFacingRight = true;
    float mRange = 5.f;   // blocks
};

// ================================================================
//  LASER GUN  —  instant beam to screen edge, kills on impact
//  Inherits Weapon directly (beam, not a projectile).
// ================================================================
class LaserGun : public Weapon
{
public:
    LaserGun();
    void fire(float originX, float originY, bool facingRight, float aimAngle = 0.f) override;
    void update(float dt) override;
    void draw(RenderWindow& window) override;

    bool  beamActive()    const { return mBeamActive; }
    float getOriginX()    const { return mOriginX; }
    float getOriginY()    const { return mOriginY; }
    bool  isFacingRight() const { return mFacingRight; }

private:
    bool  mBeamActive = false;
    float mBeamTimer = 0.f;
    float mBeamDuration = 0.1f;
    float mOriginX = 0.f;
    float mOriginY = 0.f;
    bool  mFacingRight = true;
};

// ================================================================
//  HAND GRENADE  —  arc throw, 5 damage, 3-block blast radius
//  Inherits ProjectileWeapon (grenades are projectiles).
// ================================================================
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

    Grenade* getGrenades() { return mGrenades; }
    int      getMaxGrenades() { return MAX_GRENADES; }

protected:
    void advanceProjectiles(float dt) override;

    Texture& mTexture;
    Grenade  mGrenades[MAX_GRENADES];
    float    mThrowSpeedX = 6.f;
    float    mThrowSpeedY = -10.f;
};

// ================================================================
//  FIRE BOMB GRENADE  —  Eri's grenade; arc throw + lingering fire pool
//  Inherits HandGrenade (specialises the landing behaviour).
// ================================================================
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

    FirePool* getFirePools() { return mFirePools; }
    int       getMaxPools() { return MAX_FIRE_POOLS; }

    // Call from game collision logic when a grenade lands.
    void spawnFirePool(float x, float y);

private:
    FirePool mFirePools[MAX_FIRE_POOLS];
};*/

#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
using namespace sf;

class Enemies; // forward declaration

// ================================================================
//  BASE WEAPON
// ================================================================
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

// ================================================================
//  PROJECTILE WEAPON
// ================================================================
class ProjectileWeapon : public Weapon
{
public:
    ProjectileWeapon(const char* name, int damage, float fireRate, int maxAmmo, bool infiniteAmmo);
    void update(float dt) override;
    void draw(RenderWindow& window) override = 0;

protected:
    virtual void advanceProjectiles(float dt) = 0;
};

// ================================================================
//  PISTOL
// ================================================================
static const int MAX_PISTOL_BULLETS = 10;

struct PistolBullet
{
    Sprite sprite;
    float  vx = 0.f;
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

/*
// ================================================================
//  PISTOL
// ================================================================
class Pistol : public ProjectileWeapon
{
public:
    Pistol();
    void fire(float originX, float originY, bool facingRight, float aimAngle = 0.f) override;
    void draw(RenderWindow& window) override;
    void resolveHits(Enemies** enemies, int count, int cellSize, int& score) override {}

protected:
    void advanceProjectiles(float dt) override;
};*/

// ================================================================
//  HEAVY MACHINE GUN
// ================================================================
static const int MAX_BULLETS = 20;

struct Bullet
{
    Sprite sprite;
    float  vx = 0.f;
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

// ================================================================
//  ROCKET LAUNCHER
// ================================================================
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

// ================================================================
//  FLAME SHOT
// ================================================================
class FlameShot : public Weapon
{
public:
    FlameShot(Texture& tex);
    void fire(float originX, float originY, bool facingRight, float aimAngle = 0.f) override;
    void update(float dt) override;
    void draw(RenderWindow& window) override;
    void resolveHits(Enemies** enemies, int count, int cellSize, int& score) override;

    bool  isActive()      const { return mFlameActive; }
    float getOriginX()    const { return mOriginX; }
    float getOriginY()    const { return mOriginY; }
    bool  isFacingRight() const { return mFacingRight; }
    void  stopFire() { mFlameActive = false; }

private:
    Texture& mTexture;
    Sprite   mSprite;
    float    mFrameTimer = 0.f;
    int      mCurrentFrame = 0;

    static const int FLAME_FRAMES = 9;  // adjust to your frame count
    IntRect  mFrames[FLAME_FRAMES];

    bool  mFlameActive = false;
    float mOriginX = 0.f;
    float mOriginY = 0.f;
    bool  mFacingRight = true;
    float mRange = 5.f;
};

// ================================================================
//  LASER GUN
// ================================================================
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

// ================================================================
//  HAND GRENADE
// ================================================================
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

// ================================================================
//  FIRE BOMB GRENADE
// ================================================================
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