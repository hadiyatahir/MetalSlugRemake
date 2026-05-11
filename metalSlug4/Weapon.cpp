
#include "Weapon.h"
#include "Enemy.h"
#include <cstring>
#include <cmath>

///////////////////////////////WEAPON///////////////////////////////


Weapon::Weapon(const char* name, int damage, float fireRate, int maxAmmo, bool infiniteAmmo)
    : mDamage(damage), mFireRate(fireRate),
    mAmmo(maxAmmo), mMaxAmmo(maxAmmo), mInfiniteAmmo(infiniteAmmo)
{
    int i = 0;
    while (name[i] != '\0' && i < 31) { mName[i] = name[i]; i++; }
    mName[i] = '\0';
}



///////////////////////////////////////PROJECTILE WEAPON///////////////////////////////////////////////////////


ProjectileWeapon::ProjectileWeapon(const char* name, int damage, float fireRate,
    int maxAmmo, bool infiniteAmmo)
    : Weapon(name, damage, fireRate, maxAmmo, infiniteAmmo) {
}

void ProjectileWeapon::update(float dt)
{
    if (mFireCooldown > 0.f) mFireCooldown -= dt;
    advanceProjectiles(dt);
}


//////////////////////////////////////////////////PISTOL////////////////////////////////////////////////



Pistol::Pistol(Texture& tex)
    : ProjectileWeapon("pistol", 3, 6.f, 0, true), mTexture(tex)
{
    mBulletFrames[0] = IntRect(15, 82, 61, 10);
    mBulletFrames[1] = IntRect(82, 82, 60, 12);
    mBulletFrames[2] = IntRect(148, 80, 59, 16);

    for (int i = 0; i < MAX_PISTOL_BULLETS; i++)
    {
        mBullets[i].active = false;
        mBullets[i].sprite.setTexture(mTexture);
        mBullets[i].sprite.setTextureRect(mBulletFrames[0]);
    }
}

void Pistol::fire(float originX, float originY, bool facingRight, float aimAngle)
{
    if (!canFire()) return;
    for (int i = 0; i < MAX_PISTOL_BULLETS; i++)
    {
        if (!mBullets[i].active)
        {
            mBullets[i].active = true;
            mBullets[i].damage = mDamage;
            mBullets[i].currentFrame = 0;
            mBullets[i].frameTimer = 0.f;
            mBullets[i].vx = facingRight ? mBulletSpeed : -mBulletSpeed;
            mBullets[i].sprite.setPosition(originX, originY);
            mBullets[i].sprite.setScale(facingRight ? 0.5f : -0.5f, 0.5f);
            mBullets[i].sprite.setTextureRect(mBulletFrames[0]);
            mFireCooldown = 1.f / mFireRate;
            break;
        }
    }
}

void Pistol::advanceProjectiles(float dt)
{
    for (int i = 0; i < MAX_PISTOL_BULLETS; i++)
    {
        if (!mBullets[i].active) continue;

        mBullets[i].sprite.move(mBullets[i].vx, 0.f);

        mBullets[i].frameTimer += dt;
        if (mBullets[i].frameTimer >= 0.05f)
        {
            mBullets[i].frameTimer = 0.f;
            mBullets[i].currentFrame = (mBullets[i].currentFrame + 1) % BULLET_FRAMES;
            mBullets[i].sprite.setTextureRect(mBulletFrames[mBullets[i].currentFrame]);
        }

        float px = mBullets[i].sprite.getPosition().x;
        if (px < 0 || px > 10000)
        {
            mBullets[i].active = false;
            mBullets[i].currentFrame = 0;
            mBullets[i].frameTimer = 0.f;
        }
    }
}

void Pistol::resolveHits(Enemies** enemies, int count, int cellSize, int& score)
{
    for (int b = 0; b < MAX_PISTOL_BULLETS; b++)
    {
        if (!mBullets[b].active) continue;
        sf::FloatRect bRect = mBullets[b].sprite.getGlobalBounds();

        for (int e = 0; e < count; e++)
        {
            if (!enemies[e]->isAlive()) continue;
            if (bRect.intersects(enemies[e]->getBounds()))
            {
                enemies[e]->takeDamage(mDamage);
                mBullets[b].active = false;
                if (enemies[e]->wasJustKilled())
                {
                    score += enemies[e]->getScoreValue();
                    enemies[e]->clearKillFlag();
                }
                break;
            }
        }
    }
}

void Pistol::draw(sf::RenderWindow& window)
{
    for (int i = 0; i < MAX_PISTOL_BULLETS; i++)
        if (mBullets[i].active)
            window.draw(mBullets[i].sprite);
}


/////////////////////////////////////////////HEAVY MACHINE GUN///////////////////////////////////////////////

HeavyMachineGun::HeavyMachineGun(Texture& tex)
    : ProjectileWeapon("hmg", 3, 8.f, 200, false), mTexture(tex)
{
    // fill in IntRects from ammo.png bullet row
    mBulletFrames[0] = IntRect(15, 82, 61, 10);
    mBulletFrames[1] = IntRect(82, 82, 60, 12);
    mBulletFrames[2] = IntRect(148, 80, 59, 16);

    for (int i = 0; i < MAX_BULLETS; i++)
    {
        mBullets[i].active = false;
        mBullets[i].sprite.setTexture(mTexture);
        mBullets[i].sprite.setTextureRect(mBulletFrames[0]);
    }
}

void HeavyMachineGun::resolveHits(Enemies** enemies, int count, int cellSize, int& score)
{
    for (int b = 0; b < MAX_BULLETS; b++)
    {
        if (!mBullets[b].active) continue;
        sf::FloatRect bRect = mBullets[b].sprite.getGlobalBounds();

        for (int e = 0; e < count; e++)
        {
            if (!enemies[e]->isAlive()) continue;
            if (bRect.intersects(enemies[e]->getBounds()))
            {
                enemies[e]->takeDamage(mDamage);
                mBullets[b].active = false;
                if (enemies[e]->wasJustKilled())
                {
                    score += enemies[e]->getScoreValue();
                    enemies[e]->clearKillFlag();
                }
                break;
            }
        }
    }
}

void HeavyMachineGun::fire(float originX, float originY, bool facingRight, float aimAngle)
{
    if (!canFire() || isEmpty()) return;
    for (int i = 0; i < MAX_BULLETS; i++)
    {
        if (!mBullets[i].active)
        {
            mBullets[i].active = true;
            mBullets[i].damage = mDamage;
            mBullets[i].currentFrame = 0;
            mBullets[i].frameTimer = 0.f;
            mBullets[i].vx = facingRight ? mBulletSpeed : -mBulletSpeed;
            mBullets[i].sprite.setPosition(originX, originY);
            mBullets[i].sprite.setScale(facingRight ? 1.f : -1.f, 1.f);
            mBullets[i].sprite.setTextureRect(mBulletFrames[0]);
            mFireCooldown = 1.f / mFireRate;
            mAmmo--;
            break;
        }
    }
}

/*void HeavyMachineGun::fire(float originX, float originY, bool facingRight, float aimAngle)
{
    if (!canFire() || isEmpty()) return;
    mFireCooldown = 1.f / mFireRate;
    mAmmo--;
    std::cout << "HMG fired! Ammo:" << mAmmo << "\n";
}*/

void HeavyMachineGun::draw(RenderWindow& window)
{
    for (int i = 0; i < MAX_BULLETS; i++)
        if (mBullets[i].active) window.draw(mBullets[i].sprite);
}

//void HeavyMachineGun::draw(RenderWindow& /*window*/) {}
//void HeavyMachineGun::advanceProjectiles(float /*dt*/) {}
void HeavyMachineGun::advanceProjectiles(float dt)
{
    for (int i = 0; i < MAX_BULLETS; i++)
    {
        if (!mBullets[i].active) continue;

        mBullets[i].sprite.move(mBullets[i].vx, 0.f);

        mBullets[i].frameTimer += dt;
        if (mBullets[i].frameTimer >= 0.05f)
        {
            mBullets[i].frameTimer = 0.f;
            mBullets[i].currentFrame = (mBullets[i].currentFrame + 1) % BULLET_FRAMES;
            mBullets[i].sprite.setTextureRect(mBulletFrames[mBullets[i].currentFrame]);
        }

        float px = mBullets[i].sprite.getPosition().x;
        if (px < 0 || px > 10000)
        {
            mBullets[i].active = false;
            mBullets[i].currentFrame = 0;
            mBullets[i].frameTimer = 0.f;
        }
        // hook: enemy collision -> deal mDamage -> deactivate
    }
}




////////////////////////////////////////ROCKET LAUNCHER//////////////////////////////////////////////////////

RocketLauncher::RocketLauncher(Texture& tex)
    : ProjectileWeapon("rocket", 5, 0.5f, 10, false), mTexture(tex)
{
    // fill in IntRects from ammo.png rocket row
    mRocketFrames[0] = IntRect(15, 186, 120, 30);
    mRocketFrames[1] = IntRect(140, 186, 103, 30);
    mRocketFrames[2] = IntRect(248, 186, 120, 30);
    //mRocketFrames[3] = IntRect(0, 0, 0, 0);

    for (int i = 0; i < MAX_ROCKETS; i++)
    {
        mRockets[i].active = false;
        mRockets[i].currentFrame = 0;
        mRockets[i].frameTimer = 0.f;
        mRockets[i].sprite.setTexture(mTexture);
        mRockets[i].sprite.setTextureRect(mRocketFrames[0]);
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
            mRockets[i].sprite.setScale(facingRight ? 1.f : -1.f, 1.f);
            mRockets[i].vx = facingRight ? mRocketSpeed * std::cos(rad)
                : -mRocketSpeed * std::cos(rad);
            mRockets[i].vy = -mRocketSpeed * std::sin(rad);
            mFireCooldown = 1.f / mFireRate;
            mAmmo--;
            break;
        }
    }
}

void RocketLauncher::resolveHits(Enemies** enemies, int count, int cellSize, int& score)
{
    for (int r = 0; r < MAX_ROCKETS; r++)
    {
        if (!mRockets[r].active) continue;
        sf::FloatRect rRect = mRockets[r].sprite.getGlobalBounds();

        for (int e = 0; e < count; e++)
        {
            if (!enemies[e]->isAlive()) continue;
            if (!rRect.intersects(enemies[e]->getBounds())) continue;

            float rx = mRockets[r].sprite.getPosition().x;
            float ry = mRockets[r].sprite.getPosition().y;
            float blastR = 3.f * (float)cellSize;
            int   killCount = 0;

            for (int e2 = 0; e2 < count; e2++)
            {
                if (!enemies[e2]->isAlive()) continue;
                sf::Vector2f ep = enemies[e2]->getSprite().getPosition();
                float dx = ep.x - rx;
                float dy = ep.y - ry;
                if (dx * dx + dy * dy <= blastR * blastR)
                {
                    enemies[e2]->takeDamage(mDamage);
                    if (enemies[e2]->wasJustKilled())
                    {
                        score += enemies[e2]->getScoreValue();
                        enemies[e2]->clearKillFlag();
                        killCount++;
                    }
                }
            }
            mRockets[r].active = false;

            if (killCount == 2)  score += 200;
            else if (killCount >= 3)  score += 300 + (killCount - 3) * 50;
            break;
        }
    }
}

void RocketLauncher::draw(RenderWindow& window)
{
    for (int i = 0; i < MAX_ROCKETS; i++)
        if (mRockets[i].active) window.draw(mRockets[i].sprite);
}
/*
void RocketLauncher::advanceProjectiles(float /*dt*/
/* {
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
*/

void RocketLauncher::advanceProjectiles(float dt)
{
    for (int i = 0; i < MAX_ROCKETS; i++)
    {
        if (!mRockets[i].active) continue;

        mRockets[i].sprite.move(mRockets[i].vx, mRockets[i].vy);

        mRockets[i].frameTimer += dt;
        if (mRockets[i].frameTimer >= 0.05f)
        {
            mRockets[i].frameTimer = 0.f;
            mRockets[i].currentFrame = (mRockets[i].currentFrame + 1) % ROCKET_FRAMES;
            mRockets[i].sprite.setTextureRect(mRocketFrames[mRockets[i].currentFrame]);
        }

        float px = mRockets[i].sprite.getPosition().x;
        float py = mRockets[i].sprite.getPosition().y;
        if (px < 0 || px > 10000 || py < 0 || py > 10000)
        {
            mRockets[i].active = false;
            mRockets[i].currentFrame = 0;
            mRockets[i].frameTimer = 0.f;
        }
        // hook: tile/enemy collision -> explode -> AOE damage
    }
}



//////////////////////////////////////////FLAME SHOT//////////////////////////////////////////////////////////////

FlameShot::FlameShot(Texture& tex)
    : Weapon("flameshot", 2, 30.f, 300, false), mTexture(tex)
{
    // fill in IntRects from ammo.png flame row
    mFrames[0] = IntRect(14, 131, 52, 29);
    mFrames[1] = IntRect(71, 124, 66, 45);
    mFrames[2] = IntRect(154, 124, 87, 43);
    mFrames[3] = IntRect(255, 119, 104, 53);
    mFrames[4] = IntRect(364, 117, 110, 57);
    mFrames[5] = IntRect(479, 117, 114, 57);
    mFrames[6] = IntRect(607, 119, 108, 53);
    mFrames[7] = IntRect(721, 117, 115, 57);
    mFrames[8] = IntRect(841, 115, 116, 59);

    mSprite.setTexture(mTexture);
    mSprite.setTextureRect(mFrames[0]);
}

void FlameShot::resolveHits(Enemies** enemies, int count, int cellSize, int& score)
{
    if (!mFlameActive) return;

    float range = 5.f * (float)cellSize;

    for (int e = 0; e < count; e++)
    {
        if (!enemies[e]->isAlive()) continue;
        sf::Vector2f ep = enemies[e]->getSprite().getPosition();
        float dx = ep.x - mOriginX;
        float dy = ep.y - mOriginY;

        bool inDir = mFacingRight ? (dx >= 0.f && dx <= range)
            : (dx <= 0.f && dx >= -range);

        if (inDir && dy > -(float)cellSize && dy < (float)cellSize)
        {
            // 2 HP per second — multiply by dt approximation (1/60)
            enemies[e]->takeDamage(1);   // call this every frame; 60fps ≈ 60/sec, scale as needed
            if (enemies[e]->wasJustKilled())
            {
                score += enemies[e]->getScoreValue();
                enemies[e]->clearKillFlag();
            }
        }
    }
}

void FlameShot::fire(float originX, float originY, bool facingRight, float aimAngle)
{
    if (isEmpty()) return;
    mFlameActive = true;
    mOriginX = originX;
    mOriginY = originY;
    mFacingRight = facingRight;
    mAmmo--;
}


void FlameShot::update(float dt)
{
    if (mFireCooldown > 0.f) mFireCooldown -= dt;

    if (mFlameActive)
    {
        mFrameTimer += dt;
        if (mFrameTimer >= 0.05f)
        {
            mFrameTimer = 0.f;
            mCurrentFrame = (mCurrentFrame + 1) % FLAME_FRAMES;
            mSprite.setTextureRect(mFrames[mCurrentFrame]);
        }
    }
    else
    {
        mCurrentFrame = 0;
        mSprite.setTextureRect(mFrames[0]);
    }
}
/*
void FlameShot::update(float dt)
{
    if (mFireCooldown > 0.f) mFireCooldown -= dt;
    // mFlameActive is reset each frame by the input handler (only true while key held)
}*/

//void FlameShot::draw(RenderWindow& /*window*/)
//{
    // hook: draw flame sprite stream while mFlameActive
//}

void FlameShot::draw(RenderWindow& window)
{
    if (!mFlameActive) return;

    float frameW = (float)mFrames[mCurrentFrame].width;
    for (int i = 0; i < (int)mRange; i++)
    {
        float offsetX = mFacingRight ? (i * frameW) : -(i * frameW);
        mSprite.setPosition(mOriginX + offsetX, mOriginY);
        mSprite.setScale(mFacingRight ? 1.f : -1.f, 1.f);
        window.draw(mSprite);
    }
}



////////////////////////////////////////////LASER GUN////////////////////////////////////////////////////

LaserGun::LaserGun(Texture& tex)
    : Weapon("laserbeam", 9999, 1.f, 30, false), mTexture(tex)
{
    // fill in IntRects from ammo.png laser row
    mFrames[0] = IntRect(19, 16, 25, 25);
    mFrames[1] = IntRect(49, 26, 39, 5);
    mFrames[2] = IntRect(95, 21, 25, 15);
    mFrames[3] = IntRect(49, 26, 39, 5);
    mFrames[4] = IntRect(49, 26, 39, 5);
    mFrames[5] = IntRect(49, 26, 39, 5);

    mSprite.setTexture(mTexture);
    mSprite.setTextureRect(mFrames[0]);
}

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
/*
void LaserGun::draw(RenderWindow& window)
{
    if (!mBeamActive) return;

    int frame = (int)((mBeamTimer / mBeamDuration) * LASER_FRAMES);
    if (frame >= LASER_FRAMES) frame = LASER_FRAMES - 1;
    mSprite.setTextureRect(mFrames[frame]);

    float frameW = (float)mFrames[frame].width;
    float x = mFacingRight ? mOriginX : mOriginX - frameW;

    while ((mFacingRight && x < mOriginX + 2000.f) ||
        (!mFacingRight && x > mOriginX - 2000.f))
    {
        mSprite.setPosition(x, mOriginY);
        mSprite.setScale(mFacingRight ? 1.f : -1.f, 1.f);
        window.draw(mSprite);
        x += mFacingRight ? frameW : -frameW;
    }
}*/

void LaserGun::resolveHits(Enemies** enemies, int count, int cellSize, int& score)
{
    if (!mBeamActive) return;

    for (int e = 0; e < count; e++)
    {
        if (!enemies[e]->isAlive()) continue;
        sf::Vector2f ep = enemies[e]->getSprite().getPosition();
        float dx = ep.x - mOriginX;
        float dy = ep.y - mOriginY;

        bool inDir = mFacingRight ? (dx >= 0.f) : (dx <= 0.f);

        if (inDir && dy > -(float)cellSize && dy < (float)cellSize)
        {
            enemies[e]->takeDamage(9999);
            if (enemies[e]->wasJustKilled())
            {
                score += enemies[e]->getScoreValue();
                enemies[e]->clearKillFlag();
            }
        }
    }
}

void LaserGun::draw(RenderWindow& window)
{
    if (!mBeamActive) return;

    int frame = (int)((mBeamTimer / mBeamDuration) * LASER_FRAMES);
    if (frame >= LASER_FRAMES) frame = LASER_FRAMES - 1;
    mSprite.setTextureRect(mFrames[frame]);

    float frameW = (float)mFrames[frame].width;

    // start right at the origin, tile until map edge
    float x = mOriginX;

    if (mFacingRight)
    {
        while (x < mMapPixelWidth)
        {
            mSprite.setPosition(x, mOriginY);
            mSprite.setScale(1.f, 1.f);
            window.draw(mSprite);
            x += frameW;
        }
    }
    else
    {
        x = mOriginX - frameW;
        while (x + frameW > 0.f)
        {
            mSprite.setPosition(x, mOriginY);
            mSprite.setScale(-1.f, 1.f);
            window.draw(mSprite);
            x -= frameW;
        }
    }
}


/*void LaserGun::draw(RenderWindow& /*window*/
/* {
    // hook: draw beam line from origin to screen edge while mBeamActive
}*/



/////////////////////////////////////////HAND GRENADE////////////////////////////////////////////////////////////////


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
      
    }
}



///////////////////////////////////////FIRE BOMB GRENADE//////////////////////////////////////////////////////

FireBombGrenade::FireBombGrenade(Texture& tex) : HandGrenade(tex)
{
    for (int i = 0; i < MAX_FIRE_POOLS; i++)
        mFirePools[i].active = false;
    mDamage = 5;
    
    const char* n = "firebomb";
    int j = 0;
    while (n[j] && j < 31) { mName[j] = n[j]; j++; }
    mName[j] = '\0';
}

void FireBombGrenade::fire(float originX, float originY, bool facingRight, float aimAngle)
{
    HandGrenade::fire(originX, originY, facingRight, aimAngle);
  
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
}