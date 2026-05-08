/*#include "Weapon.h"

// ================================================================
//  BASE WEAPON
// ================================================================
Weapon::Weapon(int damage, float fireRate, int maxAmmo, bool infiniteAmmo)
    : mDamage(damage), mFireRate(fireRate),
    mAmmo(maxAmmo), mMaxAmmo(maxAmmo), mInfiniteAmmo(infiniteAmmo) {
}

// ================================================================
//  PISTOL
// ================================================================
Pistol::Pistol() : Weapon(3, 4.f, 0, true) {}

void Pistol::fire(float originX, float originY, bool facingRight, float aimAngle)
{
    if (!canFire()) return;
    mFireCooldown = 1.f / mFireRate;
    std::cout << "Pistol fired! Damage:" << mDamage
        << " Angle:" << aimAngle << "\n";
    // hook: raycast at aimAngle from origin, deal mDamage to first enemy hit
}

void Pistol::update(float dt)
{
    if (mFireCooldown > 0.f) mFireCooldown -= dt;
}

// ================================================================
//  HEAVY MACHINE GUN
// ================================================================
HeavyMachineGun::HeavyMachineGun() : Weapon(3, 8.f, 200, false) {}

void HeavyMachineGun::fire(float originX, float originY, bool facingRight, float aimAngle)
{
    if (!canFire() || isEmpty()) return;
    mFireCooldown = 1.f / mFireRate;
    mAmmo--;
    std::cout << "HMG fired! Ammo left:" << mAmmo << "\n";
    // hook: raycast, deal mDamage — called every frame while key held
}

void HeavyMachineGun::update(float dt)
{
    if (mFireCooldown > 0.f) mFireCooldown -= dt;
}

// ================================================================
//  ROCKET LAUNCHER
// ================================================================
RocketLauncher::RocketLauncher(Texture& tex)
    : Weapon(5, 0.5f, 10, false), mTexture(tex)
{
    for (int i = 0; i < MAX_ROCKETS; i++)
    {
        mRockets[i].active = false;
        mRockets[i].sprite.setTexture(mTexture);
    }
}

void RocketLauncher::fire(float originX, float originY, bool facingRight, float aimAngle)
{
    if (!canFire() || isEmpty()) return;

    for (int i = 0; i < MAX_ROCKETS; i++)
    {
        if (!mRockets[i].active)
        {
            // convert aimAngle to velocity components
            float rad = aimAngle * 3.14159f / 180.f;
            mRockets[i].active = true;
            mRockets[i].damage = mDamage;
            mRockets[i].sprite.setPosition(originX, originY);
            mRockets[i].vx = facingRight ? mRocketSpeed * cos(rad)
                : -mRocketSpeed * cos(rad);
            mRockets[i].vy = -mRocketSpeed * sin(rad);
            mFireCooldown = 1.f / mFireRate;   // 2s reload
            mAmmo--;
            std::cout << "Rocket fired! Ammo left:" << mAmmo << "\n";
            break;
        }
    }
}

void RocketLauncher::update(float dt)
{
    if (mFireCooldown > 0.f) mFireCooldown -= dt;

    for (int i = 0; i < MAX_ROCKETS; i++)
    {
        if (!mRockets[i].active) continue;
        mRockets[i].sprite.move(mRockets[i].vx, mRockets[i].vy);

        float px = mRockets[i].sprite.getPosition().x;
        float py = mRockets[i].sprite.getPosition().y;
        if (px < 0 || px > 10000 || py < 0 || py > 10000)
            mRockets[i].active = false;
        // hook: check tile/enemy collision → explode → AOE damage in blastRadius blocks
    }
}

void RocketLauncher::draw(RenderWindow& window)
{
    for (int i = 0; i < MAX_ROCKETS; i++)
        if (mRockets[i].active)
            window.draw(mRockets[i].sprite);
}

// ================================================================
//  FLAME SHOT
// ================================================================
FlameShot::FlameShot() : Weapon(2, 30.f, 300, false) {}

void FlameShot::fire(float originX, float originY, bool facingRight, float aimAngle)
{
    if (isEmpty()) return;
    mFlameActive = true;
    mOriginX = originX;
    mOriginY = originY;
    mFacingRight = facingRight;
    mAmmo--;
    // hook: each frame this is active, deal 2 HP/sec to enemies within 5-block stream
}

void FlameShot::update(float dt)
{
    if (mFireCooldown > 0.f) mFireCooldown -= dt;
    // mFlameActive is reset each frame by input handler — only true while key held
}

void FlameShot::draw(RenderWindow& window)
{
    // hook: draw flame sprite stream from origin in facing direction up to mRange blocks
}

// ================================================================
//  LASER GUN
// ================================================================
LaserGun::LaserGun() : Weapon(9999, 1.f, 30, false) {}

void LaserGun::fire(float originX, float originY, bool facingRight, float aimAngle)
{
    if (!canFire() || isEmpty()) return;
    mBeamActive = true;
    mBeamTimer = 0.f;
    mOriginX = originX;
    mOriginY = originY;
    mFacingRight = facingRight;
    mFireCooldown = 1.f / mFireRate;
    mAmmo--;
    std::cout << "Laser fired! Ammo left:" << mAmmo << "\n";
    // hook: instantly kill all enemies along the beam line
}

void LaserGun::update(float dt)
{
    if (mFireCooldown > 0.f) mFireCooldown -= dt;
    if (mBeamActive)
    {
        mBeamTimer += dt;
        if (mBeamTimer >= mBeamDuration)
            mBeamActive = false;
    }
}

void LaserGun::draw(RenderWindow& window)
{
    // hook: if mBeamActive draw a line from origin to screen edge
}

// ================================================================
//  HAND GRENADE
// ================================================================
HandGrenade::HandGrenade(Texture& tex)
    : Weapon(5, 2.f, 10, false), mTexture(tex)
{
    for (int i = 0; i < MAX_GRENADES; i++)
    {
        mGrenades[i].active = false;
        mGrenades[i].sprite.setTexture(mTexture);
    }
}

void HandGrenade::fire(float originX, float originY, bool facingRight, float aimAngle)
{
    if (!canFire() || isEmpty()) return;

    for (int i = 0; i < MAX_GRENADES; i++)
    {
        if (!mGrenades[i].active)
        {
            mGrenades[i].active = true;
            mGrenades[i].damage = mDamage;
            mGrenades[i].sprite.setPosition(originX, originY);
            mGrenades[i].vx = facingRight ? mThrowSpeedX : -mThrowSpeedX;
            mGrenades[i].vy = mThrowSpeedY;
            mFireCooldown = 1.f / mFireRate;
            mAmmo--;
            std::cout << "Grenade thrown! Ammo left:" << mAmmo << "\n";
            break;
        }
    }
}

void HandGrenade::update(float dt)
{
    if (mFireCooldown > 0.f) mFireCooldown -= dt;

    for (int i = 0; i < MAX_GRENADES; i++)
    {
        if (!mGrenades[i].active) continue;
        mGrenades[i].vy += mGrenades[i].gravity;
        mGrenades[i].sprite.move(mGrenades[i].vx, mGrenades[i].vy);

        float px = mGrenades[i].sprite.getPosition().x;
        float py = mGrenades[i].sprite.getPosition().y;
        if (py > 10000 || px < 0 || px > 10000)
            mGrenades[i].active = false;
        // hook: on tile hit → explode → AOE damage in blastRadius blocks → deactivate
    }
}

void HandGrenade::draw(RenderWindow& window)
{
    for (int i = 0; i < MAX_GRENADES; i++)
        if (mGrenades[i].active)
            window.draw(mGrenades[i].sprite);
}

// ================================================================
//  FIRE BOMB GRENADE
// ================================================================
FireBombGrenade::FireBombGrenade(Texture& tex) : HandGrenade(tex)
{
    for (int i = 0; i < MAX_FIRE_POOLS; i++)
        mFirePools[i].active = false;
    mDamage = 5;    // same blast damage as hand grenade
}

void FireBombGrenade::fire(float originX, float originY, bool facingRight, float aimAngle)
{
    HandGrenade::fire(originX, originY, facingRight, aimAngle);
    // fire pool spawned when grenade lands — call spawnFirePool() from game collision logic
}

void FireBombGrenade::spawnFirePool(float x, float y)
{
    for (int i = 0; i < MAX_FIRE_POOLS; i++)
    {
        if (!mFirePools[i].active)
        {
            mFirePools[i].active = true;
            mFirePools[i].x = x;
            mFirePools[i].y = y;
            mFirePools[i].timer = 10.f;
            mFirePools[i].tickTimer = 0.f;
            std::cout << "Fire pool spawned at (" << x << "," << y << ")\n";
            break;
        }
    }
}

void FireBombGrenade::update(float dt)
{
    HandGrenade::update(dt);

    for (int i = 0; i < MAX_FIRE_POOLS; i++)
    {
        if (!mFirePools[i].active) continue;

        mFirePools[i].timer -= dt;
        mFirePools[i].tickTimer -= dt;

        if (mFirePools[i].tickTimer <= 0.f)
        {
            mFirePools[i].tickTimer = 1.f;
            // hook: deal 2 HP to all enemies within mFirePools[i].radius blocks
            std::cout << "Fire pool ticking damage\n";
        }

        if (mFirePools[i].timer <= 0.f)
            mFirePools[i].active = false;
    }
}

void FireBombGrenade::draw(RenderWindow& window)
{
    HandGrenade::draw(window);
    // hook: draw fire pool visual at each active pool position
}*/
#include "Weapon.h"
#include <cstring>
#include <cmath>

// ================================================================
//  BASE WEAPON
// ================================================================
Weapon::Weapon(const char* name, int damage, float fireRate, int maxAmmo, bool infiniteAmmo)
    : mDamage(damage), mFireRate(fireRate),
    mAmmo(maxAmmo), mMaxAmmo(maxAmmo), mInfiniteAmmo(infiniteAmmo)
{
    int i = 0;
    while (name[i] != '\0' && i < 31) { mName[i] = name[i]; i++; }
    mName[i] = '\0';
}

// ================================================================
//  PROJECTILE WEAPON
// ================================================================
ProjectileWeapon::ProjectileWeapon(const char* name, int damage, float fireRate,
    int maxAmmo, bool infiniteAmmo)
    : Weapon(name, damage, fireRate, maxAmmo, infiniteAmmo) {
}

void ProjectileWeapon::update(float dt)
{
    if (mFireCooldown > 0.f) mFireCooldown -= dt;
    advanceProjectiles(dt);
}

// ================================================================
//  PISTOL  — 3 dmg, 4 shots/sec, infinite ammo (BASE STATS)
// ================================================================
Pistol::Pistol() : ProjectileWeapon("pistol", 3, 4.f, 0, true) {}

void Pistol::fire(float originX, float originY, bool facingRight, float aimAngle)
{
    if (!canFire()) return;
    mFireCooldown = 1.f / mFireRate;
    std::cout << "Pistol fired! Damage:" << mDamage << "\n";
    // hook: raycast at aimAngle, deal mDamage to first enemy hit
}

void Pistol::draw(RenderWindow& /*window*/) {}
void Pistol::advanceProjectiles(float /*dt*/) {}

// ================================================================
//  HEAVY MACHINE GUN  — 3 dmg, 8 shots/sec (double pistol), 200 ammo
// ================================================================
HeavyMachineGun::HeavyMachineGun() : ProjectileWeapon("hmg", 3, 8.f, 200, false) {}

void HeavyMachineGun::fire(float originX, float originY, bool facingRight, float aimAngle)
{
    if (!canFire() || isEmpty()) return;
    mFireCooldown = 1.f / mFireRate;
    mAmmo--;
    std::cout << "HMG fired! Ammo:" << mAmmo << "\n";
}

void HeavyMachineGun::draw(RenderWindow& /*window*/) {}
void HeavyMachineGun::advanceProjectiles(float /*dt*/) {}

// ================================================================
//  ROCKET LAUNCHER  — 5 dmg, 3-block AOE, 0.5 shots/sec (2s reload), 10 ammo
// ================================================================
RocketLauncher::RocketLauncher(Texture& tex)
    : ProjectileWeapon("rocket", 5, 0.5f, 10, false), mTexture(tex)
{
    for (int i = 0; i < MAX_ROCKETS; i++)
    {
        mRockets[i].active = false;
        mRockets[i].sprite.setTexture(mTexture);
    }
}

void RocketLauncher::fire(float originX, float originY, bool facingRight, float aimAngle)
{
    if (!canFire() || isEmpty()) return;
    for (int i = 0; i < MAX_ROCKETS; i++)
    {
        if (!mRockets[i].active)
        {
            float rad = aimAngle * 3.14159f / 180.f;
            mRockets[i].active = true;
            mRockets[i].damage = mDamage;
            mRockets[i].sprite.setPosition(originX, originY);
            mRockets[i].vx = facingRight ? mRocketSpeed * std::cos(rad)
                : -mRocketSpeed * std::cos(rad);
            mRockets[i].vy = -mRocketSpeed * std::sin(rad);
            mFireCooldown = 1.f / mFireRate;
            mAmmo--;
            break;
        }
    }
}

void RocketLauncher::draw(RenderWindow& window)
{
    for (int i = 0; i < MAX_ROCKETS; i++)
        if (mRockets[i].active) window.draw(mRockets[i].sprite);
}

void RocketLauncher::advanceProjectiles(float /*dt*/)
{
    for (int i = 0; i < MAX_ROCKETS; i++)
    {
        if (!mRockets[i].active) continue;
        mRockets[i].sprite.move(mRockets[i].vx, mRockets[i].vy);
        float px = mRockets[i].sprite.getPosition().x;
        float py = mRockets[i].sprite.getPosition().y;
        if (px < 0 || px > 10000 || py < 0 || py > 10000)
            mRockets[i].active = false;
        // hook: tile/enemy collision -> explode -> 5 HP AOE in 3-block radius
    }
}

// ================================================================
//  FLAME SHOT  — 2 HP/sec, 5-block stream, key-held trigger, 300 ammo
//  Direct Weapon child (no projectile pool)
// ================================================================
FlameShot::FlameShot() : Weapon("flameshot", 2, 30.f, 300, false) {}

void FlameShot::fire(float originX, float originY, bool facingRight, float aimAngle)
{
    if (isEmpty()) return;
    mFlameActive = true;
    mOriginX = originX;
    mOriginY = originY;
    mFacingRight = facingRight;
    mAmmo--;
    // hook: deal 2 HP/sec to enemies in 5-block stream; instant-kill mummies
}

void FlameShot::update(float dt)
{
    if (mFireCooldown > 0.f) mFireCooldown -= dt;
    // mFlameActive is reset each frame by the input handler (only true while key held)
}

void FlameShot::draw(RenderWindow& /*window*/)
{
    // hook: draw flame sprite stream while mFlameActive
}

// ================================================================
//  LASER GUN  — instant-kill beam to screen edge, 30 ammo, 1 shot/sec
//  Direct Weapon child (beam, not projectile)
// ================================================================
LaserGun::LaserGun() : Weapon("laserbeam", 9999, 1.f, 30, false) {}

void LaserGun::fire(float originX, float originY, bool facingRight, float aimAngle)
{
    if (!canFire() || isEmpty()) return;
    mBeamActive = true;
    mBeamTimer = 0.f;
    mOriginX = originX;
    mOriginY = originY;
    mFacingRight = facingRight;
    mFireCooldown = 1.f / mFireRate;
    mAmmo--;
    // hook: instantly kill all enemies along beam line in facing direction
}

void LaserGun::update(float dt)
{
    if (mFireCooldown > 0.f) mFireCooldown -= dt;
    if (mBeamActive)
    {
        mBeamTimer += dt;
        if (mBeamTimer >= mBeamDuration)
            mBeamActive = false;
    }
}

void LaserGun::draw(RenderWindow& /*window*/)
{
    // hook: draw beam line from origin to screen edge while mBeamActive
}

// ================================================================
//  HAND GRENADE  — 5 HP, 3-block blast radius, ballistic arc, 10 ammo
//  ProjectileWeapon child
// ================================================================
HandGrenade::HandGrenade(Texture& tex)
    : ProjectileWeapon("grenade", 5, 2.f, 10, false), mTexture(tex)
{
    for (int i = 0; i < MAX_GRENADES; i++)
    {
        mGrenades[i].active = false;
        mGrenades[i].sprite.setTexture(mTexture);
    }
}

void HandGrenade::fire(float originX, float originY, bool facingRight, float aimAngle)
{
    if (!canFire() || isEmpty()) return;
    for (int i = 0; i < MAX_GRENADES; i++)
    {
        if (!mGrenades[i].active)
        {
            mGrenades[i].active = true;
            mGrenades[i].damage = mDamage;
            mGrenades[i].sprite.setPosition(originX, originY);
            mGrenades[i].vx = facingRight ? mThrowSpeedX : -mThrowSpeedX;
            mGrenades[i].vy = mThrowSpeedY;
            mFireCooldown = 1.f / mFireRate;
            mAmmo--;
            break;
        }
    }
}

void HandGrenade::draw(RenderWindow& window)
{
    for (int i = 0; i < MAX_GRENADES; i++)
        if (mGrenades[i].active) window.draw(mGrenades[i].sprite);
}

void HandGrenade::advanceProjectiles(float /*dt*/)
{
    for (int i = 0; i < MAX_GRENADES; i++)
    {
        if (!mGrenades[i].active) continue;
        mGrenades[i].vy += mGrenades[i].gravity;
        mGrenades[i].sprite.move(mGrenades[i].vx, mGrenades[i].vy);
        float px = mGrenades[i].sprite.getPosition().x;
        float py = mGrenades[i].sprite.getPosition().y;
        if (py > 10000 || px < 0 || px > 10000)
            mGrenades[i].active = false;
        // hook: tile collision -> explode -> 5 HP AOE in 3-block radius
    }
}

// ================================================================
//  FIRE BOMB GRENADE  — Eri's default; same arc as HandGrenade +
//  spawns a 10-second fire pool (radius 3 blocks, 2 HP/sec tick)
// ================================================================
FireBombGrenade::FireBombGrenade(Texture& tex) : HandGrenade(tex)
{
    for (int i = 0; i < MAX_FIRE_POOLS; i++)
        mFirePools[i].active = false;
    mDamage = 5;
    // Override name so equipWeapon() can identify it
    const char* n = "firebomb";
    int j = 0;
    while (n[j] && j < 31) { mName[j] = n[j]; j++; }
    mName[j] = '\0';
}

void FireBombGrenade::fire(float originX, float originY, bool facingRight, float aimAngle)
{
    HandGrenade::fire(originX, originY, facingRight, aimAngle);
    // Fire pool is spawned from game collision logic when grenade lands
}

void FireBombGrenade::spawnFirePool(float x, float y)
{
    for (int i = 0; i < MAX_FIRE_POOLS; i++)
    {
        if (!mFirePools[i].active)
        {
            mFirePools[i].active = true;
            mFirePools[i].x = x;
            mFirePools[i].y = y;
            mFirePools[i].timer = 10.f;
            mFirePools[i].tickTimer = 0.f;
            break;
        }
    }
}

void FireBombGrenade::update(float dt)
{
    HandGrenade::update(dt);   // advances grenade projectiles

    for (int i = 0; i < MAX_FIRE_POOLS; i++)
    {
        if (!mFirePools[i].active) continue;
        mFirePools[i].timer -= dt;
        mFirePools[i].tickTimer -= dt;

        if (mFirePools[i].tickTimer <= 0.f)
        {
            mFirePools[i].tickTimer = 1.f;
            // hook: deal 2 HP to all enemies within radius blocks
        }

        if (mFirePools[i].timer <= 0.f)
            mFirePools[i].active = false;
    }
}

void FireBombGrenade::draw(RenderWindow& window)
{
    HandGrenade::draw(window);
    // hook: draw fire pool visual at each active pool position
}