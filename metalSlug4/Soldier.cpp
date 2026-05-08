/*#include "Soldier.h"
#include <SFML/Window/Keyboard.hpp>
#include <cstring>

// ================================================================
//  Tile helper
// ================================================================
static bool isTile(char** lvl, int x, int y, int mapWidth, int mapHeight)
{
    if (x < 0 || x >= mapWidth || y < 0 || y >= mapHeight) return false;
    return lvl[y][x] == 'g';
}

// ================================================================
//  SOLDIER BASE
// ================================================================
Soldier::Soldier(float spd, int hp)
    : speed(spd), health(hp),
    mcurrentAnim(IDLE),
    mCurrentWeaponAnim(W_IDLE),
    mWeaponTextureLoaded(false)
{
}

void Soldier::setPosition(float x, float y)
{
    msprite.setPosition(x, y);
}

Sprite& Soldier::getSprite() { return msprite; }
Sprite& Soldier::getWeaponSprite() { return mWeaponSprite; }

void Soldier::equipWeapon(Weapon* w)
{
    delete m_weapon;
    m_weapon = w;
}

void Soldier::setAnimation(AnimType type)
{
    if (mcurrentAnim == type && !manimations[type].isFinished()) return;
    mcurrentAnim = type;
    manimations[type].reset();
    manimations[type].play();
}

void Soldier::setWeaponAnimation(WeaponAnimType type)
{
    if (mCurrentWeaponAnim == type && !mWeaponAnims[type].isFinished()) return;
    mCurrentWeaponAnim = type;
    mWeaponAnims[type].reset();
    mWeaponAnims[type].play();
}

void Soldier::syncWeaponSprite()
{
    // Keep weapon overlay at the same position & facing as the body
    mWeaponSprite.setPosition(msprite.getPosition());
    if (facingRight)
        mWeaponSprite.setScale(baseScale, baseScale);
    else
        mWeaponSprite.setScale(-baseScale, baseScale);
}

void Soldier::update(float dt, char** lvl, int cell_size, int mapWidth, int mapHeight)
{
    // ---- GRAVITY ----
    if (!onGround)
    {
        velocityY += gravity;
        if (velocityY > 20.f) velocityY = 20.f;
    }
    else
    {
        velocityY = 0.f;
    }

    float bx = msprite.getPosition().x + collisionOffsetX;
    float by = msprite.getPosition().y + collisionOffsetY;
    float bw = collisionWidth;
    float bh = collisionHeight;

    // ---- MOVE X ----
    msprite.move(velocityX, 0);
    bx = msprite.getPosition().x + collisionOffsetX;

    int L = (int)(bx / cell_size);
    int R = (int)((bx + bw - 1) / cell_size);
    int T = (int)(by / cell_size);
    int B = (int)((by + bh - 1) / cell_size);

    if (L < 0) L = 0; else if (L >= mapWidth)  L = mapWidth - 1;
    if (R < 0) R = 0; else if (R >= mapWidth)  R = mapWidth - 1;
    if (T < 0) T = 0; else if (T >= mapHeight) T = mapHeight - 1;
    if (B < 0) B = 0; else if (B >= mapHeight) B = mapHeight - 1;

    if (velocityX > 0)
    {
        for (int i = T; i <= B; i++)
        {
            if (isTile(lvl, R, i, mapWidth, mapHeight))
            {
                msprite.setPosition(R * cell_size - bw - collisionOffsetX, msprite.getPosition().y);
                velocityX = 0; bx = msprite.getPosition().x + collisionOffsetX; break;
            }
        }
    }
    else if (velocityX < 0)
    {
        for (int i = T; i <= B; i++)
        {
            if (isTile(lvl, L, i, mapWidth, mapHeight))
            {
                msprite.setPosition((L + 1) * cell_size - collisionOffsetX, msprite.getPosition().y);
                velocityX = 0; bx = msprite.getPosition().x + collisionOffsetX; break;
            }
        }
    }

    // ---- MOVE Y ----
    msprite.move(0, velocityY);
    by = msprite.getPosition().y + collisionOffsetY;

    L = (int)(bx / cell_size);
    R = (int)((bx + bw - 1) / cell_size);
    T = (int)(by / cell_size);
    B = (int)((by + bh - 1) / cell_size);

    if (L < 0) L = 0; else if (L >= mapWidth)  L = mapWidth - 1;
    if (R < 0) R = 0; else if (R >= mapWidth)  R = mapWidth - 1;
    if (T < 0) T = 0; else if (T >= mapHeight) T = mapHeight - 1;
    if (B < 0) B = 0; else if (B >= mapHeight) B = mapHeight - 1;

    onGround = false;

    if (velocityY >= 0)
    {
        for (int j = L; j <= R; j++)
        {
            if (isTile(lvl, j, B, mapWidth, mapHeight))
            {
                msprite.setPosition(msprite.getPosition().x, B * cell_size - bh - collisionOffsetY);
                velocityY = 0; onGround = true; isJumping = false;
                by = msprite.getPosition().y + collisionOffsetY; break;
            }
        }
    }
    else if (velocityY < 0)
    {
        for (int j = L; j <= R; j++)
        {
            if (isTile(lvl, j, T, mapWidth, mapHeight))
            {
                msprite.setPosition(msprite.getPosition().x, (T + 1) * cell_size - collisionOffsetY);
                velocityY = 0; isJumping = false;
                by = msprite.getPosition().y + collisionOffsetY; break;
            }
        }
    }

    // ---- CLAMP ----
    bx = msprite.getPosition().x + collisionOffsetX;
    by = msprite.getPosition().y + collisionOffsetY;
    float cx = bx, cy = by;
    if (cx < 0.f) cx = 0.f;
    else if (cx > mapWidth * cell_size - bw) cx = mapWidth * cell_size - bw;
    if (cy < 0.f) cy = 0.f;
    else if (cy > mapHeight * cell_size - bh) { cy = mapHeight * cell_size - bh; velocityY = 0; onGround = true; isJumping = false; }
    msprite.setPosition(cx - collisionOffsetX, cy - collisionOffsetY);

    // ---- BODY ANIMATION ----
    bool isRunning = (velocityX != 0.f);
    if (isJumping)      setAnimation(isRunning ? JUMP_RUN : JUMP_IDLE);
    else if (isRunning) setAnimation(WALK);
    else                setAnimation(IDLE);

    manimations[mcurrentAnim].update(dt);
    msprite.setTextureRect(manimations[mcurrentAnim].getCurrentFrame());
    msprite.setScale(facingRight ? baseScale : -baseScale, baseScale);

    // ---- WEAPON OVERLAY ANIMATION ----
    if (mWeaponTextureLoaded)
    {
        mWeaponAnims[mCurrentWeaponAnim].update(dt);
        mWeaponSprite.setTextureRect(mWeaponAnims[mCurrentWeaponAnim].getCurrentFrame());
        syncWeaponSprite();
    }

    // ---- UPDATE EQUIPPED WEAPON ----
    if (m_weapon) m_weapon->update(dt);
}

// ================================================================
//  PLAYER SOLDIER
// ================================================================
PlayerSoldier::PlayerSoldier(int index) : Soldier(3.f, 100), mplayerIndex(index)
{
    loadPlayerData();
    setAnimation(IDLE);
    equipWeapon(new Pistol());
}

void PlayerSoldier::update(float dt, char** lvl, int cell_size, int mapWidth, int mapHeight)
{
    handleInput(dt);
    Soldier::update(dt, lvl, cell_size, mapWidth, mapHeight);
}

// ================================================================
//  INPUT
// ================================================================
void PlayerSoldier::handleInput(float dt)
{
    velocityX = 0;
    mIsShooting = false;

    // --- Fire key (Space for all players for now) ---
    bool fireKey = Keyboard::isKeyPressed(Keyboard::Space);

    if (mplayerIndex == 0)
    {
        if (Keyboard::isKeyPressed(Keyboard::Right)) { velocityX = speed; facingRight = true; }
        if (Keyboard::isKeyPressed(Keyboard::Left)) { velocityX = -speed; facingRight = false; }
        if (Keyboard::isKeyPressed(Keyboard::Up) && onGround)
        {
            velocityY = jumpStrength; onGround = false; isJumping = true;
        }
        fireKey = Keyboard::isKeyPressed(Keyboard::RControl);
    }
    else if (mplayerIndex == 1)
    {
        if (Keyboard::isKeyPressed(Keyboard::D)) { velocityX = speed; facingRight = true; }
        if (Keyboard::isKeyPressed(Keyboard::A)) { velocityX = -speed; facingRight = false; }
        if (Keyboard::isKeyPressed(Keyboard::W) && onGround)
        {
            velocityY = jumpStrength; onGround = false; isJumping = true;
        }
        fireKey = Keyboard::isKeyPressed(Keyboard::LControl);

        if (onGround) coyoteTime = coyoteMax;
        else          coyoteTime -= dt;
    }
    // Players 2 & 3 use same body as player 1 for now

    // --- Fire ---
    if (fireKey && m_weapon)
    {
        mIsShooting = true;
        float ox = msprite.getPosition().x + (facingRight ? collisionWidth : 0.f);
        float oy = msprite.getPosition().y + collisionHeight * 0.4f;
        m_weapon->fire(ox, oy, facingRight, 0.f);
    }

    // --- Select weapon animation ---
    if (mWeaponTextureLoaded)
    {
        bool airborne = isJumping || !onGround;
        bool moving = (velocityX != 0.f);

        if (mIsShooting)
        {
            if (airborne) setWeaponAnimation(W_SHOOT_JUMP);
            else if (moving)   setWeaponAnimation(W_SHOOT_WALK);
            else               setWeaponAnimation(W_SHOOT_IDLE);
        }
        else
        {
            if (airborne) setWeaponAnimation(W_JUMP);
            else if (moving)   setWeaponAnimation(W_WALK);
            else               setWeaponAnimation(W_IDLE);
        }
    }
}

// ================================================================
//  LOAD PLAYER DATA  (body texture + base animations)
// ================================================================
void PlayerSoldier::loadPlayerData()
{
    if (mplayerIndex == 0)
    {
        mtexture.loadFromFile("Sprites/Group 10.png");
        setupPlayer4Animations();
        baseScale = 2.5f;
    }
    else if (mplayerIndex == 1)
    {
        mtexture.loadFromFile("Sprites/Group 8.png");
        setupPlayer2Animations();
        baseScale = 1.65f;
    }
    else if (mplayerIndex == 2)
    {
        mtexture.loadFromFile("Sprites/Group 5.png");
        setupPlayer1Animations();
        baseScale = 1.f;
    }
    else if (mplayerIndex == 3)
    {
        mtexture.loadFromFile("Sprites/Group 9.png");
        setupPlayer3Animations();
        baseScale = 1.25f;
    }
    msprite.setTexture(mtexture);
}

// ================================================================
//  EQUIP WEAPON  — swap weapon + reload matching overlay sheet
// ================================================================
void PlayerSoldier::equipWeapon(Weapon* w)
{
    Soldier::equipWeapon(w);
    if (!m_weapon) return;

    const char* name = m_weapon->getName();
    mWeaponTextureLoaded = false;   // default: no overlay until sheet loads

    // Dispatch to per-player × per-weapon setup function
    if (mplayerIndex == 0)
    {
        if (strcmp(name, "pistol") == 0) setupPlayer1Pistol();
        else if (strcmp(name, "hmg") == 0) setupPlayer1HMG();
        else if (strcmp(name, "flameshot") == 0) setupPlayer1Flameshot();
        else if (strcmp(name, "laserbeam") == 0) setupPlayer1Laser();
        else if (strcmp(name, "rocket") == 0) setupPlayer1RL();
    }
    else if (mplayerIndex == 1)
    {
        if (strcmp(name, "pistol") == 0) setupPlayer2Pistol();
        else if (strcmp(name, "hmg") == 0) setupPlayer2HMG();
        else if (strcmp(name, "flameshot") == 0) setupPlayer2Flameshot();
        else if (strcmp(name, "laserbeam") == 0) setupPlayer2Laser();
        else if (strcmp(name, "rocket") == 0) setupPlayer2RL();
    }
    else if (mplayerIndex == 2)
    {
        if (strcmp(name, "pistol") == 0) setupPlayer3Pistol();
        else if (strcmp(name, "hmg") == 0) setupPlayer3HMG();
        else if (strcmp(name, "flameshot") == 0) setupPlayer3Flameshot();
        else if (strcmp(name, "laserbeam") == 0) setupPlayer3Laser();
        else if (strcmp(name, "rocket") == 0) setupPlayer3RL();
    }
    else if (mplayerIndex == 3)
    {
        if (strcmp(name, "pistol") == 0) setupPlayer4Pistol();
        else if (strcmp(name, "hmg") == 0) setupPlayer4HMG();
        else if (strcmp(name, "flameshot") == 0) setupPlayer4Flameshot();
        else if (strcmp(name, "laserbeam") == 0) setupPlayer4Laser();
        else if (strcmp(name, "rocket") == 0) setupPlayer4RL();
    }

    if (mWeaponTextureLoaded)
    {
        mWeaponSprite.setTexture(mWeaponTexture);
        setWeaponAnimation(W_IDLE);
    }
}

// ================================================================
//  BODY ANIMATIONS  (unchanged from original)
// ================================================================
void PlayerSoldier::setupPlayer1Animations()
{
    IntRect idle[4] = { IntRect(90,4,83,101), IntRect(174,4,84,101), IntRect(264,4,81,101), IntRect(346,4,81,101) };
    IntRect walk[12] = { IntRect(88,126,79,90), IntRect(168,126,78,92), IntRect(246,126,78,92), IntRect(325,128,77,99), IntRect(402,128,77,99), IntRect(485,128,77,99), IntRect(567,130,77,97), IntRect(647,130,77,99), IntRect(727,130,79,97), IntRect(811,130,78,97), IntRect(889,130,78,97), IntRect(971,130,74,97) };
    IntRect jumpIdle[6] = { IntRect(88,238,66,106), IntRect(157,242,70,102), IntRect(232,244,71,98), IntRect(308,242,69,98), IntRect(384,242,69,92), IntRect(459,244,67,84) };
    IntRect jumpRun[6] = { IntRect(86,362,73,97), IntRect(159,362,73,97), IntRect(239,362,79,88), IntRect(318,360,73,90), IntRect(391,360,73,90), IntRect(464,360,73,90) };
    IntRect swimIdle[6] = { IntRect(22,803,136,77), IntRect(165,801,139,75), IntRect(312,812,137,67), IntRect(457,812,140,64), IntRect(598,810,141,64), IntRect(758,808,140,64) };
    IntRect swim[10] = { IntRect(34,916,132,74), IntRect(176,911,130,67), IntRect(314,916,133,61), IntRect(456,906,139,64), IntRect(600,908,138,61), IntRect(756,905,140,60), IntRect(898,900,138,66), IntRect(1054,895,136,66), IntRect(1196,889,130,77), IntRect(1337,890,131,76) };
    IntRect die[21] = { IntRect(78,687,72,93), IntRect(155,694,79,86), IntRect(239,694,83,86), IntRect(328,692,68,88), IntRect(399,685,69,95), IntRect(471,682,74,98), IntRect(548,680,71,100), IntRect(622,680,68,100), IntRect(696,680,61,100), IntRect(761,687,66,93), IntRect(832,690,67,90), IntRect(902,694,71,86), IntRect(976,706,78,74), IntRect(1057,704,74,76), IntRect(1136,706,69,74), IntRect(1208,709,66,71), IntRect(1277,713,69,67), IntRect(1351,718,69,62), IntRect(1426,728,71,52), IntRect(1500,742,88,38), IntRect(1588,740,84,40) };
    manimations[IDLE].setup("idle", idle, 4, 6.f, true);
    manimations[WALK].setup("walk", walk, 12, 20.f, true);
    manimations[SWIM_IDLE].setup("swim_idle", swimIdle, 6, 8.f, false);
    manimations[SWIM].setup("swim", swim, 10, 8.f, false);
    manimations[JUMP_IDLE].setup("jump_idle", jumpIdle, 6, 6.f, false);
    manimations[JUMP_RUN].setup("jump_run", jumpRun, 6, 6.f, false);
    manimations[DIE].setup("die", die, 21, 5.f, false);
}

void PlayerSoldier::setupPlayer2Animations()
{
    IntRect idle[4] = { IntRect(8,10,48,60), IntRect(61,10,48,61), IntRect(115,10,49,61), IntRect(170,10,51,61) };
    IntRect walk[6] = { IntRect(2,82,48,67), IntRect(58,82,49,67), IntRect(115,82,48,67), IntRect(170,82,50,67), IntRect(223,82,52,67), IntRect(281,82,50,67) };
    IntRect jumpIdle[6] = { IntRect(3,169,47,70), IntRect(53,170,48,69), IntRect(107,172,48,67), IntRect(158,173,48,66), IntRect(213,173,47,63), IntRect(264,173,47,62) };
    IntRect jumpRun[6] = { IntRect(329,170,48,66), IntRect(382,170,48,63), IntRect(436,170,55,60), IntRect(496,169,49,58), IntRect(549,169,49,58), IntRect(607,169,43,60) };
    IntRect die[19] = { IntRect(4,253,43,60), IntRect(58,256,49,55), IntRect(113,257,47,56), IntRect(169,256,46,57), IntRect(226,254,48,56), IntRect(283,260,42,50), IntRect(332,260,44,50), IntRect(389,263,47,48), IntRect(446,268,53,43), IntRect(510,266,56,42), IntRect(572,271,50,34), IntRect(631,265,48,40), IntRect(687,265,40,42), IntRect(736,260,37,48), IntRect(778,265,39,45), IntRect(826,268,43,45), IntRect(874,278,48,35), IntRect(927,284,53,29), IntRect(985,289,57,24) };
    manimations[IDLE].setup("idle", idle, 4, 6.f, true);
    manimations[WALK].setup("walk", walk, 6, 15.f, true);
    manimations[JUMP_IDLE].setup("jump_idle", jumpIdle, 6, 6.f, false);
    manimations[JUMP_RUN].setup("jump_run", jumpRun, 6, 6.f, false);
    manimations[DIE].setup("die", die, 19, 5.f, false);
}

void PlayerSoldier::setupPlayer3Animations()
{
    IntRect idle[4] = { IntRect(0,1,63,75), IntRect(68,1,61,75), IntRect(136,1,63,75), IntRect(206,1,66,75) };
    IntRect walk[6] = { IntRect(0,82,67,85), IntRect(67,82,67,85), IntRect(135,82,63,85), IntRect(195,82,63,67), IntRect(262,82,64,84), IntRect(326,82,67,83) };
    IntRect jumpIdle[6] = { IntRect(7,170,60,99), IntRect(75,172,60,98), IntRect(145,175,60,91), IntRect(213,175,60,87), IntRect(281,175,60,77), IntRect(349,175,60,78) };
    IntRect jumpRun[6] = { IntRect(1,272,61,94), IntRect(71,272,63,87), IntRect(143,272,73,78), IntRect(237,272,66,73), IntRect(310,272,68,75), IntRect(393,272,57,81) };
    IntRect die[19] = { IntRect(4,253,43,60), IntRect(58,256,49,55), IntRect(113,257,47,56), IntRect(169,256,46,57), IntRect(226,254,48,56), IntRect(283,260,42,50), IntRect(332,260,44,50), IntRect(389,263,47,48), IntRect(446,268,53,43), IntRect(510,266,56,42), IntRect(572,271,50,34), IntRect(631,265,48,40), IntRect(687,265,40,42), IntRect(736,260,37,48), IntRect(778,265,39,45), IntRect(826,268,43,45), IntRect(874,278,48,35), IntRect(927,284,53,29), IntRect(985,289,57,24) };
    manimations[IDLE].setup("idle", idle, 4, 6.f, true);
    manimations[WALK].setup("walk", walk, 6, 15.f, true);
    manimations[JUMP_IDLE].setup("jump_idle", jumpIdle, 6, 6.f, false);
    manimations[JUMP_RUN].setup("jump_run", jumpRun, 6, 6.f, false);
    manimations[DIE].setup("die", die, 19, 5.f, false);
}

void PlayerSoldier::setupPlayer4Animations()
{
    IntRect idle[4] = { IntRect(4,0,31,37), IntRect(37,0,31,38), IntRect(70,0,31,38), IntRect(103,0,31,38) };
    IntRect walk[14] = { IntRect(0,54,32,39), IntRect(32,54,32,39), IntRect(65,56,31,35), IntRect(96,55,32,37), IntRect(132,54,28,38), IntRect(164,54,29,39), IntRect(196,55,30,38), IntRect(227,56,33,39), IntRect(260,56,33,39), IntRect(293,56,33,39), IntRect(328,55,29,38), IntRect(359,55,30,38), IntRect(164,54,29,39), IntRect(196,55,30,38) };
    IntRect jumpIdle[6] = { IntRect(6,105,29,45), IntRect(40,107,30,43), IntRect(75,109,30,40), IntRect(110,109,30,39), IntRect(145,109,29,37), IntRect(178,109,29,35) };
    IntRect jumpRun[6] = { IntRect(9,163,29,43), IntRect(38,163,29,43), IntRect(71,163,34,32), IntRect(106,163,30,35), IntRect(138,163,30,36), IntRect(168,163,26,37) };
    IntRect die[19] = { IntRect(4,253,43,60), IntRect(58,256,49,55), IntRect(113,257,47,56), IntRect(169,256,46,57), IntRect(226,254,48,56), IntRect(283,260,42,50), IntRect(332,260,44,50), IntRect(389,263,47,48), IntRect(446,268,53,43), IntRect(510,266,56,42), IntRect(572,271,50,34), IntRect(631,265,48,40), IntRect(687,265,40,42), IntRect(736,260,37,48), IntRect(778,265,39,45), IntRect(826,268,43,45), IntRect(874,278,48,35), IntRect(927,284,53,29), IntRect(985,289,57,24) };
    manimations[IDLE].setup("idle", idle, 4, 6.f, true);
    manimations[WALK].setup("walk", walk, 14, 20.f, true);
    manimations[JUMP_IDLE].setup("jump_idle", jumpIdle, 6, 6.f, false);
    manimations[JUMP_RUN].setup("jump_run", jumpRun, 6, 6.f, false);
    manimations[DIE].setup("die", die, 19, 5.f, false);
}

// ================================================================
//  WEAPON OVERLAY SETUP  — one function per player x weapon
//
//  Same pattern as body animations:
//    - declare IntRect array with real pixel coords from your sheet
//    - call mWeaponAnims[W_*].setup(name, array, count, fps, looping)
//    - set mWeaponTextureLoaded = true at the end
//
//  All IntRect values below are (0,0,1,1) placeholders.
//  Replace them with the real coords from your weapon sprite sheets,
//  exactly as you filled in the body animation IntRects above.
// ================================================================

// ----------------------------------------------------------------
//  PLAYER 1  (Marco)
// ----------------------------------------------------------------

void PlayerSoldier::setupPlayer1Pistol()
{
    // Body sheet already covers idle/walk/jump with pistol.
    // Only load the attack overlay frames.
    if (!mWeaponTexture.loadFromFile("Sprites/p1_pistol.png")) return;

    IntRect wShootI[10] = { IntRect(43,13,108,78), IntRect(155,13,108,78), IntRect(267,13,110,78), IntRect(381,13,80,82) , IntRect(465,9,80,86) , IntRect(547,9,82,84) , IntRect(631,13,82,80), IntRect(715,13,76,80), IntRect(795,13,72,80), IntRect(869,11,72,82) };
    IntRect wMelee[6] = { IntRect(31,135,54,84), IntRect(89,135,52,84), IntRect(145,135,58,86), IntRect(207,137,88,82), IntRect(299,128,102,89), IntRect(403,128,104,89) };

    mWeaponAnims[W_SHOOT_IDLE].setup("w_s_idle", wShootI, 4, 12.f, false);
    mWeaponAnims[W_MELEE].setup("w_melee", wMelee, 6, 15.f, false);

    mWeaponTextureLoaded = true;
}

void PlayerSoldier::setupPlayer1HMG()
{
    if (!mWeaponTexture.loadFromFile("Sprites/p1_hmg.png")) return;

    IntRect wIdle[4] = { IntRect(45,247,78,100), IntRect(127,249,80,98), IntRect(211,251,82,96), IntRect(297,253,84,94) };
    IntRect wWalk[6] = { IntRect(32,370,86,104), IntRect(121,368,83,104), IntRect(208,368,80,105), IntRect(292,368,82,107), IntRect(374,372,88,102), IntRect(466,374,88,99) };
    IntRect wJump[5] = { IntRect(28,504,84,112), IntRect(202,502,80,112), IntRect(286,502,82,110), IntRect(372,506,84,100), IntRect(460,508,88,95) };
    IntRect wShootI[4] = { IntRect(19,646,135,79), IntRect(157,646,137,81), IntRect(299,646,133,81), IntRect(437,646,137,83) };

    IntRect wMelee[6] = { IntRect(31,135,54,84), IntRect(89,135,52,84), IntRect(145,135,58,86), IntRect(207,137,88,82), IntRect(299,128,102,89), IntRect(403,128,104,89) };

    mWeaponAnims[W_IDLE].setup("w_idle", wIdle, 4, 6.f, true);
    mWeaponAnims[W_WALK].setup("w_walk", wWalk, 6, 20.f, true);
    mWeaponAnims[W_JUMP].setup("w_jump", wJump, 4, 6.f, false);
    mWeaponAnims[W_SHOOT_IDLE].setup("w_s_idle", wShootI, 6, 24.f, true);

    mWeaponAnims[W_MELEE].setup("w_melee", wMelee, 6, 15.f, false);
    mWeaponTextureLoaded = true;
}

void PlayerSoldier::setupPlayer1Flameshot()
{
    if (!mWeaponTexture.loadFromFile("Sprites/p1_flame.png")) return;

    IntRect wIdle[4] = { IntRect(15,767,83,84), IntRect(99,767,83,86), IntRect(185,767,83,86), IntRect(271,767,85,84) };
    IntRect wWalk[6] = { IntRect(19,877,90,91), IntRect(113,875,85,91), IntRect(201,875,83,92), IntRect(286,887,88,92), IntRect(374,887,88,92), IntRect(467,877,90,90) };
    IntRect wJump[6] = { IntRect(19,985,90,99), IntRect(113,983,87,99), IntRect(201,983,83,101), IntRect(287,985,84,95), IntRect(375,985,88,89), IntRect(467,985,91,84) };
    IntRect wShootI[4] = { IntRect(19,1096,85,79), IntRect(105,1098,83,77), IntRect(189,1096,81,79), IntRect(271,1096,83,79) };
    IntRect wMelee[6] = { IntRect(31,135,54,84), IntRect(89,135,52,84), IntRect(145,135,58,86), IntRect(207,137,88,82), IntRect(299,128,102,89), IntRect(403,128,104,89) };
    //IntRect wMelee[4] = { IntRect(0,0,1,1), IntRect(0,0,1,1), IntRect(0,0,1,1), IntRect(0,0,1,1) };

    mWeaponAnims[W_IDLE].setup("w_idle", wIdle, 4, 6.f, true);
    mWeaponAnims[W_WALK].setup("w_walk", wWalk, 6, 20.f, true);
    mWeaponAnims[W_JUMP].setup("w_jump", wJump, 6, 6.f, false);
    mWeaponAnims[W_SHOOT_IDLE].setup("w_s_idle", wShootI, 4, 20.f, true);

    mWeaponAnims[W_MELEE].setup("w_melee", wMelee, 6, 15.f, false);
    mWeaponTextureLoaded = true;
}

void PlayerSoldier::setupPlayer1Laser()
{
    if (!mWeaponTexture.loadFromFile("Sprites/p1_laser.png")) return;

    IntRect wIdle[4] = { IntRect(21,1201,83,86), IntRect(104,1201,83,86), IntRect(191,1201,84,86), IntRect(277,1201,82,84) };
    IntRect wWalk[6] = { IntRect(25,1311,90,91), IntRect(119,1309,84,91), IntRect(207,1309,82,92), IntRect(292,1311,88,92), IntRect(380,1311,88,92), IntRect(473,1311,90,90) };
    IntRect wJump[6] = { IntRect(25,1419,90,99), IntRect(119,1417,84,99), IntRect(207,1417,82,101), IntRect(293,1419,84,95), IntRect(381,1419,88,89), IntRect(473,1419,90,84) };
    IntRect wShootI[4] = { IntRect(25,1530,84,80), IntRect(111,1532,80,78), IntRect(195,1530,78,80), IntRect(277,1530,85,80) };

    IntRect wMelee[6] = { IntRect(31,135,54,84), IntRect(89,135,52,84), IntRect(145,135,58,86), IntRect(207,137,88,82), IntRect(299,128,102,89), IntRect(403,128,104,89) };

    mWeaponAnims[W_IDLE].setup("w_idle", wIdle, 4, 6.f, true);
    mWeaponAnims[W_WALK].setup("w_walk", wWalk, 6, 20.f, true);
    mWeaponAnims[W_JUMP].setup("w_jump", wJump, 4, 6.f, false);
    mWeaponAnims[W_SHOOT_IDLE].setup("w_s_idle", wShootI, 4, 12.f, false);
    mWeaponAnims[W_MELEE].setup("w_melee", wMelee, 6, 15.f, false);
    mWeaponTextureLoaded = true;
}

void PlayerSoldier::setupPlayer1RL()
{
    if (!mWeaponTexture.loadFromFile("Sprites/p1_rocket.png")) return;

    IntRect wIdle[4] = { IntRect(24,1641,83,84), IntRect(110,1641,83,86), IntRect(197,1641,82,86), IntRect(282,1641,84,84) };
    IntRect wWalk[6] = { IntRect(31,1751,90,91), IntRect(125,1749,84,91), IntRect(212,1749,83,92), IntRect(296,1751,89,92), IntRect(385,1751,89,92), IntRect(479,1751,90,90) };
    IntRect wJump[6] = { IntRect(31,1859,90,99), IntRect(125,1857,84,99), IntRect(213,1857,82,101), IntRect(296,1859,87,95), IntRect(387,1859,88,89), IntRect(479,1859,92,84) };
    IntRect wShootI[4] = { IntRect(30,1970,83,80), IntRect(116,1972,81,78), IntRect(200,1970,79,80), IntRect(283,1970,85,80) };

    IntRect wMelee[6] = { IntRect(31,135,54,84), IntRect(89,135,52,84), IntRect(145,135,58,86), IntRect(207,137,88,82), IntRect(299,128,102,89), IntRect(403,128,104,89) };

    mWeaponAnims[W_IDLE].setup("w_idle", wIdle, 4, 6.f, true);
    mWeaponAnims[W_WALK].setup("w_walk", wWalk, 6, 20.f, true);
    mWeaponAnims[W_JUMP].setup("w_jump", wJump, 4, 6.f, false);
    mWeaponAnims[W_SHOOT_IDLE].setup("w_s_idle", wShootI, 4, 4.f, false);

    mWeaponAnims[W_MELEE].setup("w_melee", wMelee, 6, 15.f, false);
    mWeaponTextureLoaded = true;
}

// ----------------------------------------------------------------
//  PLAYER 2  (Tarma)
// ----------------------------------------------------------------

void PlayerSoldier::setupPlayer2Pistol()
{
    if (!mWeaponTexture.loadFromFile("Sprites/p2_pistol.png")) return;

    IntRect wShootI[9] = { IntRect(35,9,106,81), IntRect(147,11,108,79), IntRect(259,9,110,81), IntRect(373,7,80,83), IntRect(457,3,80,87), IntRect(541,3,80,85), IntRect(624,7,81,79), IntRect(707,7,74,81), IntRect(861,5,70,81) };
    IntRect wMelee[6] = { IntRect(29,115,54,83), IntRect(95,115,52,85), IntRect(157,115,58,85), IntRect(227,115,88,83), IntRect(325,107,102,91), IntRect(437,107,102,91) };

    mWeaponAnims[W_SHOOT_IDLE].setup("w_s_idle", wShootI, 4, 12.f, false);

    mWeaponAnims[W_MELEE].setup("w_melee", wMelee, 6, 15.f, false);
    mWeaponTextureLoaded = true;
}

void PlayerSoldier::setupPlayer2HMG()
{
    if (!mWeaponTexture.loadFromFile("Sprites/p2_hmg.png")) return;

    IntRect wIdle[4] = { IntRect(37,225,78,101), IntRect(129,227,80,99), IntRect(223,229,82,97), IntRect(313,231,84,93) };
    IntRect wWalk[9] = { IntRect(43,363,86,104), IntRect(43,363,86,104), IntRect(233,361,88,99), IntRect(337,361,82,106), IntRect(431,367,84,101), IntRect(527,369,88,99), IntRect(627,371,84,99), IntRect(716,367,89,103), IntRect(921,365,90,105) };
    IntRect wJump[6] = { IntRect(37,479,84,113), IntRect(135,477,82,113), IntRect(233,477,80,111), IntRect(329,477,82,109), IntRect(423,483,84,99) , IntRect(519,485,88,93) };
    IntRect wShootI[4] = { IntRect(40,628,133,80), IntRect(180,628,135,82), IntRect(322,628,133,82), IntRect(466,630,137,80) };

    IntRect wMelee[6] = { IntRect(29,115,54,83), IntRect(95,115,52,85), IntRect(157,115,58,85), IntRect(227,115,88,83), IntRect(325,107,102,91), IntRect(437,107,102,91) };

    //IntRect wMelee[4] = { IntRect(0,0,1,1), IntRect(0,0,1,1), IntRect(0,0,1,1), IntRect(0,0,1,1) };

    mWeaponAnims[W_IDLE].setup("w_idle", wIdle, 4, 6.f, true);
    mWeaponAnims[W_WALK].setup("w_walk", wWalk, 6, 20.f, true);
    mWeaponAnims[W_JUMP].setup("w_jump", wJump, 4, 6.f, false);
    mWeaponAnims[W_SHOOT_IDLE].setup("w_s_idle", wShootI, 6, 24.f, true);
    mWeaponAnims[W_MELEE].setup("w_melee", wMelee, 6, 15.f, false);
    mWeaponTextureLoaded = true;
}

void PlayerSoldier::setupPlayer2Flameshot()
{
    if (!mWeaponTexture.loadFromFile("Sprites/p2_flame.png")) return;

    IntRect wIdle[4] = { IntRect(26,741,83,79), IntRect(114,741,83,79), IntRect(206,741,83,83), IntRect(299,741,82,83) };
    IntRect wWalk[9] = { IntRect(31,845,92,86), IntRect(125,843,90,88), IntRect(217,841,88,83), IntRect(311,843,84,88), IntRect(403,843,88,89), IntRect(501,845,90,87), IntRect(899,845,90,89), IntRect(997,845,92,88), IntRect(1095,845,92,88) };
    IntRect wJump[6] = { IntRect(23,972,89,96), IntRect(121,970,84,96), IntRect(213,968,82,98), IntRect(301,970,83,96), IntRect(393,970,87,88), IntRect(491,972,89,84) };
    IntRect wShootI[4] = { IntRect(28,1071,83,80), IntRect(120,1073,79,78), IntRect(210,1071,77,80), IntRect(298,1071,81,80) };

    IntRect wMelee[6] = { IntRect(29,115,54,83), IntRect(95,115,52,85), IntRect(157,115,58,85), IntRect(227,115,88,83), IntRect(325,107,102,91), IntRect(437,107,102,91) };

    //IntRect wMelee[4] = { IntRect(0,0,1,1), IntRect(0,0,1,1), IntRect(0,0,1,1), IntRect(0,0,1,1) };

    mWeaponAnims[W_IDLE].setup("w_idle", wIdle, 4, 6.f, true);
    mWeaponAnims[W_WALK].setup("w_walk", wWalk, 6, 20.f, true);
    mWeaponAnims[W_JUMP].setup("w_jump", wJump, 4, 6.f, false);
    mWeaponAnims[W_SHOOT_IDLE].setup("w_s_idle", wShootI, 6, 20.f, true);

    mWeaponAnims[W_MELEE].setup("w_melee", wMelee, 6, 15.f, false);
    mWeaponTextureLoaded = true;
}

void PlayerSoldier::setupPlayer2Laser()
{
    if (!mWeaponTexture.loadFromFile("Sprites/p2_laser.png")) return;

    IntRect wIdle[4] = { IntRect(8,1179,83,79), IntRect(96,1179,85,79), IntRect(188,1179,83,83), IntRect(281,1179,84,83) };
    IntRect wWalk[9] = { IntRect(13,1283,92,86), IntRect(107,1281,90,88), IntRect(199,1279,88,83), IntRect(293,1281,84,88), IntRect(385,1281,88,89), IntRect(483,1283,90,87), IntRect(579,1283,90,89), IntRect(979,1283,92,88), IntRect(1077,1283,92,88) };
    IntRect wJump[6] = { IntRect(5,1410,89,96), IntRect(103,1408,84,96), IntRect(195,1406,81,98), IntRect(283,1408,83,96), IntRect(375,1408,87,88), IntRect(473,1410,89,84) };
    IntRect wShootI[4] = { IntRect(10,1509,83,80), IntRect(102,1511,79,78), IntRect(192,1509,79,80), IntRect(280,1509,81,80) };
    IntRect wMelee[6] = { IntRect(29,115,54,83), IntRect(95,115,52,85), IntRect(157,115,58,85), IntRect(227,115,88,83), IntRect(325,107,102,91), IntRect(437,107,102,91) };

    //IntRect wMelee[4] = { IntRect(0,0,1,1), IntRect(0,0,1,1), IntRect(0,0,1,1), IntRect(0,0,1,1) };

    mWeaponAnims[W_IDLE].setup("w_idle", wIdle, 4, 6.f, true);
    mWeaponAnims[W_WALK].setup("w_walk", wWalk, 6, 20.f, true);
    mWeaponAnims[W_JUMP].setup("w_jump", wJump, 4, 6.f, false);
    mWeaponAnims[W_SHOOT_IDLE].setup("w_s_idle", wShootI, 4, 12.f, false);
    mWeaponAnims[W_MELEE].setup("w_melee", wMelee, 6, 15.f, false);
    mWeaponTextureLoaded = true;
}

void PlayerSoldier::setupPlayer2RL()
{
    if (!mWeaponTexture.loadFromFile("Sprites/p2_rocket.png")) return;

    IntRect wIdle[4] = { IntRect(12,1609,81,79), IntRect(100,1609,83,79), IntRect(192,1609,83,83), IntRect(284,1609,83,83) };
    IntRect wWalk[8] = { IntRect(17,1713,92,86), IntRect(111,1711,91,88), IntRect(203,1709,88,83), IntRect(294,1711,87,88), IntRect(389,1711,88,89), IntRect(885,1713,90,89), IntRect(983,1713,92,88), IntRect(1081,1713,92,88) };
    IntRect wJump[6] = { IntRect(9,1840,89,96), IntRect(106,1838,84,96), IntRect(196,1836,84,98), IntRect(282,1838,88,96), IntRect(379,1838,87,88), IntRect(477,1840,89,84) };
    IntRect wShootI[4] = { IntRect(12,1939,83,80), IntRect(104,1941,82,78), IntRect(192,1939,84,80), IntRect(279,1939,88,80) };
    IntRect wMelee[6] = { IntRect(29,115,54,83), IntRect(95,115,52,85), IntRect(157,115,58,85), IntRect(227,115,88,83), IntRect(325,107,102,91), IntRect(437,107,102,91) };


    //IntRect wMelee[4] = { IntRect(0,0,1,1), IntRect(0,0,1,1), IntRect(0,0,1,1), IntRect(0,0,1,1) };

    mWeaponAnims[W_IDLE].setup("w_idle", wIdle, 4, 6.f, true);
    mWeaponAnims[W_WALK].setup("w_walk", wWalk, 6, 20.f, true);
    mWeaponAnims[W_JUMP].setup("w_jump", wJump, 4, 6.f, false);
    mWeaponAnims[W_SHOOT_IDLE].setup("w_s_idle", wShootI, 4, 4.f, false);

    mWeaponAnims[W_MELEE].setup("w_melee", wMelee, 6, 15.f, false);
    mWeaponTextureLoaded = true;
}

// ----------------------------------------------------------------
//  PLAYER 3  (Eri)
// ----------------------------------------------------------------

void PlayerSoldier::setupPlayer3Pistol()
{
    if (!mWeaponTexture.loadFromFile("Sprites/p3_pistol.png")) return;

    IntRect wShootI[9] = { IntRect(81,11,108,80), IntRect(195,9,112,82), IntRect(311,11,74,80), IntRect(429,11,70,80), IntRect(505,9,68,82), IntRect(583,13,78,78), IntRect(829,11,78,80) };

    mWeaponAnims[W_SHOOT_IDLE].setup("w_s_idle", wShootI, 4, 12.f, false);
    mWeaponTextureLoaded = true;
}

void PlayerSoldier::setupPlayer3HMG()
{
    if (!mWeaponTexture.loadFromFile("Sprites/p3_hmg.png")) return;

    IntRect wIdle[4] = { IntRect(69,135,76,96), IntRect(157,133,76,98), IntRect(245,131,74,100), IntRect(329,129,74,102) };
    IntRect wWalk[12] = { IntRect(68,256,77,95), IntRect(154,258,77,92), IntRect(240,262,75,90), IntRect(326,264,75,87), IntRect(410,262,77,89), IntRect(494,262,81,95), IntRect(582,262,77,93), IntRect(670,260,77,96), IntRect(760,256,79,101), IntRect(852,258,81,97), IntRect(940,262,81,91), IntRect(1023,258,82,98) };
    IntRect wJump[6] = { IntRect(58,370,77,108), IntRect(144,372,77,104), IntRect(230,376,75,100), IntRect(316,378,75,96), IntRect(400,376,77,93), IntRect(486,376,79,87) };
    IntRect wShootI[4] = { IntRect(45,607,88,82), IntRect(133,607,88,82), IntRect(221,613,90,84), IntRect(313,615,90,82) };

    mWeaponAnims[W_IDLE].setup("w_idle", wIdle, 4, 6.f, true);
    mWeaponAnims[W_WALK].setup("w_walk", wWalk, 6, 20.f, true);
    mWeaponAnims[W_JUMP].setup("w_jump", wJump, 4, 6.f, false);
    mWeaponAnims[W_SHOOT_IDLE].setup("w_s_idle", wShootI, 6, 24.f, true);

    mWeaponTextureLoaded = true;
}

void PlayerSoldier::setupPlayer3Flameshot()
{
    if (!mWeaponTexture.loadFromFile("Sprites/p3_flame.png")) return;

    IntRect wIdle[4] = { IntRect(45,607,88,82), IntRect(133,607,88,82), IntRect(221,613,90,84), IntRect(313,615,90,82) };
    IntRect wWalk[8] = { IntRect(235,717,90,81), IntRect(335,717,90,82), IntRect(429,717,90,84), IntRect(523,717,90,90), IntRect(619,717,90,88), IntRect(715,717,90,87), IntRect(1002,717,93,82), IntRect(1099,717,92,87) };
    IntRect wJump[6] = { IntRect(37,813,92,95), IntRect(135,813,92,93), IntRect(233,813,90,91), IntRect(333,813,90,89), IntRect(427,813,90,86), IntRect(521,813,89,80) };
    IntRect wShootI[4] = { IntRect(40,919,83,77), IntRect(138,921,85,75), IntRect(236,921,87,75), IntRect(340,921,91,75) };

    mWeaponAnims[W_IDLE].setup("w_idle", wIdle, 4, 6.f, true);
    mWeaponAnims[W_WALK].setup("w_walk", wWalk, 6, 20.f, true);
    mWeaponAnims[W_JUMP].setup("w_jump", wJump, 4, 6.f, false);
    mWeaponAnims[W_SHOOT_IDLE].setup("w_s_idle", wShootI, 6, 20.f, true);

    mWeaponTextureLoaded = true;
}

void PlayerSoldier::setupPlayer3Laser()
{
    if (!mWeaponTexture.loadFromFile("Sprites/p3_laser.png")) return;

    IntRect wIdle[4] = { IntRect(37,1025,88,82), IntRect(125,1025,88,82), IntRect(213,1031,90,84), IntRect(305,1033,92,82) };
    IntRect wWalk[8] = { IntRect(327,1135,90,82), IntRect(421,1135,90,84), IntRect(515,1135,90,90), IntRect(611,1135,90,88), IntRect(707,1135,90,87), IntRect(896,1135,95,86), IntRect(994,1135,93,82), IntRect(1091,1135,96,87) };
    IntRect wJump[6] = { IntRect(29,1231,92,95), IntRect(127,1231,92,93), IntRect(225,1231,90,91), IntRect(325,1231,90,89), IntRect(419,1231,90,86), IntRect(513,1231,90,80) };
    IntRect wShootI[4] = { IntRect(32,1337,83,77), IntRect(130,1339,87,75), IntRect(228,1339,91,75), IntRect(332,1339,93,75) };

    mWeaponAnims[W_IDLE].setup("w_idle", wIdle, 4, 6.f, true);
    mWeaponAnims[W_WALK].setup("w_walk", wWalk, 6, 20.f, true);
    mWeaponAnims[W_JUMP].setup("w_jump", wJump, 4, 6.f, false);
    mWeaponAnims[W_SHOOT_IDLE].setup("w_s_idle", wShootI, 4, 12.f, false);

    mWeaponTextureLoaded = true;
}

void PlayerSoldier::setupPlayer3RL()
{
    if (!mWeaponTexture.loadFromFile("Sprites/p3_rocket.png")) return;

    IntRect wIdle[4] = { IntRect(27,1433,88,82), IntRect(115,1433,88,82), IntRect(203,1439,90,84), IntRect(295,1441,92,82) };
    IntRect wWalk[8] = { IntRect(317,1543,90,82), IntRect(411,1543,90,84), IntRect(505,1543,90,90), IntRect(601,1543,90,88), IntRect(697,1543,90,87), IntRect(791,1543,92,88), IntRect(984,1543,93,82), IntRect(1081,1543,94,87) };
    IntRect wJump[6] = { IntRect(19,1639,92,95), IntRect(117,1639,92,93), IntRect(215,1639,90,91), IntRect(315,1639,90,89), IntRect(409,1639,90,86), IntRect(503,1639,92,80) };
    IntRect wShootI[4] = { IntRect(22,1745,85,77), IntRect(120,1747,87,75), IntRect(218,1747,89,75), IntRect(322,1747,93,75) };

    mWeaponAnims[W_IDLE].setup("w_idle", wIdle, 4, 6.f, true);
    mWeaponAnims[W_WALK].setup("w_walk", wWalk, 6, 20.f, true);
    mWeaponAnims[W_JUMP].setup("w_jump", wJump, 4, 6.f, false);
    mWeaponAnims[W_SHOOT_IDLE].setup("w_s_idle", wShootI, 4, 4.f, false);

    mWeaponTextureLoaded = true;
}

// ----------------------------------------------------------------
//  PLAYER 4  (Fio)
// ----------------------------------------------------------------

void PlayerSoldier::setupPlayer4Pistol()
{
    if (!mWeaponTexture.loadFromFile("Sprites/p4_pistol.png")) return;

    IntRect wShootI[9] = { IntRect(43,18,108,75), IntRect(155,18,108,75), IntRect(265,16,112,77), IntRect(377,14,66,79), IntRect(445,12,66,81), IntRect(511,18,76,75), IntRect(587,18,76,75), IntRect(665,18,74,77), IntRect(743,18,70,77) };

    IntRect wMelee[6] = { IntRect(51,137,52,82), IntRect(107,137,56,80), IntRect(167,139,70,76), IntRect(241,133,102,88), IntRect(615,139,54,76), IntRect(673,139,62,76) };

    mWeaponAnims[W_SHOOT_IDLE].setup("w_s_idle", wShootI, 4, 12.f, false);
    mWeaponAnims[W_MELEE].setup("w_melee", wMelee, 6, 15.f, false);
    mWeaponTextureLoaded = true;
}

void PlayerSoldier::setupPlayer4HMG()
{
    if (!mWeaponTexture.loadFromFile("Sprites/p4_hmg.png")) return;

    IntRect wIdle[4] = { IntRect(47,245,70,94), IntRect(119,243,70,96), IntRect(191,241,70,98), IntRect(263,239,70,100) };
    IntRect wWalk[6] = { IntRect(28,344,76,99), IntRect(104,334,76,99), IntRect(181,348,71,94), IntRect(252,348,71,94), IntRect(327,350,71,94), IntRect(403,350,73,94) };
    IntRect wJump[6] = { IntRect(41,464,73,107), IntRect(114,464,73,107), IntRect(187,464,73,107), IntRect(260,464,73,107), IntRect(336,470,72,92), IntRect(412,470,74,84) };
    IntRect wShootI[4] = { IntRect(53,594,130,79), IntRect(183,596,134,77), IntRect(319,596,130,77), IntRect(455,594,130,79) };

    IntRect wMelee[6] = { IntRect(51,137,52,82), IntRect(107,137,56,80), IntRect(167,139,70,76), IntRect(241,133,102,88), IntRect(615,139,54,76), IntRect(673,139,62,76) };

    //IntRect wMelee[4] = { IntRect(0,0,1,1), IntRect(0,0,1,1), IntRect(0,0,1,1), IntRect(0,0,1,1) };

    mWeaponAnims[W_IDLE].setup("w_idle", wIdle, 4, 6.f, true);
    mWeaponAnims[W_WALK].setup("w_walk", wWalk, 6, 20.f, true);
    mWeaponAnims[W_JUMP].setup("w_jump", wJump, 4, 6.f, false);
    mWeaponAnims[W_SHOOT_IDLE].setup("w_s_idle", wShootI, 6, 24.f, true);

    mWeaponAnims[W_MELEE].setup("w_melee", wMelee, 6, 15.f, false);
    mWeaponTextureLoaded = true;
}

void PlayerSoldier::setupPlayer4Flameshot()
{
    if (!mWeaponTexture.loadFromFile("Sprites/p4_flame.png")) return;

    IntRect wIdle[4] = { IntRect(31,735,84,80), IntRect(117,735,84,80), IntRect(203,735,84,80), IntRect(289,735,84,80) };
    IntRect wWalk[6] = { IntRect(22,842,87,83), IntRect(109,842,87,83), IntRect(197,842,87,83), IntRect(289,842,87,83), IntRect(373,842,85,84), IntRect(986,842,88,84) };
    IntRect wJump[6] = { IntRect(19,926,87,91), IntRect(105,926,87,91), IntRect(192,926,87,91), IntRect(279,926,87,91), IntRect(366,926,86,82), IntRect(456,926,81,74) };
    IntRect wShootI[6] = { IntRect(24,1028,82,74), IntRect(110,1030,82,72), IntRect(196,1030,82,72), IntRect(282,1028,84,74) };

    IntRect wMelee[6] = { IntRect(51,137,52,82), IntRect(107,137,56,80), IntRect(167,139,70,76), IntRect(241,133,102,88), IntRect(615,139,54,76), IntRect(673,139,62,76) };

    //IntRect wMelee[4] = { IntRect(0,0,1,1), IntRect(0,0,1,1), IntRect(0,0,1,1), IntRect(0,0,1,1) };

    mWeaponAnims[W_IDLE].setup("w_idle", wIdle, 4, 6.f, true);
    mWeaponAnims[W_WALK].setup("w_walk", wWalk, 6, 20.f, true);
    mWeaponAnims[W_JUMP].setup("w_jump", wJump, 4, 6.f, false);
    mWeaponAnims[W_SHOOT_IDLE].setup("w_s_idle", wShootI, 6, 20.f, true);

    mWeaponAnims[W_MELEE].setup("w_melee", wMelee, 6, 15.f, false);
    mWeaponTextureLoaded = true;
}

void PlayerSoldier::setupPlayer4Laser()
{
    if (!mWeaponTexture.loadFromFile("Sprites/p4_laser.png")) return;

    IntRect wIdle[4] = { IntRect(27,1129,84,80), IntRect(113,1129,84,80), IntRect(199,1129,84,80), IntRect(285,1129,84,80) };
    IntRect wWalk[6] = { IntRect(18,1236,87,83), IntRect(105,1236,87,83), IntRect(192,1236,87,83), IntRect(279,1236,87,83), IntRect(369,1236,85,84), IntRect(982,1236,88,84) };
    IntRect wJump[6] = { IntRect(15,1320,86,91), IntRect(101,1320,86,91), IntRect(187,1320,86,91), IntRect(274,1320,86,91), IntRect(362,1320,86,82), IntRect(452,1320,81,74) };
    IntRect wShootI[4] = { IntRect(20,1422,82,74), IntRect(106,1424,82,72), IntRect(192,1424,82,72), IntRect(278,1422,84,74) };

    IntRect wMelee[6] = { IntRect(51,137,52,82), IntRect(107,137,56,80), IntRect(167,139,70,76), IntRect(241,133,102,88), IntRect(615,139,54,76), IntRect(673,139,62,76) };

    //IntRect wMelee[4] = { IntRect(0,0,1,1), IntRect(0,0,1,1), IntRect(0,0,1,1), IntRect(0,0,1,1) };

    mWeaponAnims[W_IDLE].setup("w_idle", wIdle, 4, 6.f, true);
    mWeaponAnims[W_WALK].setup("w_walk", wWalk, 6, 20.f, true);
    mWeaponAnims[W_JUMP].setup("w_jump", wJump, 4, 6.f, false);
    mWeaponAnims[W_SHOOT_IDLE].setup("w_s_idle", wShootI, 4, 12.f, false);

    mWeaponAnims[W_MELEE].setup("w_melee", wMelee, 6, 15.f, false);
    mWeaponTextureLoaded = true;
}

void PlayerSoldier::setupPlayer4RL()
{
    if (!mWeaponTexture.loadFromFile("Sprites/p4_rocket.png")) return;

    IntRect wIdle[4] = { IntRect(17,1513,84,80), IntRect(103,1513,84,80), IntRect(189,1513,84,80), IntRect(275,1513,84,80) };
    IntRect wWalk[6] = { IntRect(8,1620,87,83), IntRect(95,1620,87,83), IntRect(182,1620,87,83), IntRect(269,1620,87,83), IntRect(359,1620,85,84), IntRect(972,1620,88,84) };
    IntRect wJump[6] = { IntRect(5,1704,86,91), IntRect(91,1704,86,91), IntRect(177,1704,86,91), IntRect(263,1704,86,91), IntRect(352,1704,86,82), IntRect(442,1704,84,74) };
    IntRect wShootI[4] = { IntRect(10,1806,82,74), IntRect(96,1808,82,72), IntRect(182,1808,82,72), IntRect(268,1806,84,74) };

    IntRect wMelee[6] = { IntRect(51,137,52,82), IntRect(107,137,56,80), IntRect(167,139,70,76), IntRect(241,133,102,88), IntRect(615,139,54,76), IntRect(673,139,62,76) };

    //IntRect wMelee[4] = { IntRect(0,0,1,1), IntRect(0,0,1,1), IntRect(0,0,1,1), IntRect(0,0,1,1) };

    mWeaponAnims[W_IDLE].setup("w_idle", wIdle, 4, 6.f, true);
    mWeaponAnims[W_WALK].setup("w_walk", wWalk, 6, 20.f, true);
    mWeaponAnims[W_JUMP].setup("w_jump", wJump, 4, 6.f, false);
    mWeaponAnims[W_SHOOT_IDLE].setup("w_s_idle", wShootI, 4, 4.f, false);

    mWeaponAnims[W_MELEE].setup("w_melee", wMelee, 6, 15.f, false);
    mWeaponTextureLoaded = true;
}

#include "Soldier.h"
#include <SFML/Window/Keyboard.hpp>
#include <cstring>

static bool isTile(char** lvl, int x, int y, int mapWidth, int mapHeight)
{
    if (x < 0 || x >= mapWidth || y < 0 || y >= mapHeight) return false;
    return lvl[y][x] == 'g';
}

// ================================================================
//  SOLDIER BASE
// ================================================================
Soldier::Soldier(float spd, int hp)
    : speed(spd), health(hp),
    mcurrentAnim(IDLE),
    mCurrentWeaponAnim(W_IDLE),
    mWeaponTextureLoaded(false)
{
}

void Soldier::setPosition(float x, float y) { msprite.setPosition(x, y); }
Sprite& Soldier::getSprite() { return msprite; }
Sprite& Soldier::getWeaponSprite() { return mWeaponSprite; }

void Soldier::equipWeapon(Weapon* w) { delete m_weapon; m_weapon = w; }

void Soldier::setAnimation(AnimType type)
{
    if (mcurrentAnim == type && !manimations[type].isFinished()) return;
    mcurrentAnim = type;
    manimations[type].reset();
    manimations[type].play();
}

void Soldier::setWeaponAnimation(WeaponAnimType type)
{
    if (mCurrentWeaponAnim == type && !mWeaponAnims[type].isFinished()) return;
    mCurrentWeaponAnim = type;
    mWeaponAnims[type].reset();
    mWeaponAnims[type].play();
}

void Soldier::syncWeaponSprite()
{
    mWeaponSprite.setPosition(msprite.getPosition());
    mWeaponSprite.setScale(facingRight ? mWeaponScale : -mWeaponScale, mWeaponScale);
}

/*void Soldier::syncWeaponSprite()
{
    mWeaponSprite.setPosition(msprite.getPosition());
    mWeaponSprite.setScale(facingRight ? baseScale : -baseScale, baseScale);
}*/
/*
void Soldier::update(float dt, char** lvl, int cell_size, int mapWidth, int mapHeight)
{
    if (!onGround) { velocityY += gravity; if (velocityY > 20.f) velocityY = 20.f; }
    else { velocityY = 0.f; }

    float bx = msprite.getPosition().x + collisionOffsetX;
    float by = msprite.getPosition().y + collisionOffsetY;
    float bw = collisionWidth, bh = collisionHeight;

    // ---- MOVE X ----
    msprite.move(velocityX, 0);
    bx = msprite.getPosition().x + collisionOffsetX;
    int L = (int)(bx / cell_size), R = (int)((bx + bw - 1) / cell_size);
    int T = (int)(by / cell_size), B = (int)((by + bh - 1) / cell_size);
    if (L < 0)L = 0; else if (L >= mapWidth) L = mapWidth - 1;
    if (R < 0)R = 0; else if (R >= mapWidth) R = mapWidth - 1;
    if (T < 0)T = 0; else if (T >= mapHeight)T = mapHeight - 1;
    if (B < 0)B = 0; else if (B >= mapHeight)B = mapHeight - 1;
    if (velocityX > 0) { for (int i = T; i <= B; i++) if (isTile(lvl, R, i, mapWidth, mapHeight)) { msprite.setPosition(R * cell_size - bw - collisionOffsetX, msprite.getPosition().y); velocityX = 0; bx = msprite.getPosition().x + collisionOffsetX; break; } }
    else if (velocityX < 0) { for (int i = T; i <= B; i++) if (isTile(lvl, L, i, mapWidth, mapHeight)) { msprite.setPosition((L + 1) * cell_size - collisionOffsetX, msprite.getPosition().y); velocityX = 0; bx = msprite.getPosition().x + collisionOffsetX; break; } }

    // ---- MOVE Y ----
    msprite.move(0, velocityY);
    by = msprite.getPosition().y + collisionOffsetY;
    L = (int)(bx / cell_size); R = (int)((bx + bw - 1) / cell_size);
    T = (int)(by / cell_size); B = (int)((by + bh - 1) / cell_size);
    if (L < 0)L = 0; else if (L >= mapWidth) L = mapWidth - 1;
    if (R < 0)R = 0; else if (R >= mapWidth) R = mapWidth - 1;
    if (T < 0)T = 0; else if (T >= mapHeight)T = mapHeight - 1;
    if (B < 0)B = 0; else if (B >= mapHeight)B = mapHeight - 1;
    onGround = false;
    if (velocityY >= 0) { for (int j = L; j <= R; j++) if (isTile(lvl, j, B, mapWidth, mapHeight)) { msprite.setPosition(msprite.getPosition().x, B * cell_size - bh - collisionOffsetY); velocityY = 0; onGround = true; isJumping = false; by = msprite.getPosition().y + collisionOffsetY; break; } }
    else { for (int j = L; j <= R; j++) if (isTile(lvl, j, T, mapWidth, mapHeight)) { msprite.setPosition(msprite.getPosition().x, (T + 1) * cell_size - collisionOffsetY); velocityY = 0; isJumping = false; by = msprite.getPosition().y + collisionOffsetY; break; } }

    // ---- CLAMP ----
    bx = msprite.getPosition().x + collisionOffsetX;
    by = msprite.getPosition().y + collisionOffsetY;
    float cx = bx, cy = by;
    if (cx < 0.f) cx = 0.f; else if (cx > mapWidth * cell_size - bw) cx = mapWidth * cell_size - bw;
    if (cy < 0.f) cy = 0.f; else if (cy > mapHeight * cell_size - bh) { cy = mapHeight * cell_size - bh; velocityY = 0; onGround = true; isJumping = false; }
    msprite.setPosition(cx - collisionOffsetX, cy - collisionOffsetY);

    // ---- BODY ANIMATION ----
    bool isRunning = (velocityX != 0.f);
    if (isJumping)      setAnimation(isRunning ? JUMP_RUN : JUMP_IDLE);
    else if (isRunning) setAnimation(WALK);
    else                setAnimation(IDLE);
    manimations[mcurrentAnim].update(dt);
    msprite.setTextureRect(manimations[mcurrentAnim].getCurrentFrame());
    msprite.setScale(facingRight ? baseScale : -baseScale, baseScale);

    // ---- WEAPON OVERLAY ANIMATION ----
    if (mWeaponTextureLoaded)
    {
        mWeaponAnims[mCurrentWeaponAnim].update(dt);
        mWeaponSprite.setTextureRect(mWeaponAnims[mCurrentWeaponAnim].getCurrentFrame());
        syncWeaponSprite();
    }

    if (m_weapon) m_weapon->update(dt);
}

// ================================================================
//  PLAYER SOLDIER
// ================================================================
PlayerSoldier::PlayerSoldier(int index) : Soldier(3.f, 100), mplayerIndex(index)
{
    loadPlayerData();
    setAnimation(IDLE);
    equipWeapon(new Pistol());
}

void PlayerSoldier::update(float dt, char** lvl, int cell_size, int mapWidth, int mapHeight)
{
    handleInput(dt);
    Soldier::update(dt, lvl, cell_size, mapWidth, mapHeight);
}

// ================================================================
//  LOAD PROJECTILE TEXTURES
//  Called once in loadPlayerData() so RocketLauncher / HandGrenade
//  always have a valid Texture& when constructed via equipWeapon().
// ================================================================
void PlayerSoldier::loadProjectileTextures()
{
    // These sheets are the same for all players — the projectile itself
    // looks the same regardless of who fired it.
    mRocketTex.loadFromFile("Sprites/rocket.png");
    mGrenadeTex.loadFromFile("Sprites/grenade.png");
}

// ================================================================
//  INPUT
// ================================================================
void PlayerSoldier::handleInput(float dt)
{
    velocityX = 0;
    mIsShooting = false;

    bool fireKey = false;
    bool cycleKey = false;

    if (mplayerIndex == 0)
    {
        if (Keyboard::isKeyPressed(Keyboard::Right)) { velocityX = speed; facingRight = true; }
        if (Keyboard::isKeyPressed(Keyboard::Left)) { velocityX = -speed; facingRight = false; }
        if (Keyboard::isKeyPressed(Keyboard::Up) && onGround)
        {
            velocityY = jumpStrength; onGround = false; isJumping = true;
        }

        fireKey = Keyboard::isKeyPressed(Keyboard::Z);   // Z = shoot / attack
        cycleKey = Keyboard::isKeyPressed(Keyboard::X);   // X = cycle weapon
    }
    else if (mplayerIndex == 1)
    {
        if (Keyboard::isKeyPressed(Keyboard::D)) { velocityX = speed; facingRight = true; }
        if (Keyboard::isKeyPressed(Keyboard::A)) { velocityX = -speed; facingRight = false; }
        if (Keyboard::isKeyPressed(Keyboard::W) && onGround)
        {
            velocityY = jumpStrength; onGround = false; isJumping = true;
        }

        if (onGround) coyoteTime = coyoteMax;
        else          coyoteTime -= dt;

        fireKey = Keyboard::isKeyPressed(Keyboard::LControl); // LCtrl = shoot / attack
        cycleKey = Keyboard::isKeyPressed(Keyboard::Q);        // Q     = cycle weapon
    }
    // Players 2 & 3 input not yet assigned

    // ---- CYCLE WEAPON (one step per press, not held) ----
    if (cycleKey && !mPrevCycleKey)
    {
        mWeaponSlot = (mWeaponSlot + 1) % 5;  // 0=Pistol 1=HMG 2=Flame 3=Laser 4=Rocket
        switch (mWeaponSlot)
        {
        case 0: equipWeapon(new Pistol());                   break;
        case 1: equipWeapon(new HeavyMachineGun());          break;
        case 2: equipWeapon(new FlameShot());                break;
        case 3: equipWeapon(new LaserGun());                 break;
        case 4: equipWeapon(new RocketLauncher(mRocketTex)); break;
        }
    }
    mPrevCycleKey = cycleKey;

    // ---- FIRE / ATTACK ----
    if (fireKey && m_weapon)
    {
        mIsShooting = true;
        float ox = msprite.getPosition().x + (facingRight ? collisionWidth : 0.f);
        float oy = msprite.getPosition().y + collisionHeight * 0.4f;
        m_weapon->fire(ox, oy, facingRight, 0.f);
    }

    // ---- SELECT WEAPON ANIMATION ----
    if (mWeaponTextureLoaded)
    {
        bool airborne = isJumping || !onGround;
        bool moving = (velocityX != 0.f);

        if (mIsShooting)
        {
            if (airborne) setWeaponAnimation(W_SHOOT_JUMP);
            else if (moving)   setWeaponAnimation(W_SHOOT_WALK);
            else               setWeaponAnimation(W_SHOOT_IDLE);
        }
        else
        {
            if (airborne) setWeaponAnimation(W_JUMP);
            else if (moving)   setWeaponAnimation(W_WALK);
            else               setWeaponAnimation(W_IDLE);
        }
    }
}

// ================================================================
//  LOAD PLAYER DATA  (body texture + base animations)
// ================================================================
void PlayerSoldier::loadPlayerData()
{
    if (mplayerIndex == 0)
    {
        mtexture.loadFromFile("Sprites/Group 10.png");
        setupPlayer4Animations();
        baseScale = 2.5f;
        mWeaponScale = 1.f;
    }
    else if (mplayerIndex == 1)
    {
        mtexture.loadFromFile("Sprites/Group 8.png");
        setupPlayer2Animations();
        baseScale = 1.65f;
        mWeaponScale = 1.f;
    }
    else if (mplayerIndex == 2)
    {
        mtexture.loadFromFile("Sprites/Group 5.png");
        setupPlayer1Animations();
        baseScale = 1.f;
        mWeaponScale = 1.f;
    }
    else if (mplayerIndex == 3)
    {
        mtexture.loadFromFile("Sprites/Group 9.png");
        setupPlayer3Animations();
        baseScale = 1.25f;
        mWeaponScale = 1.f;
    }
    msprite.setTexture(mtexture);
    loadProjectileTextures();   // always load so weapon swap never has a dangling ref
}

// ================================================================
//  EQUIP WEAPON  — swap weapon + reload matching overlay sheet
// ================================================================
void PlayerSoldier::equipWeapon(Weapon* w)
{
    Soldier::equipWeapon(w);
    if (!m_weapon) return;

    const char* name = m_weapon->getName();
    mWeaponTextureLoaded = false;

    if (mplayerIndex == 0)
    {
        if (strcmp(name, "pistol") == 0) setupPlayer1Pistol();
        else if (strcmp(name, "hmg") == 0) setupPlayer1HMG();
        else if (strcmp(name, "flameshot") == 0) setupPlayer1Flameshot();
        else if (strcmp(name, "laserbeam") == 0) setupPlayer1Laser();
        else if (strcmp(name, "rocket") == 0) setupPlayer1RL();
    }
    else if (mplayerIndex == 1)
    {
        if (strcmp(name, "pistol") == 0) setupPlayer2Pistol();
        else if (strcmp(name, "hmg") == 0) setupPlayer2HMG();
        else if (strcmp(name, "flameshot") == 0) setupPlayer2Flameshot();
        else if (strcmp(name, "laserbeam") == 0) setupPlayer2Laser();
        else if (strcmp(name, "rocket") == 0) setupPlayer2RL();
    }
    else if (mplayerIndex == 2)
    {
        if (strcmp(name, "pistol") == 0) setupPlayer3Pistol();
        else if (strcmp(name, "hmg") == 0) setupPlayer3HMG();
        else if (strcmp(name, "flameshot") == 0) setupPlayer3Flameshot();
        else if (strcmp(name, "laserbeam") == 0) setupPlayer3Laser();
        else if (strcmp(name, "rocket") == 0) setupPlayer3RL();
    }
    else if (mplayerIndex == 3)
    {
        if (strcmp(name, "pistol") == 0) setupPlayer4Pistol();
        else if (strcmp(name, "hmg") == 0) setupPlayer4HMG();
        else if (strcmp(name, "flameshot") == 0) setupPlayer4Flameshot();
        else if (strcmp(name, "laserbeam") == 0) setupPlayer4Laser();
        else if (strcmp(name, "rocket") == 0) setupPlayer4RL();
    }

    /*if (mWeaponTextureLoaded)
    {
        mWeaponSprite.setTexture(mWeaponTexture);
        setWeaponAnimation(W_IDLE);
    }*//*
    if (mWeaponTextureLoaded)
    {
        mWeaponSprite.setTexture(mWeaponTexture, true);  // <-- pass 'true' to reset the rect
        setWeaponAnimation(W_IDLE);
    }
}

// ================================================================
//  BODY ANIMATIONS  (unchanged from original)
// ================================================================
void PlayerSoldier::setupPlayer1Animations()
{
    IntRect idle[4] = { IntRect(90,4,83,101), IntRect(174,4,84,101), IntRect(264,4,81,101), IntRect(346,4,81,101) };
    IntRect walk[12] = { IntRect(88,126,79,90), IntRect(168,126,78,92), IntRect(246,126,78,92), IntRect(325,128,77,99), IntRect(402,128,77,99), IntRect(485,128,77,99), IntRect(567,130,77,97), IntRect(647,130,77,99), IntRect(727,130,79,97), IntRect(811,130,78,97), IntRect(889,130,78,97), IntRect(971,130,74,97) };
    IntRect jumpIdle[6] = { IntRect(88,238,66,106), IntRect(157,242,70,102), IntRect(232,244,71,98), IntRect(308,242,69,98), IntRect(384,242,69,92), IntRect(459,244,67,84) };
    IntRect jumpRun[6] = { IntRect(86,362,73,97), IntRect(159,362,73,97), IntRect(239,362,79,88), IntRect(318,360,73,90), IntRect(391,360,73,90), IntRect(464,360,73,90) };
    IntRect swimIdle[6] = { IntRect(22,803,136,77), IntRect(165,801,139,75), IntRect(312,812,137,67), IntRect(457,812,140,64), IntRect(598,810,141,64), IntRect(758,808,140,64) };
    IntRect swim[10] = { IntRect(34,916,132,74), IntRect(176,911,130,67), IntRect(314,916,133,61), IntRect(456,906,139,64), IntRect(600,908,138,61), IntRect(756,905,140,60), IntRect(898,900,138,66), IntRect(1054,895,136,66), IntRect(1196,889,130,77), IntRect(1337,890,131,76) };
    IntRect die[21] = { IntRect(78,687,72,93), IntRect(155,694,79,86), IntRect(239,694,83,86), IntRect(328,692,68,88), IntRect(399,685,69,95), IntRect(471,682,74,98), IntRect(548,680,71,100), IntRect(622,680,68,100), IntRect(696,680,61,100), IntRect(761,687,66,93), IntRect(832,690,67,90), IntRect(902,694,71,86), IntRect(976,706,78,74), IntRect(1057,704,74,76), IntRect(1136,706,69,74), IntRect(1208,709,66,71), IntRect(1277,713,69,67), IntRect(1351,718,69,62), IntRect(1426,728,71,52), IntRect(1500,742,88,38), IntRect(1588,740,84,40) };
    manimations[IDLE].setup("idle", idle, 4, 6.f, true);
    manimations[WALK].setup("walk", walk, 12, 20.f, true);
    manimations[SWIM_IDLE].setup("swim_idle", swimIdle, 6, 8.f, false);
    manimations[SWIM].setup("swim", swim, 10, 8.f, false);
    manimations[JUMP_IDLE].setup("jump_idle", jumpIdle, 6, 6.f, false);
    manimations[JUMP_RUN].setup("jump_run", jumpRun, 6, 6.f, false);
    manimations[DIE].setup("die", die, 21, 5.f, false);
}

void PlayerSoldier::setupPlayer2Animations()
{
    IntRect idle[4] = { IntRect(8,10,48,60), IntRect(61,10,48,61), IntRect(115,10,49,61), IntRect(170,10,51,61) };
    IntRect walk[6] = { IntRect(2,82,48,67), IntRect(58,82,49,67), IntRect(115,82,48,67), IntRect(170,82,50,67), IntRect(223,82,52,67), IntRect(281,82,50,67) };
    IntRect jumpIdle[6] = { IntRect(3,169,47,70), IntRect(53,170,48,69), IntRect(107,172,48,67), IntRect(158,173,48,66), IntRect(213,173,47,63), IntRect(264,173,47,62) };
    IntRect jumpRun[6] = { IntRect(329,170,48,66), IntRect(382,170,48,63), IntRect(436,170,55,60), IntRect(496,169,49,58), IntRect(549,169,49,58), IntRect(607,169,43,60) };
    IntRect die[19] = { IntRect(4,253,43,60), IntRect(58,256,49,55), IntRect(113,257,47,56), IntRect(169,256,46,57), IntRect(226,254,48,56), IntRect(283,260,42,50), IntRect(332,260,44,50), IntRect(389,263,47,48), IntRect(446,268,53,43), IntRect(510,266,56,42), IntRect(572,271,50,34), IntRect(631,265,48,40), IntRect(687,265,40,42), IntRect(736,260,37,48), IntRect(778,265,39,45), IntRect(826,268,43,45), IntRect(874,278,48,35), IntRect(927,284,53,29), IntRect(985,289,57,24) };
    manimations[IDLE].setup("idle", idle, 4, 6.f, true);
    manimations[WALK].setup("walk", walk, 6, 15.f, true);
    manimations[JUMP_IDLE].setup("jump_idle", jumpIdle, 6, 6.f, false);
    manimations[JUMP_RUN].setup("jump_run", jumpRun, 6, 6.f, false);
    manimations[DIE].setup("die", die, 19, 5.f, false);
}

void PlayerSoldier::setupPlayer3Animations()
{
    IntRect idle[4] = { IntRect(0,1,63,75), IntRect(68,1,61,75), IntRect(136,1,63,75), IntRect(206,1,66,75) };
    IntRect walk[6] = { IntRect(0,82,67,85), IntRect(67,82,67,85), IntRect(135,82,63,85), IntRect(195,82,63,67), IntRect(262,82,64,84), IntRect(326,82,67,83) };
    IntRect jumpIdle[6] = { IntRect(7,170,60,99), IntRect(75,172,60,98), IntRect(145,175,60,91), IntRect(213,175,60,87), IntRect(281,175,60,77), IntRect(349,175,60,78) };
    IntRect jumpRun[6] = { IntRect(1,272,61,94), IntRect(71,272,63,87), IntRect(143,272,73,78), IntRect(237,272,66,73), IntRect(310,272,68,75), IntRect(393,272,57,81) };
    IntRect die[19] = { IntRect(4,253,43,60), IntRect(58,256,49,55), IntRect(113,257,47,56), IntRect(169,256,46,57), IntRect(226,254,48,56), IntRect(283,260,42,50), IntRect(332,260,44,50), IntRect(389,263,47,48), IntRect(446,268,53,43), IntRect(510,266,56,42), IntRect(572,271,50,34), IntRect(631,265,48,40), IntRect(687,265,40,42), IntRect(736,260,37,48), IntRect(778,265,39,45), IntRect(826,268,43,45), IntRect(874,278,48,35), IntRect(927,284,53,29), IntRect(985,289,57,24) };
    manimations[IDLE].setup("idle", idle, 4, 6.f, true);
    manimations[WALK].setup("walk", walk, 6, 15.f, true);
    manimations[JUMP_IDLE].setup("jump_idle", jumpIdle, 6, 6.f, false);
    manimations[JUMP_RUN].setup("jump_run", jumpRun, 6, 6.f, false);
    manimations[DIE].setup("die", die, 19, 5.f, false);
}

void PlayerSoldier::setupPlayer4Animations()
{
    IntRect idle[4] = { IntRect(4,0,31,37), IntRect(37,0,31,38), IntRect(70,0,31,38), IntRect(103,0,31,38) };
    IntRect walk[14] = { IntRect(0,54,32,39), IntRect(32,54,32,39), IntRect(65,56,31,35), IntRect(96,55,32,37), IntRect(132,54,28,38), IntRect(164,54,29,39), IntRect(196,55,30,38), IntRect(227,56,33,39), IntRect(260,56,33,39), IntRect(293,56,33,39), IntRect(328,55,29,38), IntRect(359,55,30,38), IntRect(164,54,29,39), IntRect(196,55,30,38) };
    IntRect jumpIdle[6] = { IntRect(6,105,29,45), IntRect(40,107,30,43), IntRect(75,109,30,40), IntRect(110,109,30,39), IntRect(145,109,29,37), IntRect(178,109,29,35) };
    IntRect jumpRun[6] = { IntRect(9,163,29,43), IntRect(38,163,29,43), IntRect(71,163,34,32), IntRect(106,163,30,35), IntRect(138,163,30,36), IntRect(168,163,26,37) };
    IntRect die[19] = { IntRect(4,253,43,60), IntRect(58,256,49,55), IntRect(113,257,47,56), IntRect(169,256,46,57), IntRect(226,254,48,56), IntRect(283,260,42,50), IntRect(332,260,44,50), IntRect(389,263,47,48), IntRect(446,268,53,43), IntRect(510,266,56,42), IntRect(572,271,50,34), IntRect(631,265,48,40), IntRect(687,265,40,42), IntRect(736,260,37,48), IntRect(778,265,39,45), IntRect(826,268,43,45), IntRect(874,278,48,35), IntRect(927,284,53,29), IntRect(985,289,57,24) };
    manimations[IDLE].setup("idle", idle, 4, 6.f, true);
    manimations[WALK].setup("walk", walk, 14, 20.f, true);
    manimations[JUMP_IDLE].setup("jump_idle", jumpIdle, 6, 6.f, false);
    manimations[JUMP_RUN].setup("jump_run", jumpRun, 6, 6.f, false);
    manimations[DIE].setup("die", die, 19, 5.f, false);
}

// ================================================================
//  WEAPON OVERLAY SETUPS  (all IntRect values from document 8 — unchanged)
// ================================================================

void PlayerSoldier::setupPlayer1Pistol()
{
    if (!mWeaponTexture.loadFromFile("Sprites/Group 24.png")) return;
    IntRect wShootI[10] = { IntRect(43,13,108,78), IntRect(155,13,108,78), IntRect(267,13,110,78), IntRect(381,13,80,82), IntRect(465,9,80,86), IntRect(547,9,82,84), IntRect(631,13,82,80), IntRect(715,13,76,80), IntRect(795,13,72,80), IntRect(869,11,72,82) };
    IntRect wMelee[6] = { IntRect(31,135,54,84), IntRect(89,135,52,84), IntRect(145,135,58,86), IntRect(207,137,88,82), IntRect(299,128,102,89), IntRect(403,128,104,89) };
    mWeaponAnims[W_SHOOT_IDLE].setup("w_s_idle", wShootI, 4, 12.f, false);
    mWeaponAnims[W_MELEE].setup("w_melee", wMelee, 6, 15.f, false);
    mWeaponTextureLoaded = true;
}
void PlayerSoldier::setupPlayer1HMG()
{
    if (!mWeaponTexture.loadFromFile("Sprites/Group 24.png")) return;
    IntRect wIdle[4] = { IntRect(45,247,78,100), IntRect(127,249,80,98), IntRect(211,251,82,96), IntRect(297,253,84,94) };
    IntRect wWalk[6] = { IntRect(32,370,86,104), IntRect(121,368,83,104), IntRect(208,368,80,105), IntRect(292,368,82,107), IntRect(374,372,88,102), IntRect(466,374,88,99) };
    IntRect wJump[5] = { IntRect(28,504,84,112), IntRect(202,502,80,112), IntRect(286,502,82,110), IntRect(372,506,84,100), IntRect(460,508,88,95) };
    IntRect wShootI[4] = { IntRect(19,646,135,79), IntRect(157,646,137,81), IntRect(299,646,133,81), IntRect(437,646,137,83) };
    IntRect wMelee[6] = { IntRect(31,135,54,84), IntRect(89,135,52,84), IntRect(145,135,58,86), IntRect(207,137,88,82), IntRect(299,128,102,89), IntRect(403,128,104,89) };
    mWeaponAnims[W_IDLE].setup("w_idle", wIdle, 4, 6.f, true);
    mWeaponAnims[W_WALK].setup("w_walk", wWalk, 6, 20.f, true);
    mWeaponAnims[W_JUMP].setup("w_jump", wJump, 4, 6.f, false);
    mWeaponAnims[W_SHOOT_IDLE].setup("w_s_idle", wShootI, 6, 24.f, true);
    mWeaponAnims[W_MELEE].setup("w_melee", wMelee, 6, 15.f, false);
    mWeaponTextureLoaded = true;
}
void PlayerSoldier::setupPlayer1Flameshot()
{
    if (!mWeaponTexture.loadFromFile("Sprites/Group 24.png")) return;
    IntRect wIdle[4] = { IntRect(15,767,83,84), IntRect(99,767,83,86), IntRect(185,767,83,86), IntRect(271,767,85,84) };
    IntRect wWalk[6] = { IntRect(19,877,90,91), IntRect(113,875,85,91), IntRect(201,875,83,92), IntRect(286,887,88,92), IntRect(374,887,88,92), IntRect(467,877,90,90) };
    IntRect wJump[6] = { IntRect(19,985,90,99), IntRect(113,983,87,99), IntRect(201,983,83,101), IntRect(287,985,84,95), IntRect(375,985,88,89), IntRect(467,985,91,84) };
    IntRect wShootI[4] = { IntRect(19,1096,85,79), IntRect(105,1098,83,77), IntRect(189,1096,81,79), IntRect(271,1096,83,79) };
    IntRect wMelee[6] = { IntRect(31,135,54,84), IntRect(89,135,52,84), IntRect(145,135,58,86), IntRect(207,137,88,82), IntRect(299,128,102,89), IntRect(403,128,104,89) };
    mWeaponAnims[W_IDLE].setup("w_idle", wIdle, 4, 6.f, true);
    mWeaponAnims[W_WALK].setup("w_walk", wWalk, 6, 20.f, true);
    mWeaponAnims[W_JUMP].setup("w_jump", wJump, 6, 6.f, false);
    mWeaponAnims[W_SHOOT_IDLE].setup("w_s_idle", wShootI, 4, 20.f, true);
    mWeaponAnims[W_MELEE].setup("w_melee", wMelee, 6, 15.f, false);
    mWeaponTextureLoaded = true;
}
void PlayerSoldier::setupPlayer1Laser()
{
    if (!mWeaponTexture.loadFromFile("Sprites/Group 24.png")) return;
    IntRect wIdle[4] = { IntRect(21,1201,83,86), IntRect(104,1201,83,86), IntRect(191,1201,84,86), IntRect(277,1201,82,84) };
    IntRect wWalk[6] = { IntRect(25,1311,90,91), IntRect(119,1309,84,91), IntRect(207,1309,82,92), IntRect(292,1311,88,92), IntRect(380,1311,88,92), IntRect(473,1311,90,90) };
    IntRect wJump[6] = { IntRect(25,1419,90,99), IntRect(119,1417,84,99), IntRect(207,1417,82,101), IntRect(293,1419,84,95), IntRect(381,1419,88,89), IntRect(473,1419,90,84) };
    IntRect wShootI[4] = { IntRect(25,1530,84,80), IntRect(111,1532,80,78), IntRect(195,1530,78,80), IntRect(277,1530,85,80) };
    IntRect wMelee[6] = { IntRect(31,135,54,84), IntRect(89,135,52,84), IntRect(145,135,58,86), IntRect(207,137,88,82), IntRect(299,128,102,89), IntRect(403,128,104,89) };
    mWeaponAnims[W_IDLE].setup("w_idle", wIdle, 4, 6.f, true);
    mWeaponAnims[W_WALK].setup("w_walk", wWalk, 6, 20.f, true);
    mWeaponAnims[W_JUMP].setup("w_jump", wJump, 4, 6.f, false);
    mWeaponAnims[W_SHOOT_IDLE].setup("w_s_idle", wShootI, 4, 12.f, false);
    mWeaponAnims[W_MELEE].setup("w_melee", wMelee, 6, 15.f, false);
    mWeaponTextureLoaded = true;
}
void PlayerSoldier::setupPlayer1RL()
{
    if (!mWeaponTexture.loadFromFile("Sprites/Group 24.png")) return;
    IntRect wIdle[4] = { IntRect(24,1641,83,84), IntRect(110,1641,83,86), IntRect(197,1641,82,86), IntRect(282,1641,84,84) };
    IntRect wWalk[6] = { IntRect(31,1751,90,91), IntRect(125,1749,84,91), IntRect(212,1749,83,92), IntRect(296,1751,89,92), IntRect(385,1751,89,92), IntRect(479,1751,90,90) };
    IntRect wJump[6] = { IntRect(31,1859,90,99), IntRect(125,1857,84,99), IntRect(213,1857,82,101), IntRect(296,1859,87,95), IntRect(387,1859,88,89), IntRect(479,1859,92,84) };
    IntRect wShootI[4] = { IntRect(30,1970,83,80), IntRect(116,1972,81,78), IntRect(200,1970,79,80), IntRect(283,1970,85,80) };
    IntRect wMelee[6] = { IntRect(31,135,54,84), IntRect(89,135,52,84), IntRect(145,135,58,86), IntRect(207,137,88,82), IntRect(299,128,102,89), IntRect(403,128,104,89) };
    mWeaponAnims[W_IDLE].setup("w_idle", wIdle, 4, 6.f, true);
    mWeaponAnims[W_WALK].setup("w_walk", wWalk, 6, 20.f, true);
    mWeaponAnims[W_JUMP].setup("w_jump", wJump, 4, 6.f, false);
    mWeaponAnims[W_SHOOT_IDLE].setup("w_s_idle", wShootI, 4, 4.f, false);
    mWeaponAnims[W_MELEE].setup("w_melee", wMelee, 6, 15.f, false);
    mWeaponTextureLoaded = true;
}

void PlayerSoldier::setupPlayer2Pistol()
{
    if (!mWeaponTexture.loadFromFile("Sprites/Group 27.png")) return;
    IntRect wShootI[9] = { IntRect(35,9,106,81), IntRect(147,11,108,79), IntRect(259,9,110,81), IntRect(373,7,80,83), IntRect(457,3,80,87), IntRect(541,3,80,85), IntRect(624,7,81,79), IntRect(707,7,74,81), IntRect(861,5,70,81) };
    IntRect wMelee[6] = { IntRect(29,115,54,83), IntRect(95,115,52,85), IntRect(157,115,58,85), IntRect(227,115,88,83), IntRect(325,107,102,91), IntRect(437,107,102,91) };
    mWeaponAnims[W_SHOOT_IDLE].setup("w_s_idle", wShootI, 4, 12.f, false);
    mWeaponAnims[W_MELEE].setup("w_melee", wMelee, 6, 15.f, false);
    mWeaponTextureLoaded = true;
}
void PlayerSoldier::setupPlayer2HMG()
{
    if (!mWeaponTexture.loadFromFile("Sprites/Group 27.png")) return;
    IntRect wIdle[4] = { IntRect(37,225,78,101), IntRect(129,227,80,99), IntRect(223,229,82,97), IntRect(313,231,84,93) };
    IntRect wWalk[9] = { IntRect(43,363,86,104), IntRect(43,363,86,104), IntRect(233,361,88,99), IntRect(337,361,82,106), IntRect(431,367,84,101), IntRect(527,369,88,99), IntRect(627,371,84,99), IntRect(716,367,89,103), IntRect(921,365,90,105) };
    IntRect wJump[6] = { IntRect(37,479,84,113), IntRect(135,477,82,113), IntRect(233,477,80,111), IntRect(329,477,82,109), IntRect(423,483,84,99), IntRect(519,485,88,93) };
    IntRect wShootI[4] = { IntRect(40,628,133,80), IntRect(180,628,135,82), IntRect(322,628,133,82), IntRect(466,630,137,80) };
    IntRect wMelee[6] = { IntRect(29,115,54,83), IntRect(95,115,52,85), IntRect(157,115,58,85), IntRect(227,115,88,83), IntRect(325,107,102,91), IntRect(437,107,102,91) };
    mWeaponAnims[W_IDLE].setup("w_idle", wIdle, 4, 6.f, true);
    mWeaponAnims[W_WALK].setup("w_walk", wWalk, 6, 20.f, true);
    mWeaponAnims[W_JUMP].setup("w_jump", wJump, 4, 6.f, false);
    mWeaponAnims[W_SHOOT_IDLE].setup("w_s_idle", wShootI, 6, 24.f, true);
    mWeaponAnims[W_MELEE].setup("w_melee", wMelee, 6, 15.f, false);
    mWeaponTextureLoaded = true;
}
void PlayerSoldier::setupPlayer2Flameshot()
{
    if (!mWeaponTexture.loadFromFile("Sprites/Group 27.png")) return;
    IntRect wIdle[4] = { IntRect(26,741,83,79), IntRect(114,741,83,79), IntRect(206,741,83,83), IntRect(299,741,82,83) };
    IntRect wWalk[9] = { IntRect(31,845,92,86), IntRect(125,843,90,88), IntRect(217,841,88,83), IntRect(311,843,84,88), IntRect(403,843,88,89), IntRect(501,845,90,87), IntRect(899,845,90,89), IntRect(997,845,92,88), IntRect(1095,845,92,88) };
    IntRect wJump[6] = { IntRect(23,972,89,96), IntRect(121,970,84,96), IntRect(213,968,82,98), IntRect(301,970,83,96), IntRect(393,970,87,88), IntRect(491,972,89,84) };
    IntRect wShootI[4] = { IntRect(28,1071,83,80), IntRect(120,1073,79,78), IntRect(210,1071,77,80), IntRect(298,1071,81,80) };
    IntRect wMelee[6] = { IntRect(29,115,54,83), IntRect(95,115,52,85), IntRect(157,115,58,85), IntRect(227,115,88,83), IntRect(325,107,102,91), IntRect(437,107,102,91) };
    mWeaponAnims[W_IDLE].setup("w_idle", wIdle, 4, 6.f, true);
    mWeaponAnims[W_WALK].setup("w_walk", wWalk, 6, 20.f, true);
    mWeaponAnims[W_JUMP].setup("w_jump", wJump, 4, 6.f, false);
    mWeaponAnims[W_SHOOT_IDLE].setup("w_s_idle", wShootI, 6, 20.f, true);
    mWeaponAnims[W_MELEE].setup("w_melee", wMelee, 6, 15.f, false);
    mWeaponTextureLoaded = true;
}
void PlayerSoldier::setupPlayer2Laser()
{
    if (!mWeaponTexture.loadFromFile("Sprites/Group 27.png")) return;
    IntRect wIdle[4] = { IntRect(8,1179,83,79), IntRect(96,1179,85,79), IntRect(188,1179,83,83), IntRect(281,1179,84,83) };
    IntRect wWalk[9] = { IntRect(13,1283,92,86), IntRect(107,1281,90,88), IntRect(199,1279,88,83), IntRect(293,1281,84,88), IntRect(385,1281,88,89), IntRect(483,1283,90,87), IntRect(579,1283,90,89), IntRect(979,1283,92,88), IntRect(1077,1283,92,88) };
    IntRect wJump[6] = { IntRect(5,1410,89,96), IntRect(103,1408,84,96), IntRect(195,1406,81,98), IntRect(283,1408,83,96), IntRect(375,1408,87,88), IntRect(473,1410,89,84) };
    IntRect wShootI[4] = { IntRect(10,1509,83,80), IntRect(102,1511,79,78), IntRect(192,1509,79,80), IntRect(280,1509,81,80) };
    IntRect wMelee[6] = { IntRect(29,115,54,83), IntRect(95,115,52,85), IntRect(157,115,58,85), IntRect(227,115,88,83), IntRect(325,107,102,91), IntRect(437,107,102,91) };
    mWeaponAnims[W_IDLE].setup("w_idle", wIdle, 4, 6.f, true);
    mWeaponAnims[W_WALK].setup("w_walk", wWalk, 6, 20.f, true);
    mWeaponAnims[W_JUMP].setup("w_jump", wJump, 4, 6.f, false);
    mWeaponAnims[W_SHOOT_IDLE].setup("w_s_idle", wShootI, 4, 12.f, false);
    mWeaponAnims[W_MELEE].setup("w_melee", wMelee, 6, 15.f, false);
    mWeaponTextureLoaded = true;
}
void PlayerSoldier::setupPlayer2RL()
{
    if (!mWeaponTexture.loadFromFile("Sprites/Group 27.png")) return;
    IntRect wIdle[4] = { IntRect(12,1609,81,79), IntRect(100,1609,83,79), IntRect(192,1609,83,83), IntRect(284,1609,83,83) };
    IntRect wWalk[8] = { IntRect(17,1713,92,86), IntRect(111,1711,91,88), IntRect(203,1709,88,83), IntRect(294,1711,87,88), IntRect(389,1711,88,89), IntRect(885,1713,90,89), IntRect(983,1713,92,88), IntRect(1081,1713,92,88) };
    IntRect wJump[6] = { IntRect(9,1840,89,96), IntRect(106,1838,84,96), IntRect(196,1836,84,98), IntRect(282,1838,88,96), IntRect(379,1838,87,88), IntRect(477,1840,89,84) };
    IntRect wShootI[4] = { IntRect(12,1939,83,80), IntRect(104,1941,82,78), IntRect(192,1939,84,80), IntRect(279,1939,88,80) };
    IntRect wMelee[6] = { IntRect(29,115,54,83), IntRect(95,115,52,85), IntRect(157,115,58,85), IntRect(227,115,88,83), IntRect(325,107,102,91), IntRect(437,107,102,91) };
    mWeaponAnims[W_IDLE].setup("w_idle", wIdle, 4, 6.f, true);
    mWeaponAnims[W_WALK].setup("w_walk", wWalk, 6, 20.f, true);
    mWeaponAnims[W_JUMP].setup("w_jump", wJump, 4, 6.f, false);
    mWeaponAnims[W_SHOOT_IDLE].setup("w_s_idle", wShootI, 4, 4.f, false);
    mWeaponAnims[W_MELEE].setup("w_melee", wMelee, 6, 15.f, false);
    mWeaponTextureLoaded = true;
}

void PlayerSoldier::setupPlayer3Pistol()
{
    if (!mWeaponTexture.loadFromFile("Sprites/Group 28.png")) return;
    IntRect wShootI[9] = { IntRect(81,11,108,80), IntRect(195,9,112,82), IntRect(311,11,74,80), IntRect(429,11,70,80), IntRect(505,9,68,82), IntRect(583,13,78,78), IntRect(829,11,78,80) };
    mWeaponAnims[W_SHOOT_IDLE].setup("w_s_idle", wShootI, 4, 12.f, false);
    mWeaponTextureLoaded = true;
}
void PlayerSoldier::setupPlayer3HMG()
{
    if (!mWeaponTexture.loadFromFile("Sprites/Group 28.png")) return;
    IntRect wIdle[4] = { IntRect(69,135,76,96), IntRect(157,133,76,98), IntRect(245,131,74,100), IntRect(329,129,74,102) };
    IntRect wWalk[12] = { IntRect(68,256,77,95), IntRect(154,258,77,92), IntRect(240,262,75,90), IntRect(326,264,75,87), IntRect(410,262,77,89), IntRect(494,262,81,95), IntRect(582,262,77,93), IntRect(670,260,77,96), IntRect(760,256,79,101), IntRect(852,258,81,97), IntRect(940,262,81,91), IntRect(1023,258,82,98) };
    IntRect wJump[6] = { IntRect(58,370,77,108), IntRect(144,372,77,104), IntRect(230,376,75,100), IntRect(316,378,75,96), IntRect(400,376,77,93), IntRect(486,376,79,87) };
    IntRect wShootI[4] = { IntRect(45,607,88,82), IntRect(133,607,88,82), IntRect(221,613,90,84), IntRect(313,615,90,82) };
    mWeaponAnims[W_IDLE].setup("w_idle", wIdle, 4, 6.f, true);
    mWeaponAnims[W_WALK].setup("w_walk", wWalk, 6, 20.f, true);
    mWeaponAnims[W_JUMP].setup("w_jump", wJump, 4, 6.f, false);
    mWeaponAnims[W_SHOOT_IDLE].setup("w_s_idle", wShootI, 6, 24.f, true);
    mWeaponTextureLoaded = true;
}
void PlayerSoldier::setupPlayer3Flameshot()
{
    if (!mWeaponTexture.loadFromFile("Sprites/Group 28.png")) return;
    IntRect wIdle[4] = { IntRect(45,607,88,82), IntRect(133,607,88,82), IntRect(221,613,90,84), IntRect(313,615,90,82) };
    IntRect wWalk[8] = { IntRect(235,717,90,81), IntRect(335,717,90,82), IntRect(429,717,90,84), IntRect(523,717,90,90), IntRect(619,717,90,88), IntRect(715,717,90,87), IntRect(1002,717,93,82), IntRect(1099,717,92,87) };
    IntRect wJump[6] = { IntRect(37,813,92,95), IntRect(135,813,92,93), IntRect(233,813,90,91), IntRect(333,813,90,89), IntRect(427,813,90,86), IntRect(521,813,89,80) };
    IntRect wShootI[4] = { IntRect(40,919,83,77), IntRect(138,921,85,75), IntRect(236,921,87,75), IntRect(340,921,91,75) };
    mWeaponAnims[W_IDLE].setup("w_idle", wIdle, 4, 6.f, true);
    mWeaponAnims[W_WALK].setup("w_walk", wWalk, 6, 20.f, true);
    mWeaponAnims[W_JUMP].setup("w_jump", wJump, 4, 6.f, false);
    mWeaponAnims[W_SHOOT_IDLE].setup("w_s_idle", wShootI, 6, 20.f, true);
    mWeaponTextureLoaded = true;
}
void PlayerSoldier::setupPlayer3Laser()
{
    if (!mWeaponTexture.loadFromFile("Sprites/Group 28.png")) return;
    IntRect wIdle[4] = { IntRect(37,1025,88,82), IntRect(125,1025,88,82), IntRect(213,1031,90,84), IntRect(305,1033,92,82) };
    IntRect wWalk[8] = { IntRect(327,1135,90,82), IntRect(421,1135,90,84), IntRect(515,1135,90,90), IntRect(611,1135,90,88), IntRect(707,1135,90,87), IntRect(896,1135,95,86), IntRect(994,1135,93,82), IntRect(1091,1135,96,87) };
    IntRect wJump[6] = { IntRect(29,1231,92,95), IntRect(127,1231,92,93), IntRect(225,1231,90,91), IntRect(325,1231,90,89), IntRect(419,1231,90,86), IntRect(513,1231,90,80) };
    IntRect wShootI[4] = { IntRect(32,1337,83,77), IntRect(130,1339,87,75), IntRect(228,1339,91,75), IntRect(332,1339,93,75) };
    mWeaponAnims[W_IDLE].setup("w_idle", wIdle, 4, 6.f, true);
    mWeaponAnims[W_WALK].setup("w_walk", wWalk, 6, 20.f, true);
    mWeaponAnims[W_JUMP].setup("w_jump", wJump, 4, 6.f, false);
    mWeaponAnims[W_SHOOT_IDLE].setup("w_s_idle", wShootI, 4, 12.f, false);
    mWeaponTextureLoaded = true;
}
void PlayerSoldier::setupPlayer3RL()
{
    if (!mWeaponTexture.loadFromFile("Sprites/Group 28.png")) return;
    IntRect wIdle[4] = { IntRect(27,1433,88,82), IntRect(115,1433,88,82), IntRect(203,1439,90,84), IntRect(295,1441,92,82) };
    IntRect wWalk[8] = { IntRect(317,1543,90,82), IntRect(411,1543,90,84), IntRect(505,1543,90,90), IntRect(601,1543,90,88), IntRect(697,1543,90,87), IntRect(791,1543,92,88), IntRect(984,1543,93,82), IntRect(1081,1543,94,87) };
    IntRect wJump[6] = { IntRect(19,1639,92,95), IntRect(117,1639,92,93), IntRect(215,1639,90,91), IntRect(315,1639,90,89), IntRect(409,1639,90,86), IntRect(503,1639,92,80) };
    IntRect wShootI[4] = { IntRect(22,1745,85,77), IntRect(120,1747,87,75), IntRect(218,1747,89,75), IntRect(322,1747,93,75) };
    mWeaponAnims[W_IDLE].setup("w_idle", wIdle, 4, 6.f, true);
    mWeaponAnims[W_WALK].setup("w_walk", wWalk, 6, 20.f, true);
    mWeaponAnims[W_JUMP].setup("w_jump", wJump, 4, 6.f, false);
    mWeaponAnims[W_SHOOT_IDLE].setup("w_s_idle", wShootI, 4, 4.f, false);
    mWeaponTextureLoaded = true;
}

void PlayerSoldier::setupPlayer4Pistol()
{
    if (!mWeaponTexture.loadFromFile("Sprites/Group 29.png")) return;
    IntRect wShootI[9] = { IntRect(43,18,108,75), IntRect(155,18,108,75), IntRect(265,16,112,77), IntRect(377,14,66,79), IntRect(445,12,66,81), IntRect(511,18,76,75), IntRect(587,18,76,75), IntRect(665,18,74,77), IntRect(743,18,70,77) };
    IntRect wMelee[6] = { IntRect(51,137,52,82), IntRect(107,137,56,80), IntRect(167,139,70,76), IntRect(241,133,102,88), IntRect(615,139,54,76), IntRect(673,139,62,76) };
    mWeaponAnims[W_SHOOT_IDLE].setup("w_s_idle", wShootI, 4, 12.f, false);
    mWeaponAnims[W_MELEE].setup("w_melee", wMelee, 6, 15.f, false);
    mWeaponTextureLoaded = true;
}
void PlayerSoldier::setupPlayer4HMG()
{
    if (!mWeaponTexture.loadFromFile("Sprites/Group 29.png")) return;
    IntRect wIdle[4] = { IntRect(47,245,70,94), IntRect(119,243,70,96), IntRect(191,241,70,98), IntRect(263,239,70,100) };
    IntRect wWalk[6] = { IntRect(28,344,76,99), IntRect(104,334,76,99), IntRect(181,348,71,94), IntRect(252,348,71,94), IntRect(327,350,71,94), IntRect(403,350,73,94) };
    IntRect wJump[6] = { IntRect(41,464,73,107), IntRect(114,464,73,107), IntRect(187,464,73,107), IntRect(260,464,73,107), IntRect(336,470,72,92), IntRect(412,470,74,84) };
    IntRect wShootI[4] = { IntRect(53,594,130,79), IntRect(183,596,134,77), IntRect(319,596,130,77), IntRect(455,594,130,79) };
    IntRect wMelee[6] = { IntRect(51,137,52,82), IntRect(107,137,56,80), IntRect(167,139,70,76), IntRect(241,133,102,88), IntRect(615,139,54,76), IntRect(673,139,62,76) };
    mWeaponAnims[W_IDLE].setup("w_idle", wIdle, 4, 6.f, true);
    mWeaponAnims[W_WALK].setup("w_walk", wWalk, 6, 20.f, true);
    mWeaponAnims[W_JUMP].setup("w_jump", wJump, 4, 6.f, false);
    mWeaponAnims[W_SHOOT_IDLE].setup("w_s_idle", wShootI, 6, 24.f, true);
    mWeaponAnims[W_MELEE].setup("w_melee", wMelee, 6, 15.f, false);
    mWeaponTextureLoaded = true;
}
void PlayerSoldier::setupPlayer4Flameshot()
{
    if (!mWeaponTexture.loadFromFile("Sprites/Group 29.png")) return;
    IntRect wIdle[4] = { IntRect(31,735,84,80), IntRect(117,735,84,80), IntRect(203,735,84,80), IntRect(289,735,84,80) };
    IntRect wWalk[6] = { IntRect(22,842,87,83), IntRect(109,842,87,83), IntRect(197,842,87,83), IntRect(289,842,87,83), IntRect(373,842,85,84), IntRect(986,842,88,84) };
    IntRect wJump[6] = { IntRect(19,926,87,91), IntRect(105,926,87,91), IntRect(192,926,87,91), IntRect(279,926,87,91), IntRect(366,926,86,82), IntRect(456,926,81,74) };
    IntRect wShootI[6] = { IntRect(24,1028,82,74), IntRect(110,1030,82,72), IntRect(196,1030,82,72), IntRect(282,1028,84,74) };
    IntRect wMelee[6] = { IntRect(51,137,52,82), IntRect(107,137,56,80), IntRect(167,139,70,76), IntRect(241,133,102,88), IntRect(615,139,54,76), IntRect(673,139,62,76) };
    mWeaponAnims[W_IDLE].setup("w_idle", wIdle, 4, 6.f, true);
    mWeaponAnims[W_WALK].setup("w_walk", wWalk, 6, 20.f, true);
    mWeaponAnims[W_JUMP].setup("w_jump", wJump, 4, 6.f, false);
    mWeaponAnims[W_SHOOT_IDLE].setup("w_s_idle", wShootI, 6, 20.f, true);
    mWeaponAnims[W_MELEE].setup("w_melee", wMelee, 6, 15.f, false);
    mWeaponTextureLoaded = true;
}
void PlayerSoldier::setupPlayer4Laser()
{
    if (!mWeaponTexture.loadFromFile("Sprites/Group 29.png")) return;
    IntRect wIdle[4] = { IntRect(27,1129,84,80), IntRect(113,1129,84,80), IntRect(199,1129,84,80), IntRect(285,1129,84,80) };
    IntRect wWalk[6] = { IntRect(18,1236,87,83), IntRect(105,1236,87,83), IntRect(192,1236,87,83), IntRect(279,1236,87,83), IntRect(369,1236,85,84), IntRect(982,1236,88,84) };
    IntRect wJump[6] = { IntRect(15,1320,86,91), IntRect(101,1320,86,91), IntRect(187,1320,86,91), IntRect(274,1320,86,91), IntRect(362,1320,86,82), IntRect(452,1320,81,74) };
    IntRect wShootI[4] = { IntRect(20,1422,82,74), IntRect(106,1424,82,72), IntRect(192,1424,82,72), IntRect(278,1422,84,74) };
    IntRect wMelee[6] = { IntRect(51,137,52,82), IntRect(107,137,56,80), IntRect(167,139,70,76), IntRect(241,133,102,88), IntRect(615,139,54,76), IntRect(673,139,62,76) };
    mWeaponAnims[W_IDLE].setup("w_idle", wIdle, 4, 6.f, true);
    mWeaponAnims[W_WALK].setup("w_walk", wWalk, 6, 20.f, true);
    mWeaponAnims[W_JUMP].setup("w_jump", wJump, 4, 6.f, false);
    mWeaponAnims[W_SHOOT_IDLE].setup("w_s_idle", wShootI, 4, 12.f, false);
    mWeaponAnims[W_MELEE].setup("w_melee", wMelee, 6, 15.f, false);
    mWeaponTextureLoaded = true;
}
void PlayerSoldier::setupPlayer4RL()
{
    if (!mWeaponTexture.loadFromFile("Sprites/Group 29.png")) return;
    IntRect wIdle[4] = { IntRect(17,1513,84,80), IntRect(103,1513,84,80), IntRect(189,1513,84,80), IntRect(275,1513,84,80) };
    IntRect wWalk[6] = { IntRect(8,1620,87,83), IntRect(95,1620,87,83), IntRect(182,1620,87,83), IntRect(269,1620,87,83), IntRect(359,1620,85,84), IntRect(972,1620,88,84) };
    IntRect wJump[6] = { IntRect(5,1704,86,91), IntRect(91,1704,86,91), IntRect(177,1704,86,91), IntRect(263,1704,86,91), IntRect(352,1704,86,82), IntRect(442,1704,84,74) };
    IntRect wShootI[4] = { IntRect(10,1806,82,74), IntRect(96,1808,82,72), IntRect(182,1808,82,72), IntRect(268,1806,84,74) };
    IntRect wMelee[6] = { IntRect(51,137,52,82), IntRect(107,137,56,80), IntRect(167,139,70,76), IntRect(241,133,102,88), IntRect(615,139,54,76), IntRect(673,139,62,76) };
    mWeaponAnims[W_IDLE].setup("w_idle", wIdle, 4, 6.f, true);
    mWeaponAnims[W_WALK].setup("w_walk", wWalk, 6, 20.f, true);
    mWeaponAnims[W_JUMP].setup("w_jump", wJump, 4, 6.f, false);
    mWeaponAnims[W_SHOOT_IDLE].setup("w_s_idle", wShootI, 4, 4.f, false);
    mWeaponAnims[W_MELEE].setup("w_melee", wMelee, 6, 15.f, false);
    mWeaponTextureLoaded = true;
}*/

/*#include "Soldier.h"
#include <SFML/Window/Keyboard.hpp>
#include <cstring>

// ================================================================
//  Tile helper
// ================================================================
static bool isTile(char** lvl, int x, int y, int mapWidth, int mapHeight)
{
    if (x < 0 || x >= mapWidth || y < 0 || y >= mapHeight) return false;
    return lvl[y][x] == 'g';
}

// ================================================================
//  SOLDIER BASE
// ================================================================
Soldier::Soldier(float spd, int hp)
    : speed(spd), health(hp),
    mcurrentAnim(IDLE),
    mCurrentWeaponAnim(W_IDLE),
    mWeaponTextureLoaded(false)
{
}

void Soldier::setPosition(float x, float y)
{
    msprite.setPosition(x, y);
}

Sprite& Soldier::getSprite() { return msprite; }
Sprite& Soldier::getWeaponSprite() { return mWeaponSprite; }

void Soldier::equipWeapon(Weapon* w)
{
    delete m_weapon;
    m_weapon = w;
}

void Soldier::setAnimation(AnimType type)
{
    if (mcurrentAnim == type && !manimations[type].isFinished()) return;
    mcurrentAnim = type;
    manimations[type].reset();
    manimations[type].play();
}

void Soldier::setWeaponAnimation(WeaponAnimType type)
{
    if (mCurrentWeaponAnim == type && !mWeaponAnims[type].isFinished()) return;
    mCurrentWeaponAnim = type;
    mWeaponAnims[type].reset();
    mWeaponAnims[type].play();
}

void Soldier::syncWeaponSprite()
{
    // Keep weapon overlay at the same position & facing as the body
    mWeaponSprite.setPosition(msprite.getPosition());
    if (facingRight)
        mWeaponSprite.setScale(baseScale, baseScale);
    else
        mWeaponSprite.setScale(-baseScale, baseScale);
}

void Soldier::update(float dt, char** lvl, int cell_size, int mapWidth, int mapHeight)
{
    // ---- GRAVITY ----
    if (!onGround)
    {
        velocityY += gravity;
        if (velocityY > 20.f) velocityY = 20.f;
    }
    else
    {
        velocityY = 0.f;
    }

    float bx = msprite.getPosition().x + collisionOffsetX;
    float by = msprite.getPosition().y + collisionOffsetY;
    float bw = collisionWidth;
    float bh = collisionHeight;

    // ---- MOVE X ----
    msprite.move(velocityX, 0);
    bx = msprite.getPosition().x + collisionOffsetX;

    int L = (int)(bx / cell_size);
    int R = (int)((bx + bw - 1) / cell_size);
    int T = (int)(by / cell_size);
    int B = (int)((by + bh - 1) / cell_size);

    if (L < 0) L = 0; else if (L >= mapWidth)  L = mapWidth - 1;
    if (R < 0) R = 0; else if (R >= mapWidth)  R = mapWidth - 1;
    if (T < 0) T = 0; else if (T >= mapHeight) T = mapHeight - 1;
    if (B < 0) B = 0; else if (B >= mapHeight) B = mapHeight - 1;

    if (velocityX > 0)
    {
        for (int i = T; i <= B; i++)
        {
            if (isTile(lvl, R, i, mapWidth, mapHeight))
            {
                msprite.setPosition(R * cell_size - bw - collisionOffsetX, msprite.getPosition().y);
                velocityX = 0; bx = msprite.getPosition().x + collisionOffsetX; break;
            }
        }
    }
    else if (velocityX < 0)
    {
        for (int i = T; i <= B; i++)
        {
            if (isTile(lvl, L, i, mapWidth, mapHeight))
            {
                msprite.setPosition((L + 1) * cell_size - collisionOffsetX, msprite.getPosition().y);
                velocityX = 0; bx = msprite.getPosition().x + collisionOffsetX; break;
            }
        }
    }

    // ---- MOVE Y ----
    msprite.move(0, velocityY);
    by = msprite.getPosition().y + collisionOffsetY;

    L = (int)(bx / cell_size);
    R = (int)((bx + bw - 1) / cell_size);
    T = (int)(by / cell_size);
    B = (int)((by + bh - 1) / cell_size);

    if (L < 0) L = 0; else if (L >= mapWidth)  L = mapWidth - 1;
    if (R < 0) R = 0; else if (R >= mapWidth)  R = mapWidth - 1;
    if (T < 0) T = 0; else if (T >= mapHeight) T = mapHeight - 1;
    if (B < 0) B = 0; else if (B >= mapHeight) B = mapHeight - 1;

    onGround = false;

    if (velocityY >= 0)
    {
        for (int j = L; j <= R; j++)
        {
            if (isTile(lvl, j, B, mapWidth, mapHeight))
            {
                msprite.setPosition(msprite.getPosition().x, B * cell_size - bh - collisionOffsetY);
                velocityY = 0; onGround = true; isJumping = false;
                by = msprite.getPosition().y + collisionOffsetY; break;
            }
        }
    }
    else if (velocityY < 0)
    {
        for (int j = L; j <= R; j++)
        {
            if (isTile(lvl, j, T, mapWidth, mapHeight))
            {
                msprite.setPosition(msprite.getPosition().x, (T + 1) * cell_size - collisionOffsetY);
                velocityY = 0; isJumping = false;
                by = msprite.getPosition().y + collisionOffsetY; break;
            }
        }
    }

    // ---- CLAMP ----
    bx = msprite.getPosition().x + collisionOffsetX;
    by = msprite.getPosition().y + collisionOffsetY;
    float cx = bx, cy = by;
    if (cx < 0.f) cx = 0.f;
    else if (cx > mapWidth * cell_size - bw) cx = mapWidth * cell_size - bw;
    if (cy < 0.f) cy = 0.f;
    else if (cy > mapHeight * cell_size - bh) { cy = mapHeight * cell_size - bh; velocityY = 0; onGround = true; isJumping = false; }
    msprite.setPosition(cx - collisionOffsetX, cy - collisionOffsetY);

    // ---- BODY ANIMATION ----
    bool isRunning = (velocityX != 0.f);
    if (isJumping)      setAnimation(isRunning ? JUMP_RUN : JUMP_IDLE);
    else if (isRunning) setAnimation(WALK);
    else                setAnimation(IDLE);

    manimations[mcurrentAnim].update(dt);
    msprite.setTextureRect(manimations[mcurrentAnim].getCurrentFrame());
    msprite.setScale(facingRight ? baseScale : -baseScale, baseScale);

    // ---- WEAPON OVERLAY ANIMATION ----
    if (mWeaponTextureLoaded)
    {
        mWeaponAnims[mCurrentWeaponAnim].update(dt);
        mWeaponSprite.setTextureRect(mWeaponAnims[mCurrentWeaponAnim].getCurrentFrame());
        syncWeaponSprite();
    }

    // ---- UPDATE EQUIPPED WEAPON ----
    if (m_weapon) m_weapon->update(dt);
}

// ================================================================
//  PLAYER SOLDIER
// ================================================================
PlayerSoldier::PlayerSoldier(int index) : Soldier(3.f, 100), mplayerIndex(index)
{
    loadPlayerData();
    setAnimation(IDLE);
    equipWeapon(new Pistol());
}

void PlayerSoldier::update(float dt, char** lvl, int cell_size, int mapWidth, int mapHeight)
{
    handleInput(dt);
    Soldier::update(dt, lvl, cell_size, mapWidth, mapHeight);
}

// ================================================================
//  INPUT
// ================================================================
void PlayerSoldier::handleInput(float dt)
{
    velocityX = 0;
    mIsShooting = false;

    // --- Fire key (Space for all players for now) ---
    bool fireKey = Keyboard::isKeyPressed(Keyboard::Space);

    if (mplayerIndex == 0)
    {
        if (Keyboard::isKeyPressed(Keyboard::Right)) { velocityX = speed; facingRight = true; }
        if (Keyboard::isKeyPressed(Keyboard::Left)) { velocityX = -speed; facingRight = false; }
        if (Keyboard::isKeyPressed(Keyboard::Up) && onGround)
        {
            velocityY = jumpStrength; onGround = false; isJumping = true;
        }
        fireKey = Keyboard::isKeyPressed(Keyboard::RControl);
    }
    else if (mplayerIndex == 1)
    {
        if (Keyboard::isKeyPressed(Keyboard::D)) { velocityX = speed; facingRight = true; }
        if (Keyboard::isKeyPressed(Keyboard::A)) { velocityX = -speed; facingRight = false; }
        if (Keyboard::isKeyPressed(Keyboard::W) && onGround)
        {
            velocityY = jumpStrength; onGround = false; isJumping = true;
        }
        fireKey = Keyboard::isKeyPressed(Keyboard::LControl);

        if (onGround) coyoteTime = coyoteMax;
        else          coyoteTime -= dt;
    }
    // Players 2 & 3 use same body as player 1 for now

    // --- Fire ---
    if (fireKey && m_weapon)
    {
        mIsShooting = true;
        float ox = msprite.getPosition().x + (facingRight ? collisionWidth : 0.f);
        float oy = msprite.getPosition().y + collisionHeight * 0.4f;
        m_weapon->fire(ox, oy, facingRight, 0.f);
    }

    // --- Select weapon animation ---
    if (mWeaponTextureLoaded)
    {
        bool airborne = isJumping || !onGround;
        bool moving = (velocityX != 0.f);

        if (mIsShooting)
        {
            if (airborne) setWeaponAnimation(W_SHOOT_JUMP);
            else if (moving)   setWeaponAnimation(W_SHOOT_WALK);
            else               setWeaponAnimation(W_SHOOT_IDLE);
        }
        else
        {
            if (airborne) setWeaponAnimation(W_JUMP);
            else if (moving)   setWeaponAnimation(W_WALK);
            else               setWeaponAnimation(W_IDLE);
        }
    }
}

// ================================================================
//  LOAD PLAYER DATA  (body texture + base animations)
// ================================================================
void PlayerSoldier::loadPlayerData()
{
    if (mplayerIndex == 0)
    {
        mtexture.loadFromFile("Sprites/Group 10.png");
        setupPlayer4Animations();
        baseScale = 2.5f;
    }
    else if (mplayerIndex == 1)
    {
        mtexture.loadFromFile("Sprites/Group 8.png");
        setupPlayer2Animations();
        baseScale = 1.65f;
    }
    else if (mplayerIndex == 2)
    {
        mtexture.loadFromFile("Sprites/Group 5.png");
        setupPlayer1Animations();
        baseScale = 1.f;
    }
    else if (mplayerIndex == 3)
    {
        mtexture.loadFromFile("Sprites/Group 9.png");
        setupPlayer3Animations();
        baseScale = 1.25f;
    }
    msprite.setTexture(mtexture);
}

// ================================================================
//  EQUIP WEAPON  — swap weapon + reload matching overlay sheet
// ================================================================
void PlayerSoldier::equipWeapon(Weapon* w)
{
    Soldier::equipWeapon(w);
    if (!m_weapon) return;

    const char* name = m_weapon->getName();
    mWeaponTextureLoaded = false;   // default: no overlay until sheet loads

    // Dispatch to per-player × per-weapon setup function
    if (mplayerIndex == 0)
    {
        if (strcmp(name, "pistol") == 0) setupPlayer1Pistol();
        else if (strcmp(name, "hmg") == 0) setupPlayer1HMG();
        else if (strcmp(name, "flameshot") == 0) setupPlayer1Flameshot();
        else if (strcmp(name, "laserbeam") == 0) setupPlayer1Laser();
        else if (strcmp(name, "rocket") == 0) setupPlayer1RL();
    }
    else if (mplayerIndex == 1)
    {
        if (strcmp(name, "pistol") == 0) setupPlayer2Pistol();
        else if (strcmp(name, "hmg") == 0) setupPlayer2HMG();
        else if (strcmp(name, "flameshot") == 0) setupPlayer2Flameshot();
        else if (strcmp(name, "laserbeam") == 0) setupPlayer2Laser();
        else if (strcmp(name, "rocket") == 0) setupPlayer2RL();
    }
    else if (mplayerIndex == 2)
    {
        if (strcmp(name, "pistol") == 0) setupPlayer3Pistol();
        else if (strcmp(name, "hmg") == 0) setupPlayer3HMG();
        else if (strcmp(name, "flameshot") == 0) setupPlayer3Flameshot();
        else if (strcmp(name, "laserbeam") == 0) setupPlayer3Laser();
        else if (strcmp(name, "rocket") == 0) setupPlayer3RL();
    }
    else if (mplayerIndex == 3)
    {
        if (strcmp(name, "pistol") == 0) setupPlayer4Pistol();
        else if (strcmp(name, "hmg") == 0) setupPlayer4HMG();
        else if (strcmp(name, "flameshot") == 0) setupPlayer4Flameshot();
        else if (strcmp(name, "laserbeam") == 0) setupPlayer4Laser();
        else if (strcmp(name, "rocket") == 0) setupPlayer4RL();
    }

    if (mWeaponTextureLoaded)
    {
        mWeaponSprite.setTexture(mWeaponTexture);
        setWeaponAnimation(W_IDLE);
    }
}

// ================================================================
//  BODY ANIMATIONS  (unchanged from original)
// ================================================================
void PlayerSoldier::setupPlayer1Animations()
{
    IntRect idle[4] = { IntRect(90,4,83,101), IntRect(174,4,84,101), IntRect(264,4,81,101), IntRect(346,4,81,101) };
    IntRect walk[12] = { IntRect(88,126,79,90), IntRect(168,126,78,92), IntRect(246,126,78,92), IntRect(325,128,77,99), IntRect(402,128,77,99), IntRect(485,128,77,99), IntRect(567,130,77,97), IntRect(647,130,77,99), IntRect(727,130,79,97), IntRect(811,130,78,97), IntRect(889,130,78,97), IntRect(971,130,74,97) };
    IntRect jumpIdle[6] = { IntRect(88,238,66,106), IntRect(157,242,70,102), IntRect(232,244,71,98), IntRect(308,242,69,98), IntRect(384,242,69,92), IntRect(459,244,67,84) };
    IntRect jumpRun[6] = { IntRect(86,362,73,97), IntRect(159,362,73,97), IntRect(239,362,79,88), IntRect(318,360,73,90), IntRect(391,360,73,90), IntRect(464,360,73,90) };
    IntRect swimIdle[6] = { IntRect(22,803,136,77), IntRect(165,801,139,75), IntRect(312,812,137,67), IntRect(457,812,140,64), IntRect(598,810,141,64), IntRect(758,808,140,64) };
    IntRect swim[10] = { IntRect(34,916,132,74), IntRect(176,911,130,67), IntRect(314,916,133,61), IntRect(456,906,139,64), IntRect(600,908,138,61), IntRect(756,905,140,60), IntRect(898,900,138,66), IntRect(1054,895,136,66), IntRect(1196,889,130,77), IntRect(1337,890,131,76) };
    IntRect die[21] = { IntRect(78,687,72,93), IntRect(155,694,79,86), IntRect(239,694,83,86), IntRect(328,692,68,88), IntRect(399,685,69,95), IntRect(471,682,74,98), IntRect(548,680,71,100), IntRect(622,680,68,100), IntRect(696,680,61,100), IntRect(761,687,66,93), IntRect(832,690,67,90), IntRect(902,694,71,86), IntRect(976,706,78,74), IntRect(1057,704,74,76), IntRect(1136,706,69,74), IntRect(1208,709,66,71), IntRect(1277,713,69,67), IntRect(1351,718,69,62), IntRect(1426,728,71,52), IntRect(1500,742,88,38), IntRect(1588,740,84,40) };
    manimations[IDLE].setup("idle", idle, 4, 6.f, true);
    manimations[WALK].setup("walk", walk, 12, 20.f, true);
    manimations[SWIM_IDLE].setup("swim_idle", swimIdle, 6, 8.f, false);
    manimations[SWIM].setup("swim", swim, 10, 8.f, false);
    manimations[JUMP_IDLE].setup("jump_idle", jumpIdle, 6, 6.f, false);
    manimations[JUMP_RUN].setup("jump_run", jumpRun, 6, 6.f, false);
    manimations[DIE].setup("die", die, 21, 5.f, false);
}

void PlayerSoldier::setupPlayer2Animations()
{
    IntRect idle[4] = { IntRect(8,10,48,60), IntRect(61,10,48,61), IntRect(115,10,49,61), IntRect(170,10,51,61) };
    IntRect walk[6] = { IntRect(2,82,48,67), IntRect(58,82,49,67), IntRect(115,82,48,67), IntRect(170,82,50,67), IntRect(223,82,52,67), IntRect(281,82,50,67) };
    IntRect jumpIdle[6] = { IntRect(3,169,47,70), IntRect(53,170,48,69), IntRect(107,172,48,67), IntRect(158,173,48,66), IntRect(213,173,47,63), IntRect(264,173,47,62) };
    IntRect jumpRun[6] = { IntRect(329,170,48,66), IntRect(382,170,48,63), IntRect(436,170,55,60), IntRect(496,169,49,58), IntRect(549,169,49,58), IntRect(607,169,43,60) };
    IntRect die[19] = { IntRect(4,253,43,60), IntRect(58,256,49,55), IntRect(113,257,47,56), IntRect(169,256,46,57), IntRect(226,254,48,56), IntRect(283,260,42,50), IntRect(332,260,44,50), IntRect(389,263,47,48), IntRect(446,268,53,43), IntRect(510,266,56,42), IntRect(572,271,50,34), IntRect(631,265,48,40), IntRect(687,265,40,42), IntRect(736,260,37,48), IntRect(778,265,39,45), IntRect(826,268,43,45), IntRect(874,278,48,35), IntRect(927,284,53,29), IntRect(985,289,57,24) };
    manimations[IDLE].setup("idle", idle, 4, 6.f, true);
    manimations[WALK].setup("walk", walk, 6, 15.f, true);
    manimations[JUMP_IDLE].setup("jump_idle", jumpIdle, 6, 6.f, false);
    manimations[JUMP_RUN].setup("jump_run", jumpRun, 6, 6.f, false);
    manimations[DIE].setup("die", die, 19, 5.f, false);
}

void PlayerSoldier::setupPlayer3Animations()
{
    IntRect idle[4] = { IntRect(0,1,63,75), IntRect(68,1,61,75), IntRect(136,1,63,75), IntRect(206,1,66,75) };
    IntRect walk[6] = { IntRect(0,82,67,85), IntRect(67,82,67,85), IntRect(135,82,63,85), IntRect(195,82,63,67), IntRect(262,82,64,84), IntRect(326,82,67,83) };
    IntRect jumpIdle[6] = { IntRect(7,170,60,99), IntRect(75,172,60,98), IntRect(145,175,60,91), IntRect(213,175,60,87), IntRect(281,175,60,77), IntRect(349,175,60,78) };
    IntRect jumpRun[6] = { IntRect(1,272,61,94), IntRect(71,272,63,87), IntRect(143,272,73,78), IntRect(237,272,66,73), IntRect(310,272,68,75), IntRect(393,272,57,81) };
    IntRect die[19] = { IntRect(4,253,43,60), IntRect(58,256,49,55), IntRect(113,257,47,56), IntRect(169,256,46,57), IntRect(226,254,48,56), IntRect(283,260,42,50), IntRect(332,260,44,50), IntRect(389,263,47,48), IntRect(446,268,53,43), IntRect(510,266,56,42), IntRect(572,271,50,34), IntRect(631,265,48,40), IntRect(687,265,40,42), IntRect(736,260,37,48), IntRect(778,265,39,45), IntRect(826,268,43,45), IntRect(874,278,48,35), IntRect(927,284,53,29), IntRect(985,289,57,24) };
    manimations[IDLE].setup("idle", idle, 4, 6.f, true);
    manimations[WALK].setup("walk", walk, 6, 15.f, true);
    manimations[JUMP_IDLE].setup("jump_idle", jumpIdle, 6, 6.f, false);
    manimations[JUMP_RUN].setup("jump_run", jumpRun, 6, 6.f, false);
    manimations[DIE].setup("die", die, 19, 5.f, false);
}

void PlayerSoldier::setupPlayer4Animations()
{
    IntRect idle[4] = { IntRect(4,0,31,37), IntRect(37,0,31,38), IntRect(70,0,31,38), IntRect(103,0,31,38) };
    IntRect walk[14] = { IntRect(0,54,32,39), IntRect(32,54,32,39), IntRect(65,56,31,35), IntRect(96,55,32,37), IntRect(132,54,28,38), IntRect(164,54,29,39), IntRect(196,55,30,38), IntRect(227,56,33,39), IntRect(260,56,33,39), IntRect(293,56,33,39), IntRect(328,55,29,38), IntRect(359,55,30,38), IntRect(164,54,29,39), IntRect(196,55,30,38) };
    IntRect jumpIdle[6] = { IntRect(6,105,29,45), IntRect(40,107,30,43), IntRect(75,109,30,40), IntRect(110,109,30,39), IntRect(145,109,29,37), IntRect(178,109,29,35) };
    IntRect jumpRun[6] = { IntRect(9,163,29,43), IntRect(38,163,29,43), IntRect(71,163,34,32), IntRect(106,163,30,35), IntRect(138,163,30,36), IntRect(168,163,26,37) };
    IntRect die[19] = { IntRect(4,253,43,60), IntRect(58,256,49,55), IntRect(113,257,47,56), IntRect(169,256,46,57), IntRect(226,254,48,56), IntRect(283,260,42,50), IntRect(332,260,44,50), IntRect(389,263,47,48), IntRect(446,268,53,43), IntRect(510,266,56,42), IntRect(572,271,50,34), IntRect(631,265,48,40), IntRect(687,265,40,42), IntRect(736,260,37,48), IntRect(778,265,39,45), IntRect(826,268,43,45), IntRect(874,278,48,35), IntRect(927,284,53,29), IntRect(985,289,57,24) };
    manimations[IDLE].setup("idle", idle, 4, 6.f, true);
    manimations[WALK].setup("walk", walk, 14, 20.f, true);
    manimations[JUMP_IDLE].setup("jump_idle", jumpIdle, 6, 6.f, false);
    manimations[JUMP_RUN].setup("jump_run", jumpRun, 6, 6.f, false);
    manimations[DIE].setup("die", die, 19, 5.f, false);
}

// ================================================================
//  WEAPON OVERLAY SETUP  — one function per player x weapon
//
//  Same pattern as body animations:
//    - declare IntRect array with real pixel coords from your sheet
//    - call mWeaponAnims[W_*].setup(name, array, count, fps, looping)
//    - set mWeaponTextureLoaded = true at the end
//
//  All IntRect values below are (0,0,1,1) placeholders.
//  Replace them with the real coords from your weapon sprite sheets,
//  exactly as you filled in the body animation IntRects above.
// ================================================================

// ----------------------------------------------------------------
//  PLAYER 1  (Marco)
// ----------------------------------------------------------------

void PlayerSoldier::setupPlayer1Pistol()
{
    // Body sheet already covers idle/walk/jump with pistol.
    // Only load the attack overlay frames.
    if (!mWeaponTexture.loadFromFile("Sprites/p1_pistol.png")) return;

    IntRect wShootI[10] = { IntRect(43,13,108,78), IntRect(155,13,108,78), IntRect(267,13,110,78), IntRect(381,13,80,82) , IntRect(465,9,80,86) , IntRect(547,9,82,84) , IntRect(631,13,82,80), IntRect(715,13,76,80), IntRect(795,13,72,80), IntRect(869,11,72,82) };
    IntRect wMelee[6] = { IntRect(31,135,54,84), IntRect(89,135,52,84), IntRect(145,135,58,86), IntRect(207,137,88,82), IntRect(299,128,102,89), IntRect(403,128,104,89) };

    mWeaponAnims[W_SHOOT_IDLE].setup("w_s_idle", wShootI, 4, 12.f, false);
    mWeaponAnims[W_MELEE].setup("w_melee", wMelee, 6, 15.f, false);

    mWeaponTextureLoaded = true;
}

void PlayerSoldier::setupPlayer1HMG()
{
    if (!mWeaponTexture.loadFromFile("Sprites/p1_hmg.png")) return;

    IntRect wIdle[4] = { IntRect(45,247,78,100), IntRect(127,249,80,98), IntRect(211,251,82,96), IntRect(297,253,84,94) };
    IntRect wWalk[6] = { IntRect(32,370,86,104), IntRect(121,368,83,104), IntRect(208,368,80,105), IntRect(292,368,82,107), IntRect(374,372,88,102), IntRect(466,374,88,99) };
    IntRect wJump[5] = { IntRect(28,504,84,112), IntRect(202,502,80,112), IntRect(286,502,82,110), IntRect(372,506,84,100), IntRect(460,508,88,95) };
    IntRect wShootI[4] = { IntRect(19,646,135,79), IntRect(157,646,137,81), IntRect(299,646,133,81), IntRect(437,646,137,83) };

    IntRect wMelee[6] = { IntRect(31,135,54,84), IntRect(89,135,52,84), IntRect(145,135,58,86), IntRect(207,137,88,82), IntRect(299,128,102,89), IntRect(403,128,104,89) };

    mWeaponAnims[W_IDLE].setup("w_idle", wIdle, 4, 6.f, true);
    mWeaponAnims[W_WALK].setup("w_walk", wWalk, 6, 20.f, true);
    mWeaponAnims[W_JUMP].setup("w_jump", wJump, 4, 6.f, false);
    mWeaponAnims[W_SHOOT_IDLE].setup("w_s_idle", wShootI, 6, 24.f, true);

    mWeaponAnims[W_MELEE].setup("w_melee", wMelee, 6, 15.f, false);
    mWeaponTextureLoaded = true;
}

void PlayerSoldier::setupPlayer1Flameshot()
{
    if (!mWeaponTexture.loadFromFile("Sprites/p1_flame.png")) return;

    IntRect wIdle[4] = { IntRect(15,767,83,84), IntRect(99,767,83,86), IntRect(185,767,83,86), IntRect(271,767,85,84) };
    IntRect wWalk[6] = { IntRect(19,877,90,91), IntRect(113,875,85,91), IntRect(201,875,83,92), IntRect(286,887,88,92), IntRect(374,887,88,92), IntRect(467,877,90,90) };
    IntRect wJump[6] = { IntRect(19,985,90,99), IntRect(113,983,87,99), IntRect(201,983,83,101), IntRect(287,985,84,95), IntRect(375,985,88,89), IntRect(467,985,91,84) };
    IntRect wShootI[4] = { IntRect(19,1096,85,79), IntRect(105,1098,83,77), IntRect(189,1096,81,79), IntRect(271,1096,83,79) };
    IntRect wMelee[6] = { IntRect(31,135,54,84), IntRect(89,135,52,84), IntRect(145,135,58,86), IntRect(207,137,88,82), IntRect(299,128,102,89), IntRect(403,128,104,89) };
    //IntRect wMelee[4] = { IntRect(0,0,1,1), IntRect(0,0,1,1), IntRect(0,0,1,1), IntRect(0,0,1,1) };

    mWeaponAnims[W_IDLE].setup("w_idle", wIdle, 4, 6.f, true);
    mWeaponAnims[W_WALK].setup("w_walk", wWalk, 6, 20.f, true);
    mWeaponAnims[W_JUMP].setup("w_jump", wJump, 6, 6.f, false);
    mWeaponAnims[W_SHOOT_IDLE].setup("w_s_idle", wShootI, 4, 20.f, true);

    mWeaponAnims[W_MELEE].setup("w_melee", wMelee, 6, 15.f, false);
    mWeaponTextureLoaded = true;
}

void PlayerSoldier::setupPlayer1Laser()
{
    if (!mWeaponTexture.loadFromFile("Sprites/p1_laser.png")) return;

    IntRect wIdle[4] = { IntRect(21,1201,83,86), IntRect(104,1201,83,86), IntRect(191,1201,84,86), IntRect(277,1201,82,84) };
    IntRect wWalk[6] = { IntRect(25,1311,90,91), IntRect(119,1309,84,91), IntRect(207,1309,82,92), IntRect(292,1311,88,92), IntRect(380,1311,88,92), IntRect(473,1311,90,90) };
    IntRect wJump[6] = { IntRect(25,1419,90,99), IntRect(119,1417,84,99), IntRect(207,1417,82,101), IntRect(293,1419,84,95), IntRect(381,1419,88,89), IntRect(473,1419,90,84) };
    IntRect wShootI[4] = { IntRect(25,1530,84,80), IntRect(111,1532,80,78), IntRect(195,1530,78,80), IntRect(277,1530,85,80) };

    IntRect wMelee[6] = { IntRect(31,135,54,84), IntRect(89,135,52,84), IntRect(145,135,58,86), IntRect(207,137,88,82), IntRect(299,128,102,89), IntRect(403,128,104,89) };

    mWeaponAnims[W_IDLE].setup("w_idle", wIdle, 4, 6.f, true);
    mWeaponAnims[W_WALK].setup("w_walk", wWalk, 6, 20.f, true);
    mWeaponAnims[W_JUMP].setup("w_jump", wJump, 4, 6.f, false);
    mWeaponAnims[W_SHOOT_IDLE].setup("w_s_idle", wShootI, 4, 12.f, false);
    mWeaponAnims[W_MELEE].setup("w_melee", wMelee, 6, 15.f, false);
    mWeaponTextureLoaded = true;
}

void PlayerSoldier::setupPlayer1RL()
{
    if (!mWeaponTexture.loadFromFile("Sprites/p1_rocket.png")) return;

    IntRect wIdle[4] = { IntRect(24,1641,83,84), IntRect(110,1641,83,86), IntRect(197,1641,82,86), IntRect(282,1641,84,84) };
    IntRect wWalk[6] = { IntRect(31,1751,90,91), IntRect(125,1749,84,91), IntRect(212,1749,83,92), IntRect(296,1751,89,92), IntRect(385,1751,89,92), IntRect(479,1751,90,90) };
    IntRect wJump[6] = { IntRect(31,1859,90,99), IntRect(125,1857,84,99), IntRect(213,1857,82,101), IntRect(296,1859,87,95), IntRect(387,1859,88,89), IntRect(479,1859,92,84) };
    IntRect wShootI[4] = { IntRect(30,1970,83,80), IntRect(116,1972,81,78), IntRect(200,1970,79,80), IntRect(283,1970,85,80) };

    IntRect wMelee[6] = { IntRect(31,135,54,84), IntRect(89,135,52,84), IntRect(145,135,58,86), IntRect(207,137,88,82), IntRect(299,128,102,89), IntRect(403,128,104,89) };

    mWeaponAnims[W_IDLE].setup("w_idle", wIdle, 4, 6.f, true);
    mWeaponAnims[W_WALK].setup("w_walk", wWalk, 6, 20.f, true);
    mWeaponAnims[W_JUMP].setup("w_jump", wJump, 4, 6.f, false);
    mWeaponAnims[W_SHOOT_IDLE].setup("w_s_idle", wShootI, 4, 4.f, false);

    mWeaponAnims[W_MELEE].setup("w_melee", wMelee, 6, 15.f, false);
    mWeaponTextureLoaded = true;
}

// ----------------------------------------------------------------
//  PLAYER 2  (Tarma)
// ----------------------------------------------------------------

void PlayerSoldier::setupPlayer2Pistol()
{
    if (!mWeaponTexture.loadFromFile("Sprites/p2_pistol.png")) return;

    IntRect wShootI[9] = { IntRect(35,9,106,81), IntRect(147,11,108,79), IntRect(259,9,110,81), IntRect(373,7,80,83), IntRect(457,3,80,87), IntRect(541,3,80,85), IntRect(624,7,81,79), IntRect(707,7,74,81), IntRect(861,5,70,81) };
    IntRect wMelee[6] = { IntRect(29,115,54,83), IntRect(95,115,52,85), IntRect(157,115,58,85), IntRect(227,115,88,83), IntRect(325,107,102,91), IntRect(437,107,102,91) };

    mWeaponAnims[W_SHOOT_IDLE].setup("w_s_idle", wShootI, 4, 12.f, false);

    mWeaponAnims[W_MELEE].setup("w_melee", wMelee, 6, 15.f, false);
    mWeaponTextureLoaded = true;
}

void PlayerSoldier::setupPlayer2HMG()
{
    if (!mWeaponTexture.loadFromFile("Sprites/p2_hmg.png")) return;

    IntRect wIdle[4] = { IntRect(37,225,78,101), IntRect(129,227,80,99), IntRect(223,229,82,97), IntRect(313,231,84,93) };
    IntRect wWalk[9] = { IntRect(43,363,86,104), IntRect(43,363,86,104), IntRect(233,361,88,99), IntRect(337,361,82,106), IntRect(431,367,84,101), IntRect(527,369,88,99), IntRect(627,371,84,99), IntRect(716,367,89,103), IntRect(921,365,90,105) };
    IntRect wJump[6] = { IntRect(37,479,84,113), IntRect(135,477,82,113), IntRect(233,477,80,111), IntRect(329,477,82,109), IntRect(423,483,84,99) , IntRect(519,485,88,93) };
    IntRect wShootI[4] = { IntRect(40,628,133,80), IntRect(180,628,135,82), IntRect(322,628,133,82), IntRect(466,630,137,80) };

    IntRect wMelee[6] = { IntRect(29,115,54,83), IntRect(95,115,52,85), IntRect(157,115,58,85), IntRect(227,115,88,83), IntRect(325,107,102,91), IntRect(437,107,102,91) };

    //IntRect wMelee[4] = { IntRect(0,0,1,1), IntRect(0,0,1,1), IntRect(0,0,1,1), IntRect(0,0,1,1) };

    mWeaponAnims[W_IDLE].setup("w_idle", wIdle, 4, 6.f, true);
    mWeaponAnims[W_WALK].setup("w_walk", wWalk, 6, 20.f, true);
    mWeaponAnims[W_JUMP].setup("w_jump", wJump, 4, 6.f, false);
    mWeaponAnims[W_SHOOT_IDLE].setup("w_s_idle", wShootI, 6, 24.f, true);
    mWeaponAnims[W_MELEE].setup("w_melee", wMelee, 6, 15.f, false);
    mWeaponTextureLoaded = true;
}

void PlayerSoldier::setupPlayer2Flameshot()
{
    if (!mWeaponTexture.loadFromFile("Sprites/p2_flame.png")) return;

    IntRect wIdle[4] = { IntRect(26,741,83,79), IntRect(114,741,83,79), IntRect(206,741,83,83), IntRect(299,741,82,83) };
    IntRect wWalk[9] = { IntRect(31,845,92,86), IntRect(125,843,90,88), IntRect(217,841,88,83), IntRect(311,843,84,88), IntRect(403,843,88,89), IntRect(501,845,90,87), IntRect(899,845,90,89), IntRect(997,845,92,88), IntRect(1095,845,92,88) };
    IntRect wJump[6] = { IntRect(23,972,89,96), IntRect(121,970,84,96), IntRect(213,968,82,98), IntRect(301,970,83,96), IntRect(393,970,87,88), IntRect(491,972,89,84) };
    IntRect wShootI[4] = { IntRect(28,1071,83,80), IntRect(120,1073,79,78), IntRect(210,1071,77,80), IntRect(298,1071,81,80) };

    IntRect wMelee[6] = { IntRect(29,115,54,83), IntRect(95,115,52,85), IntRect(157,115,58,85), IntRect(227,115,88,83), IntRect(325,107,102,91), IntRect(437,107,102,91) };

    //IntRect wMelee[4] = { IntRect(0,0,1,1), IntRect(0,0,1,1), IntRect(0,0,1,1), IntRect(0,0,1,1) };

    mWeaponAnims[W_IDLE].setup("w_idle", wIdle, 4, 6.f, true);
    mWeaponAnims[W_WALK].setup("w_walk", wWalk, 6, 20.f, true);
    mWeaponAnims[W_JUMP].setup("w_jump", wJump, 4, 6.f, false);
    mWeaponAnims[W_SHOOT_IDLE].setup("w_s_idle", wShootI, 6, 20.f, true);

    mWeaponAnims[W_MELEE].setup("w_melee", wMelee, 6, 15.f, false);
    mWeaponTextureLoaded = true;
}

void PlayerSoldier::setupPlayer2Laser()
{
    if (!mWeaponTexture.loadFromFile("Sprites/p2_laser.png")) return;

    IntRect wIdle[4] = { IntRect(8,1179,83,79), IntRect(96,1179,85,79), IntRect(188,1179,83,83), IntRect(281,1179,84,83) };
    IntRect wWalk[9] = { IntRect(13,1283,92,86), IntRect(107,1281,90,88), IntRect(199,1279,88,83), IntRect(293,1281,84,88), IntRect(385,1281,88,89), IntRect(483,1283,90,87), IntRect(579,1283,90,89), IntRect(979,1283,92,88), IntRect(1077,1283,92,88) };
    IntRect wJump[6] = { IntRect(5,1410,89,96), IntRect(103,1408,84,96), IntRect(195,1406,81,98), IntRect(283,1408,83,96), IntRect(375,1408,87,88), IntRect(473,1410,89,84) };
    IntRect wShootI[4] = { IntRect(10,1509,83,80), IntRect(102,1511,79,78), IntRect(192,1509,79,80), IntRect(280,1509,81,80) };
    IntRect wMelee[6] = { IntRect(29,115,54,83), IntRect(95,115,52,85), IntRect(157,115,58,85), IntRect(227,115,88,83), IntRect(325,107,102,91), IntRect(437,107,102,91) };

    //IntRect wMelee[4] = { IntRect(0,0,1,1), IntRect(0,0,1,1), IntRect(0,0,1,1), IntRect(0,0,1,1) };

    mWeaponAnims[W_IDLE].setup("w_idle", wIdle, 4, 6.f, true);
    mWeaponAnims[W_WALK].setup("w_walk", wWalk, 6, 20.f, true);
    mWeaponAnims[W_JUMP].setup("w_jump", wJump, 4, 6.f, false);
    mWeaponAnims[W_SHOOT_IDLE].setup("w_s_idle", wShootI, 4, 12.f, false);
    mWeaponAnims[W_MELEE].setup("w_melee", wMelee, 6, 15.f, false);
    mWeaponTextureLoaded = true;
}

void PlayerSoldier::setupPlayer2RL()
{
    if (!mWeaponTexture.loadFromFile("Sprites/p2_rocket.png")) return;

    IntRect wIdle[4] = { IntRect(12,1609,81,79), IntRect(100,1609,83,79), IntRect(192,1609,83,83), IntRect(284,1609,83,83) };
    IntRect wWalk[8] = { IntRect(17,1713,92,86), IntRect(111,1711,91,88), IntRect(203,1709,88,83), IntRect(294,1711,87,88), IntRect(389,1711,88,89), IntRect(885,1713,90,89), IntRect(983,1713,92,88), IntRect(1081,1713,92,88) };
    IntRect wJump[6] = { IntRect(9,1840,89,96), IntRect(106,1838,84,96), IntRect(196,1836,84,98), IntRect(282,1838,88,96), IntRect(379,1838,87,88), IntRect(477,1840,89,84) };
    IntRect wShootI[4] = { IntRect(12,1939,83,80), IntRect(104,1941,82,78), IntRect(192,1939,84,80), IntRect(279,1939,88,80) };
    IntRect wMelee[6] = { IntRect(29,115,54,83), IntRect(95,115,52,85), IntRect(157,115,58,85), IntRect(227,115,88,83), IntRect(325,107,102,91), IntRect(437,107,102,91) };


    //IntRect wMelee[4] = { IntRect(0,0,1,1), IntRect(0,0,1,1), IntRect(0,0,1,1), IntRect(0,0,1,1) };

    mWeaponAnims[W_IDLE].setup("w_idle", wIdle, 4, 6.f, true);
    mWeaponAnims[W_WALK].setup("w_walk", wWalk, 6, 20.f, true);
    mWeaponAnims[W_JUMP].setup("w_jump", wJump, 4, 6.f, false);
    mWeaponAnims[W_SHOOT_IDLE].setup("w_s_idle", wShootI, 4, 4.f, false);

    mWeaponAnims[W_MELEE].setup("w_melee", wMelee, 6, 15.f, false);
    mWeaponTextureLoaded = true;
}

// ----------------------------------------------------------------
//  PLAYER 3  (Eri)
// ----------------------------------------------------------------

void PlayerSoldier::setupPlayer3Pistol()
{
    if (!mWeaponTexture.loadFromFile("Sprites/p3_pistol.png")) return;

    IntRect wShootI[9] = { IntRect(81,11,108,80), IntRect(195,9,112,82), IntRect(311,11,74,80), IntRect(429,11,70,80), IntRect(505,9,68,82), IntRect(583,13,78,78), IntRect(829,11,78,80) };

    mWeaponAnims[W_SHOOT_IDLE].setup("w_s_idle", wShootI, 4, 12.f, false);
    mWeaponTextureLoaded = true;
}

void PlayerSoldier::setupPlayer3HMG()
{
    if (!mWeaponTexture.loadFromFile("Sprites/p3_hmg.png")) return;

    IntRect wIdle[4] = { IntRect(69,135,76,96), IntRect(157,133,76,98), IntRect(245,131,74,100), IntRect(329,129,74,102) };
    IntRect wWalk[12] = { IntRect(68,256,77,95), IntRect(154,258,77,92), IntRect(240,262,75,90), IntRect(326,264,75,87), IntRect(410,262,77,89), IntRect(494,262,81,95), IntRect(582,262,77,93), IntRect(670,260,77,96), IntRect(760,256,79,101), IntRect(852,258,81,97), IntRect(940,262,81,91), IntRect(1023,258,82,98) };
    IntRect wJump[6] = { IntRect(58,370,77,108), IntRect(144,372,77,104), IntRect(230,376,75,100), IntRect(316,378,75,96), IntRect(400,376,77,93), IntRect(486,376,79,87) };
    IntRect wShootI[4] = { IntRect(45,607,88,82), IntRect(133,607,88,82), IntRect(221,613,90,84), IntRect(313,615,90,82) };

    mWeaponAnims[W_IDLE].setup("w_idle", wIdle, 4, 6.f, true);
    mWeaponAnims[W_WALK].setup("w_walk", wWalk, 6, 20.f, true);
    mWeaponAnims[W_JUMP].setup("w_jump", wJump, 4, 6.f, false);
    mWeaponAnims[W_SHOOT_IDLE].setup("w_s_idle", wShootI, 6, 24.f, true);

    mWeaponTextureLoaded = true;
}

void PlayerSoldier::setupPlayer3Flameshot()
{
    if (!mWeaponTexture.loadFromFile("Sprites/p3_flame.png")) return;

    IntRect wIdle[4] = { IntRect(45,607,88,82), IntRect(133,607,88,82), IntRect(221,613,90,84), IntRect(313,615,90,82) };
    IntRect wWalk[8] = { IntRect(235,717,90,81), IntRect(335,717,90,82), IntRect(429,717,90,84), IntRect(523,717,90,90), IntRect(619,717,90,88), IntRect(715,717,90,87), IntRect(1002,717,93,82), IntRect(1099,717,92,87) };
    IntRect wJump[6] = { IntRect(37,813,92,95), IntRect(135,813,92,93), IntRect(233,813,90,91), IntRect(333,813,90,89), IntRect(427,813,90,86), IntRect(521,813,89,80) };
    IntRect wShootI[4] = { IntRect(40,919,83,77), IntRect(138,921,85,75), IntRect(236,921,87,75), IntRect(340,921,91,75) };

    mWeaponAnims[W_IDLE].setup("w_idle", wIdle, 4, 6.f, true);
    mWeaponAnims[W_WALK].setup("w_walk", wWalk, 6, 20.f, true);
    mWeaponAnims[W_JUMP].setup("w_jump", wJump, 4, 6.f, false);
    mWeaponAnims[W_SHOOT_IDLE].setup("w_s_idle", wShootI, 6, 20.f, true);

    mWeaponTextureLoaded = true;
}

void PlayerSoldier::setupPlayer3Laser()
{
    if (!mWeaponTexture.loadFromFile("Sprites/p3_laser.png")) return;

    IntRect wIdle[4] = { IntRect(37,1025,88,82), IntRect(125,1025,88,82), IntRect(213,1031,90,84), IntRect(305,1033,92,82) };
    IntRect wWalk[8] = { IntRect(327,1135,90,82), IntRect(421,1135,90,84), IntRect(515,1135,90,90), IntRect(611,1135,90,88), IntRect(707,1135,90,87), IntRect(896,1135,95,86), IntRect(994,1135,93,82), IntRect(1091,1135,96,87) };
    IntRect wJump[6] = { IntRect(29,1231,92,95), IntRect(127,1231,92,93), IntRect(225,1231,90,91), IntRect(325,1231,90,89), IntRect(419,1231,90,86), IntRect(513,1231,90,80) };
    IntRect wShootI[4] = { IntRect(32,1337,83,77), IntRect(130,1339,87,75), IntRect(228,1339,91,75), IntRect(332,1339,93,75) };

    mWeaponAnims[W_IDLE].setup("w_idle", wIdle, 4, 6.f, true);
    mWeaponAnims[W_WALK].setup("w_walk", wWalk, 6, 20.f, true);
    mWeaponAnims[W_JUMP].setup("w_jump", wJump, 4, 6.f, false);
    mWeaponAnims[W_SHOOT_IDLE].setup("w_s_idle", wShootI, 4, 12.f, false);

    mWeaponTextureLoaded = true;
}

void PlayerSoldier::setupPlayer3RL()
{
    if (!mWeaponTexture.loadFromFile("Sprites/p3_rocket.png")) return;

    IntRect wIdle[4] = { IntRect(27,1433,88,82), IntRect(115,1433,88,82), IntRect(203,1439,90,84), IntRect(295,1441,92,82) };
    IntRect wWalk[8] = { IntRect(317,1543,90,82), IntRect(411,1543,90,84), IntRect(505,1543,90,90), IntRect(601,1543,90,88), IntRect(697,1543,90,87), IntRect(791,1543,92,88), IntRect(984,1543,93,82), IntRect(1081,1543,94,87) };
    IntRect wJump[6] = { IntRect(19,1639,92,95), IntRect(117,1639,92,93), IntRect(215,1639,90,91), IntRect(315,1639,90,89), IntRect(409,1639,90,86), IntRect(503,1639,92,80) };
    IntRect wShootI[4] = { IntRect(22,1745,85,77), IntRect(120,1747,87,75), IntRect(218,1747,89,75), IntRect(322,1747,93,75) };

    mWeaponAnims[W_IDLE].setup("w_idle", wIdle, 4, 6.f, true);
    mWeaponAnims[W_WALK].setup("w_walk", wWalk, 6, 20.f, true);
    mWeaponAnims[W_JUMP].setup("w_jump", wJump, 4, 6.f, false);
    mWeaponAnims[W_SHOOT_IDLE].setup("w_s_idle", wShootI, 4, 4.f, false);

    mWeaponTextureLoaded = true;
}

// ----------------------------------------------------------------
//  PLAYER 4  (Fio)
// ----------------------------------------------------------------

void PlayerSoldier::setupPlayer4Pistol()
{
    if (!mWeaponTexture.loadFromFile("Sprites/p4_pistol.png")) return;

    IntRect wShootI[9] = { IntRect(43,18,108,75), IntRect(155,18,108,75), IntRect(265,16,112,77), IntRect(377,14,66,79), IntRect(445,12,66,81), IntRect(511,18,76,75), IntRect(587,18,76,75), IntRect(665,18,74,77), IntRect(743,18,70,77) };

    IntRect wMelee[6] = { IntRect(51,137,52,82), IntRect(107,137,56,80), IntRect(167,139,70,76), IntRect(241,133,102,88), IntRect(615,139,54,76), IntRect(673,139,62,76) };

    mWeaponAnims[W_SHOOT_IDLE].setup("w_s_idle", wShootI, 4, 12.f, false);
    mWeaponAnims[W_MELEE].setup("w_melee", wMelee, 6, 15.f, false);
    mWeaponTextureLoaded = true;
}

void PlayerSoldier::setupPlayer4HMG()
{
    if (!mWeaponTexture.loadFromFile("Sprites/p4_hmg.png")) return;

    IntRect wIdle[4] = { IntRect(47,245,70,94), IntRect(119,243,70,96), IntRect(191,241,70,98), IntRect(263,239,70,100) };
    IntRect wWalk[6] = { IntRect(28,344,76,99), IntRect(104,334,76,99), IntRect(181,348,71,94), IntRect(252,348,71,94), IntRect(327,350,71,94), IntRect(403,350,73,94) };
    IntRect wJump[6] = { IntRect(41,464,73,107), IntRect(114,464,73,107), IntRect(187,464,73,107), IntRect(260,464,73,107), IntRect(336,470,72,92), IntRect(412,470,74,84) };
    IntRect wShootI[4] = { IntRect(53,594,130,79), IntRect(183,596,134,77), IntRect(319,596,130,77), IntRect(455,594,130,79) };

    IntRect wMelee[6] = { IntRect(51,137,52,82), IntRect(107,137,56,80), IntRect(167,139,70,76), IntRect(241,133,102,88), IntRect(615,139,54,76), IntRect(673,139,62,76) };

    //IntRect wMelee[4] = { IntRect(0,0,1,1), IntRect(0,0,1,1), IntRect(0,0,1,1), IntRect(0,0,1,1) };

    mWeaponAnims[W_IDLE].setup("w_idle", wIdle, 4, 6.f, true);
    mWeaponAnims[W_WALK].setup("w_walk", wWalk, 6, 20.f, true);
    mWeaponAnims[W_JUMP].setup("w_jump", wJump, 4, 6.f, false);
    mWeaponAnims[W_SHOOT_IDLE].setup("w_s_idle", wShootI, 6, 24.f, true);

    mWeaponAnims[W_MELEE].setup("w_melee", wMelee, 6, 15.f, false);
    mWeaponTextureLoaded = true;
}

void PlayerSoldier::setupPlayer4Flameshot()
{
    if (!mWeaponTexture.loadFromFile("Sprites/p4_flame.png")) return;

    IntRect wIdle[4] = { IntRect(31,735,84,80), IntRect(117,735,84,80), IntRect(203,735,84,80), IntRect(289,735,84,80) };
    IntRect wWalk[6] = { IntRect(22,842,87,83), IntRect(109,842,87,83), IntRect(197,842,87,83), IntRect(289,842,87,83), IntRect(373,842,85,84), IntRect(986,842,88,84) };
    IntRect wJump[6] = { IntRect(19,926,87,91), IntRect(105,926,87,91), IntRect(192,926,87,91), IntRect(279,926,87,91), IntRect(366,926,86,82), IntRect(456,926,81,74) };
    IntRect wShootI[6] = { IntRect(24,1028,82,74), IntRect(110,1030,82,72), IntRect(196,1030,82,72), IntRect(282,1028,84,74) };

    IntRect wMelee[6] = { IntRect(51,137,52,82), IntRect(107,137,56,80), IntRect(167,139,70,76), IntRect(241,133,102,88), IntRect(615,139,54,76), IntRect(673,139,62,76) };

    //IntRect wMelee[4] = { IntRect(0,0,1,1), IntRect(0,0,1,1), IntRect(0,0,1,1), IntRect(0,0,1,1) };

    mWeaponAnims[W_IDLE].setup("w_idle", wIdle, 4, 6.f, true);
    mWeaponAnims[W_WALK].setup("w_walk", wWalk, 6, 20.f, true);
    mWeaponAnims[W_JUMP].setup("w_jump", wJump, 4, 6.f, false);
    mWeaponAnims[W_SHOOT_IDLE].setup("w_s_idle", wShootI, 6, 20.f, true);

    mWeaponAnims[W_MELEE].setup("w_melee", wMelee, 6, 15.f, false);
    mWeaponTextureLoaded = true;
}

void PlayerSoldier::setupPlayer4Laser()
{
    if (!mWeaponTexture.loadFromFile("Sprites/p4_laser.png")) return;

    IntRect wIdle[4] = { IntRect(27,1129,84,80), IntRect(113,1129,84,80), IntRect(199,1129,84,80), IntRect(285,1129,84,80) };
    IntRect wWalk[6] = { IntRect(18,1236,87,83), IntRect(105,1236,87,83), IntRect(192,1236,87,83), IntRect(279,1236,87,83), IntRect(369,1236,85,84), IntRect(982,1236,88,84) };
    IntRect wJump[6] = { IntRect(15,1320,86,91), IntRect(101,1320,86,91), IntRect(187,1320,86,91), IntRect(274,1320,86,91), IntRect(362,1320,86,82), IntRect(452,1320,81,74) };
    IntRect wShootI[4] = { IntRect(20,1422,82,74), IntRect(106,1424,82,72), IntRect(192,1424,82,72), IntRect(278,1422,84,74) };

    IntRect wMelee[6] = { IntRect(51,137,52,82), IntRect(107,137,56,80), IntRect(167,139,70,76), IntRect(241,133,102,88), IntRect(615,139,54,76), IntRect(673,139,62,76) };

    //IntRect wMelee[4] = { IntRect(0,0,1,1), IntRect(0,0,1,1), IntRect(0,0,1,1), IntRect(0,0,1,1) };

    mWeaponAnims[W_IDLE].setup("w_idle", wIdle, 4, 6.f, true);
    mWeaponAnims[W_WALK].setup("w_walk", wWalk, 6, 20.f, true);
    mWeaponAnims[W_JUMP].setup("w_jump", wJump, 4, 6.f, false);
    mWeaponAnims[W_SHOOT_IDLE].setup("w_s_idle", wShootI, 4, 12.f, false);

    mWeaponAnims[W_MELEE].setup("w_melee", wMelee, 6, 15.f, false);
    mWeaponTextureLoaded = true;
}

void PlayerSoldier::setupPlayer4RL()
{
    if (!mWeaponTexture.loadFromFile("Sprites/p4_rocket.png")) return;

    IntRect wIdle[4] = { IntRect(17,1513,84,80), IntRect(103,1513,84,80), IntRect(189,1513,84,80), IntRect(275,1513,84,80) };
    IntRect wWalk[6] = { IntRect(8,1620,87,83), IntRect(95,1620,87,83), IntRect(182,1620,87,83), IntRect(269,1620,87,83), IntRect(359,1620,85,84), IntRect(972,1620,88,84) };
    IntRect wJump[6] = { IntRect(5,1704,86,91), IntRect(91,1704,86,91), IntRect(177,1704,86,91), IntRect(263,1704,86,91), IntRect(352,1704,86,82), IntRect(442,1704,84,74) };
    IntRect wShootI[4] = { IntRect(10,1806,82,74), IntRect(96,1808,82,72), IntRect(182,1808,82,72), IntRect(268,1806,84,74) };

    IntRect wMelee[6] = { IntRect(51,137,52,82), IntRect(107,137,56,80), IntRect(167,139,70,76), IntRect(241,133,102,88), IntRect(615,139,54,76), IntRect(673,139,62,76) };

    //IntRect wMelee[4] = { IntRect(0,0,1,1), IntRect(0,0,1,1), IntRect(0,0,1,1), IntRect(0,0,1,1) };

    mWeaponAnims[W_IDLE].setup("w_idle", wIdle, 4, 6.f, true);
    mWeaponAnims[W_WALK].setup("w_walk", wWalk, 6, 20.f, true);
    mWeaponAnims[W_JUMP].setup("w_jump", wJump, 4, 6.f, false);
    mWeaponAnims[W_SHOOT_IDLE].setup("w_s_idle", wShootI, 4, 4.f, false);

    mWeaponAnims[W_MELEE].setup("w_melee", wMelee, 6, 15.f, false);
    mWeaponTextureLoaded = true;
}*/

#include "Soldier.h"
#include <SFML/Window/Keyboard.hpp>
#include <cstring>

static bool isTile(char** lvl, int x, int y, int mapWidth, int mapHeight)
{
    if (x < 0 || x >= mapWidth || y < 0 || y >= mapHeight) return false;
    return lvl[y][x] == 'g';
}

// ================================================================
//  SOLDIER BASE
// ================================================================
Soldier::Soldier(float spd, int hp)
    : speed(spd), health(hp),
    mcurrentAnim(IDLE),
    mCurrentWeaponAnim(W_IDLE),
    mWeaponTextureLoaded(false)
{
}

void Soldier::setPosition(float x, float y) { msprite.setPosition(x, y); }
Sprite& Soldier::getSprite() { return msprite; }
Sprite& Soldier::getWeaponSprite() { return mWeaponSprite; }

void Soldier::equipWeapon(Weapon* w)
{
    delete m_weapon;
    m_weapon = w;

    if (!m_weapon)
    {
        mWeaponTextureLoaded = false;
    }
}
//void Soldier::equipWeapon(Weapon* w) { delete m_weapon; m_weapon = w; }

void Soldier::setAnimation(AnimType type)
{
    if (mcurrentAnim == type && !manimations[type].isFinished()) return;
    mcurrentAnim = type;
    manimations[type].reset();
    manimations[type].play();
}

void Soldier::setWeaponAnimation(WeaponAnimType type)
{
    /*if (mCurrentWeaponAnim == type) return;
    mCurrentWeaponAnim = type;
    mWeaponAnims[type].reset();
    mWeaponAnims[type].play();
    */
    if (mCurrentWeaponAnim == type) return;
    //if (mCurrentWeaponAnim == type && !mWeaponAnims[type].isFinished()) return;
    mCurrentWeaponAnim = type;
    mWeaponAnims[type].reset();
    mWeaponAnims[type].play();
}
/*
void Soldier::setWeaponAnimation(WeaponAnimType type)
{
    if (mCurrentWeaponAnim == type && !mWeaponAnims[type].isFinished()) return;
    mCurrentWeaponAnim = type;
    mWeaponAnims[type].reset();
    mWeaponAnims[type].play();
}*/

void Soldier::syncWeaponSprite()
{
    mWeaponSprite.setPosition(msprite.getPosition());
    mWeaponSprite.setScale(facingRight ? mWeaponScale : -mWeaponScale, mWeaponScale);
}

/*void Soldier::syncWeaponSprite()
{
    mWeaponSprite.setPosition(msprite.getPosition());
    mWeaponSprite.setScale(facingRight ? baseScale : -baseScale, baseScale);
}*/

void Soldier::update(float dt, char** lvl, int cell_size, int mapWidth, int mapHeight)
{
    if (!onGround) { velocityY += gravity; if (velocityY > 20.f) velocityY = 20.f; }
    else { velocityY = 0.f; }

    float bx = msprite.getPosition().x + collisionOffsetX;
    float by = msprite.getPosition().y + collisionOffsetY;
    float bw = collisionWidth, bh = collisionHeight;

    // ---- MOVE X ----
    msprite.move(velocityX, 0);
    bx = msprite.getPosition().x + collisionOffsetX;
    int L = (int)(bx / cell_size), R = (int)((bx + bw - 1) / cell_size);
    int T = (int)(by / cell_size), B = (int)((by + bh - 1) / cell_size);
    if (L < 0)L = 0; else if (L >= mapWidth) L = mapWidth - 1;
    if (R < 0)R = 0; else if (R >= mapWidth) R = mapWidth - 1;
    if (T < 0)T = 0; else if (T >= mapHeight)T = mapHeight - 1;
    if (B < 0)B = 0; else if (B >= mapHeight)B = mapHeight - 1;
    if (velocityX > 0) { for (int i = T; i <= B; i++) if (isTile(lvl, R, i, mapWidth, mapHeight)) { msprite.setPosition(R * cell_size - bw - collisionOffsetX, msprite.getPosition().y); velocityX = 0; bx = msprite.getPosition().x + collisionOffsetX; break; } }
    else if (velocityX < 0) { for (int i = T; i <= B; i++) if (isTile(lvl, L, i, mapWidth, mapHeight)) { msprite.setPosition((L + 1) * cell_size - collisionOffsetX, msprite.getPosition().y); velocityX = 0; bx = msprite.getPosition().x + collisionOffsetX; break; } }

    // ---- MOVE Y ----
    msprite.move(0, velocityY);
    by = msprite.getPosition().y + collisionOffsetY;
    L = (int)(bx / cell_size); R = (int)((bx + bw - 1) / cell_size);
    T = (int)(by / cell_size); B = (int)((by + bh - 1) / cell_size);
    if (L < 0)L = 0; else if (L >= mapWidth) L = mapWidth - 1;
    if (R < 0)R = 0; else if (R >= mapWidth) R = mapWidth - 1;
    if (T < 0)T = 0; else if (T >= mapHeight)T = mapHeight - 1;
    if (B < 0)B = 0; else if (B >= mapHeight)B = mapHeight - 1;
    onGround = false;
    if (velocityY >= 0) { for (int j = L; j <= R; j++) if (isTile(lvl, j, B, mapWidth, mapHeight)) { msprite.setPosition(msprite.getPosition().x, B * cell_size - bh - collisionOffsetY); velocityY = 0; onGround = true; isJumping = false; by = msprite.getPosition().y + collisionOffsetY; break; } }
    else { for (int j = L; j <= R; j++) if (isTile(lvl, j, T, mapWidth, mapHeight)) { msprite.setPosition(msprite.getPosition().x, (T + 1) * cell_size - collisionOffsetY); velocityY = 0; isJumping = false; by = msprite.getPosition().y + collisionOffsetY; break; } }

    // ---- CLAMP ----
    bx = msprite.getPosition().x + collisionOffsetX;
    by = msprite.getPosition().y + collisionOffsetY;
    float cx = bx, cy = by;
    if (cx < 0.f) cx = 0.f; else if (cx > mapWidth * cell_size - bw) cx = mapWidth * cell_size - bw;
    if (cy < 0.f) cy = 0.f; else if (cy > mapHeight * cell_size - bh) { cy = mapHeight * cell_size - bh; velocityY = 0; onGround = true; isJumping = false; }
    msprite.setPosition(cx - collisionOffsetX, cy - collisionOffsetY);

    // ---- BODY ANIMATION ----
    bool isRunning = (velocityX != 0.f);
    if (isJumping)      setAnimation(isRunning ? JUMP_RUN : JUMP_IDLE);
    else if (isRunning) setAnimation(WALK);
    else                setAnimation(IDLE);
    manimations[mcurrentAnim].update(dt);
    msprite.setTextureRect(manimations[mcurrentAnim].getCurrentFrame());
    msprite.setScale(facingRight ? baseScale : -baseScale, baseScale);

    // ---- WEAPON OVERLAY ANIMATION (mirrors body anim — same state machine) ----
    // State selection lives here (not in handleInput) so weapon and body
    // animations are always driven by the exact same physics variables.
    if (mWeaponTextureLoaded)
    {
        bool airborne = isJumping && !onGround;
        if (airborne)
        {
            setWeaponAnimation(W_JUMP);
        }
        else if (isRunning)
        {
            setWeaponAnimation(W_WALK);
        }
        else if (mIsShooting)
        {
            setWeaponAnimation(W_SHOOT_IDLE);
        }
        else
        {
            setWeaponAnimation(W_IDLE);
        }

        /* if (mIsShooting)
        {
            if (airborne)       setWeaponAnimation(W_SHOOT_JUMP);
            else if (isRunning) setWeaponAnimation(W_SHOOT_WALK);
            else                setWeaponAnimation(W_SHOOT_IDLE);
        }
        else
        {
            if (airborne)       setWeaponAnimation(W_JUMP);
            else if (isRunning) setWeaponAnimation(W_WALK);
            else                setWeaponAnimation(W_IDLE);
        }*/
        //mWeaponAnims[mCurrentWeaponAnim].update(dt);


        if (mWeaponTextureLoaded && mCurrentWeaponAnim != W_NONE)
        {
            mWeaponAnims[mCurrentWeaponAnim].update(dt);
            mWeaponSprite.setTextureRect(
                mWeaponAnims[mCurrentWeaponAnim].getCurrentFrame()
            );
        }
        bool weaponEquipped = m_weapon != nullptr;

        if (!weaponEquipped)
        {
            // only animate body when no weapon
            manimations[mcurrentAnim].update(dt);
            msprite.setTextureRect(manimations[mcurrentAnim].getCurrentFrame());
        }
        /*if (mWeaponTextureLoaded && mCurrentWeaponAnim != W_NONE)
        {
            mWeaponAnims[mCurrentWeaponAnim].update(dt);
            mWeaponSprite.setTextureRect(mWeaponAnims[mCurrentWeaponAnim].getCurrentFrame());
        }*/

        //mWeaponSprite.setTextureRect(mWeaponAnims[mCurrentWeaponAnim].getCurrentFrame());
        syncWeaponSprite();
    }
    else
    {
        manimations[mcurrentAnim].update(dt);
        msprite.setTextureRect(manimations[mcurrentAnim].getCurrentFrame());
    }

    if (m_weapon) m_weapon->update(dt);
}

// ================================================================
//  PLAYER SOLDIER
// ================================================================
PlayerSoldier::PlayerSoldier(int index) : Soldier(3.f, 100), mplayerIndex(index)
{
    loadPlayerData();
    setAnimation(IDLE);
    equipWeapon(new Pistol());
}

void PlayerSoldier::update(float dt, char** lvl, int cell_size, int mapWidth, int mapHeight)
{
    handleInput(dt);
    Soldier::update(dt, lvl, cell_size, mapWidth, mapHeight);
}

// ================================================================
//  LOAD PROJECTILE TEXTURES
//  Called once in loadPlayerData() so RocketLauncher / HandGrenade
//  always have a valid Texture& when constructed via equipWeapon().
// ================================================================
void PlayerSoldier::loadProjectileTextures()
{
    // These sheets are the same for all players — the projectile itself
    // looks the same regardless of who fired it.
    mRocketTex.loadFromFile("Sprites/rocket.png");
    mGrenadeTex.loadFromFile("Sprites/grenade.png");
}

// ================================================================
//  INPUT
// ================================================================
void PlayerSoldier::handleInput(float dt)
{
    velocityX = 0;
    mIsShooting = false;

    bool fireKey = false;
    bool cycleKey = false;

    if (mplayerIndex == 0)
    {
        if (Keyboard::isKeyPressed(Keyboard::Right)) { velocityX = speed; facingRight = true; }
        if (Keyboard::isKeyPressed(Keyboard::Left)) { velocityX = -speed; facingRight = false; }
        if (Keyboard::isKeyPressed(Keyboard::Up) && onGround)
        {
            velocityY = jumpStrength; onGround = false; isJumping = true;
        }

        fireKey = Keyboard::isKeyPressed(Keyboard::Z);   // Z = shoot / attack
        cycleKey = Keyboard::isKeyPressed(Keyboard::X);   // X = cycle weapon
    }
    else if (mplayerIndex == 1)
    {
        if (Keyboard::isKeyPressed(Keyboard::D)) { velocityX = speed; facingRight = true; }
        if (Keyboard::isKeyPressed(Keyboard::A)) { velocityX = -speed; facingRight = false; }
        if (Keyboard::isKeyPressed(Keyboard::W) && onGround)
        {
            velocityY = jumpStrength; onGround = false; isJumping = true;
        }

        if (onGround) coyoteTime = coyoteMax;
        else          coyoteTime -= dt;

        fireKey = Keyboard::isKeyPressed(Keyboard::LControl); // LCtrl = shoot / attack
        cycleKey = Keyboard::isKeyPressed(Keyboard::Q);        // Q     = cycle weapon
    }
    // Players 2 & 3 input not yet assigned

    // ---- CYCLE WEAPON (one step per press, not held) ----
    if (cycleKey && !mPrevCycleKey)
    {
        mWeaponSlot = (mWeaponSlot + 1) % 6;  // 0=Pistol 1=HMG 2=Flame 3=Laser 4=Rocket
        switch (mWeaponSlot)
        {
        case 0: equipWeapon(new Pistol());                   break;
        case 1: equipWeapon(new HeavyMachineGun());          break;
        case 2: equipWeapon(new FlameShot());                break;
        case 3: equipWeapon(new LaserGun());                 break;
        case 4: equipWeapon(new RocketLauncher(mRocketTex)); break;
        case 5: {

            m_weapon = nullptr;
            mWeaponTextureLoaded = false;

            mCurrentWeaponAnim = W_NONE;
            mWeaponSprite = sf::Sprite();

        }

        }
    }
    mPrevCycleKey = cycleKey;

    // ---- FIRE / ATTACK ----
    // mIsShooting is read by Soldier::update() to select the correct weapon anim.
    if (fireKey && m_weapon)
    {
        mIsShooting = true;
        float ox = msprite.getPosition().x + (facingRight ? collisionWidth : 0.f);
        float oy = msprite.getPosition().y + collisionHeight * 0.4f;
        m_weapon->fire(ox, oy, facingRight, 0.f);
    }
    // Weapon animation selection is intentionally NOT here.
    // Soldier::update() handles it in one place alongside the body animation.
}

// ================================================================
//  LOAD PLAYER DATA  (body texture + base animations)
// ================================================================
void PlayerSoldier::loadPlayerData()
{
    if (mplayerIndex == 0)
    {
        mtexture.loadFromFile("Sprites/Group 10.png");
        setupPlayer4Animations();
        baseScale = 2.5f;
        mWeaponScale = 1.5f;
    }
    else if (mplayerIndex == 1)
    {
        mtexture.loadFromFile("Sprites/Group 8.png");
        setupPlayer2Animations();
        baseScale = 1.65f;
        mWeaponScale = 1.65f;
    }
    else if (mplayerIndex == 2)
    {
        mtexture.loadFromFile("Sprites/Group 5.png");
        setupPlayer1Animations();
        baseScale = 1.f;
        mWeaponScale = 1.f;
    }
    else if (mplayerIndex == 3)
    {
        mtexture.loadFromFile("Sprites/Group 9.png");
        setupPlayer3Animations();
        baseScale = 1.25f;
        mWeaponScale = 1.25f;
    }
    msprite.setTexture(mtexture);
    loadProjectileTextures();   // always load so weapon swap never has a dangling ref
}

// ================================================================
//  EQUIP WEAPON  — swap weapon + reload matching overlay sheet
// ================================================================
void PlayerSoldier::equipWeapon(Weapon* w)
{
    Soldier::equipWeapon(w);

    if (!m_weapon)
    {
        mWeaponTextureLoaded = false;
        mCurrentWeaponAnim = static_cast<WeaponAnimType>(-1);
        return;
    }
    /*Soldier::equipWeapon(w);
    if (!m_weapon) return;*/

    const char* name = m_weapon->getName();
    mWeaponTextureLoaded = false;

    if (mplayerIndex == 0)
    {
        if (strcmp(name, "pistol") == 0) setupPlayer1Pistol();
        else if (strcmp(name, "hmg") == 0) setupPlayer1HMG();
        else if (strcmp(name, "flameshot") == 0) setupPlayer1Flameshot();
        else if (strcmp(name, "laserbeam") == 0) setupPlayer1Laser();
        else if (strcmp(name, "rocket") == 0) setupPlayer1RL();
    }
    else if (mplayerIndex == 1)
    {
        if (strcmp(name, "pistol") == 0) setupPlayer2Pistol();
        else if (strcmp(name, "hmg") == 0) setupPlayer2HMG();
        else if (strcmp(name, "flameshot") == 0) setupPlayer2Flameshot();
        else if (strcmp(name, "laserbeam") == 0) setupPlayer2Laser();
        else if (strcmp(name, "rocket") == 0) setupPlayer2RL();
    }
    else if (mplayerIndex == 2)
    {
        if (strcmp(name, "pistol") == 0) setupPlayer3Pistol();
        else if (strcmp(name, "hmg") == 0) setupPlayer3HMG();
        else if (strcmp(name, "flameshot") == 0) setupPlayer3Flameshot();
        else if (strcmp(name, "laserbeam") == 0) setupPlayer3Laser();
        else if (strcmp(name, "rocket") == 0) setupPlayer3RL();
    }
    else if (mplayerIndex == 3)
    {
        if (strcmp(name, "pistol") == 0) setupPlayer4Pistol();
        else if (strcmp(name, "hmg") == 0) setupPlayer4HMG();
        else if (strcmp(name, "flameshot") == 0) setupPlayer4Flameshot();
        else if (strcmp(name, "laserbeam") == 0) setupPlayer4Laser();
        else if (strcmp(name, "rocket") == 0) setupPlayer4RL();
    }

    /*if (mWeaponTextureLoaded)
    {
        mWeaponSprite.setTexture(mWeaponTexture);
        setWeaponAnimation(W_IDLE);
    }*/
    if (mWeaponTextureLoaded)
    {
        mWeaponSprite.setTexture(mWeaponTexture, true);  // true = reset rect to full texture

        // Force-reset unconditionally — bypass the "already playing" guard in
        // setWeaponAnimation() so the previous weapon's stale frames never show.
        /*mCurrentWeaponAnim = W_IDLE;
        mWeaponAnims[W_IDLE].reset();
        mWeaponAnims[W_IDLE].play();
        mWeaponSprite.setTextureRect(mWeaponAnims[W_IDLE].getCurrentFrame());*/
        mCurrentWeaponAnim = W_NONE; // invalidate so guard always misses
        setWeaponAnimation(W_IDLE);
        mWeaponSprite.setTextureRect(mWeaponAnims[W_IDLE].getCurrentFrame());

    }
}

// ================================================================
//  BODY ANIMATIONS  (unchanged from original)
// ================================================================
void PlayerSoldier::setupPlayer1Animations()
{
    IntRect idle[4] = { IntRect(90,4,83,101), IntRect(174,4,84,101), IntRect(264,4,81,101), IntRect(346,4,81,101) };
    IntRect walk[12] = { IntRect(88,126,79,90), IntRect(168,126,78,92), IntRect(246,126,78,92), IntRect(325,128,77,99), IntRect(402,128,77,99), IntRect(485,128,77,99), IntRect(567,130,77,97), IntRect(647,130,77,99), IntRect(727,130,79,97), IntRect(811,130,78,97), IntRect(889,130,78,97), IntRect(971,130,74,97) };
    IntRect jumpIdle[6] = { IntRect(88,238,66,106), IntRect(157,242,70,102), IntRect(232,244,71,98), IntRect(308,242,69,98), IntRect(384,242,69,92), IntRect(459,244,67,84) };
    IntRect jumpRun[6] = { IntRect(86,362,73,97), IntRect(159,362,73,97), IntRect(239,362,79,88), IntRect(318,360,73,90), IntRect(391,360,73,90), IntRect(464,360,73,90) };
    IntRect swimIdle[6] = { IntRect(22,803,136,77), IntRect(165,801,139,75), IntRect(312,812,137,67), IntRect(457,812,140,64), IntRect(598,810,141,64), IntRect(758,808,140,64) };
    IntRect swim[10] = { IntRect(34,916,132,74), IntRect(176,911,130,67), IntRect(314,916,133,61), IntRect(456,906,139,64), IntRect(600,908,138,61), IntRect(756,905,140,60), IntRect(898,900,138,66), IntRect(1054,895,136,66), IntRect(1196,889,130,77), IntRect(1337,890,131,76) };
    IntRect die[21] = { IntRect(78,687,72,93), IntRect(155,694,79,86), IntRect(239,694,83,86), IntRect(328,692,68,88), IntRect(399,685,69,95), IntRect(471,682,74,98), IntRect(548,680,71,100), IntRect(622,680,68,100), IntRect(696,680,61,100), IntRect(761,687,66,93), IntRect(832,690,67,90), IntRect(902,694,71,86), IntRect(976,706,78,74), IntRect(1057,704,74,76), IntRect(1136,706,69,74), IntRect(1208,709,66,71), IntRect(1277,713,69,67), IntRect(1351,718,69,62), IntRect(1426,728,71,52), IntRect(1500,742,88,38), IntRect(1588,740,84,40) };
    manimations[IDLE].setup("idle", idle, 4, 6.f, true);
    manimations[WALK].setup("walk", walk, 12, 20.f, true);
    manimations[SWIM_IDLE].setup("swim_idle", swimIdle, 6, 8.f, false);
    manimations[SWIM].setup("swim", swim, 10, 8.f, false);
    manimations[JUMP_IDLE].setup("jump_idle", jumpIdle, 6, 6.f, false);
    manimations[JUMP_RUN].setup("jump_run", jumpRun, 6, 6.f, false);
    manimations[DIE].setup("die", die, 21, 5.f, false);
}

void PlayerSoldier::setupPlayer2Animations()
{
    IntRect idle[4] = { IntRect(8,10,48,60), IntRect(61,10,48,61), IntRect(115,10,49,61), IntRect(170,10,51,61) };
    IntRect walk[6] = { IntRect(2,82,48,67), IntRect(58,82,49,67), IntRect(115,82,48,67), IntRect(170,82,50,67), IntRect(223,82,52,67), IntRect(281,82,50,67) };
    IntRect jumpIdle[6] = { IntRect(3,169,47,70), IntRect(53,170,48,69), IntRect(107,172,48,67), IntRect(158,173,48,66), IntRect(213,173,47,63), IntRect(264,173,47,62) };
    IntRect jumpRun[6] = { IntRect(329,170,48,66), IntRect(382,170,48,63), IntRect(436,170,55,60), IntRect(496,169,49,58), IntRect(549,169,49,58), IntRect(607,169,43,60) };
    IntRect die[19] = { IntRect(4,253,43,60), IntRect(58,256,49,55), IntRect(113,257,47,56), IntRect(169,256,46,57), IntRect(226,254,48,56), IntRect(283,260,42,50), IntRect(332,260,44,50), IntRect(389,263,47,48), IntRect(446,268,53,43), IntRect(510,266,56,42), IntRect(572,271,50,34), IntRect(631,265,48,40), IntRect(687,265,40,42), IntRect(736,260,37,48), IntRect(778,265,39,45), IntRect(826,268,43,45), IntRect(874,278,48,35), IntRect(927,284,53,29), IntRect(985,289,57,24) };
    manimations[IDLE].setup("idle", idle, 4, 6.f, true);
    manimations[WALK].setup("walk", walk, 6, 15.f, true);
    manimations[JUMP_IDLE].setup("jump_idle", jumpIdle, 6, 6.f, false);
    manimations[JUMP_RUN].setup("jump_run", jumpRun, 6, 6.f, false);
    manimations[DIE].setup("die", die, 19, 5.f, false);
}

void PlayerSoldier::setupPlayer3Animations()
{
    IntRect idle[4] = { IntRect(0,1,63,75), IntRect(68,1,61,75), IntRect(136,1,63,75), IntRect(206,1,66,75) };
    IntRect walk[6] = { IntRect(0,82,67,85), IntRect(67,82,67,85), IntRect(135,82,63,85), IntRect(195,82,63,67), IntRect(262,82,64,84), IntRect(326,82,67,83) };
    IntRect jumpIdle[6] = { IntRect(7,170,60,99), IntRect(75,172,60,98), IntRect(145,175,60,91), IntRect(213,175,60,87), IntRect(281,175,60,77), IntRect(349,175,60,78) };
    IntRect jumpRun[6] = { IntRect(1,272,61,94), IntRect(71,272,63,87), IntRect(143,272,73,78), IntRect(237,272,66,73), IntRect(310,272,68,75), IntRect(393,272,57,81) };
    IntRect die[19] = { IntRect(4,253,43,60), IntRect(58,256,49,55), IntRect(113,257,47,56), IntRect(169,256,46,57), IntRect(226,254,48,56), IntRect(283,260,42,50), IntRect(332,260,44,50), IntRect(389,263,47,48), IntRect(446,268,53,43), IntRect(510,266,56,42), IntRect(572,271,50,34), IntRect(631,265,48,40), IntRect(687,265,40,42), IntRect(736,260,37,48), IntRect(778,265,39,45), IntRect(826,268,43,45), IntRect(874,278,48,35), IntRect(927,284,53,29), IntRect(985,289,57,24) };
    manimations[IDLE].setup("idle", idle, 4, 6.f, true);
    manimations[WALK].setup("walk", walk, 6, 15.f, true);
    manimations[JUMP_IDLE].setup("jump_idle", jumpIdle, 6, 6.f, false);
    manimations[JUMP_RUN].setup("jump_run", jumpRun, 6, 6.f, false);
    manimations[DIE].setup("die", die, 19, 5.f, false);
}

void PlayerSoldier::setupPlayer4Animations()
{
    IntRect idle[4] = { IntRect(4,0,31,37), IntRect(37,0,31,38), IntRect(70,0,31,38), IntRect(103,0,31,38) };
    IntRect walk[14] = { IntRect(0,54,32,39), IntRect(32,54,32,39), IntRect(65,56,31,35), IntRect(96,55,32,37), IntRect(132,54,28,38), IntRect(164,54,29,39), IntRect(196,55,30,38), IntRect(227,56,33,39), IntRect(260,56,33,39), IntRect(293,56,33,39), IntRect(328,55,29,38), IntRect(359,55,30,38), IntRect(164,54,29,39), IntRect(196,55,30,38) };
    IntRect jumpIdle[6] = { IntRect(6,105,29,45), IntRect(40,107,30,43), IntRect(75,109,30,40), IntRect(110,109,30,39), IntRect(145,109,29,37), IntRect(178,109,29,35) };
    IntRect jumpRun[6] = { IntRect(9,163,29,43), IntRect(38,163,29,43), IntRect(71,163,34,32), IntRect(106,163,30,35), IntRect(138,163,30,36), IntRect(168,163,26,37) };
    IntRect die[19] = { IntRect(4,253,43,60), IntRect(58,256,49,55), IntRect(113,257,47,56), IntRect(169,256,46,57), IntRect(226,254,48,56), IntRect(283,260,42,50), IntRect(332,260,44,50), IntRect(389,263,47,48), IntRect(446,268,53,43), IntRect(510,266,56,42), IntRect(572,271,50,34), IntRect(631,265,48,40), IntRect(687,265,40,42), IntRect(736,260,37,48), IntRect(778,265,39,45), IntRect(826,268,43,45), IntRect(874,278,48,35), IntRect(927,284,53,29), IntRect(985,289,57,24) };
    manimations[IDLE].setup("idle", idle, 4, 6.f, true);
    manimations[WALK].setup("walk", walk, 14, 20.f, true);
    manimations[JUMP_IDLE].setup("jump_idle", jumpIdle, 6, 6.f, false);
    manimations[JUMP_RUN].setup("jump_run", jumpRun, 6, 6.f, false);
    manimations[DIE].setup("die", die, 19, 5.f, false);
}

// ================================================================
//  WEAPON OVERLAY SETUPS  (all IntRect values from document 8 — unchanged)
// ================================================================

void PlayerSoldier::setupPlayer1Pistol()
{
    if (!mWeaponTexture.loadFromFile("Sprites/Group 24.png")) return;
    IntRect wShootI[10] = { IntRect(43,13,108,78), IntRect(155,13,108,78), IntRect(267,13,110,78), IntRect(381,13,80,82), IntRect(465,9,80,86), IntRect(547,9,82,84), IntRect(631,13,82,80), IntRect(715,13,76,80), IntRect(795,13,72,80), IntRect(869,11,72,82) };
    IntRect wMelee[6] = { IntRect(31,135,54,84), IntRect(89,135,52,84), IntRect(145,135,58,86), IntRect(207,137,88,82), IntRect(299,128,102,89), IntRect(403,128,104,89) };
    mWeaponAnims[W_SHOOT_IDLE].setup("w_s_idle", wShootI, 4, 12.f, false);
    mWeaponAnims[W_MELEE].setup("w_melee", wMelee, 6, 15.f, false);
    mWeaponTextureLoaded = true;
}
void PlayerSoldier::setupPlayer1HMG()
{
    if (!mWeaponTexture.loadFromFile("Sprites/Group 24.png")) return;
    IntRect wIdle[4] = { IntRect(45,247,78,100), IntRect(127,249,80,98), IntRect(211,251,82,96), IntRect(297,253,84,94) };
    IntRect wWalk[6] = { IntRect(32,370,86,104), IntRect(121,368,83,104), IntRect(208,368,80,105), IntRect(292,368,82,107), IntRect(374,372,88,102), IntRect(466,374,88,99) };
    IntRect wJump[5] = { IntRect(28,504,84,112), IntRect(202,502,80,112), IntRect(286,502,82,110), IntRect(372,506,84,100), IntRect(460,508,88,95) };
    IntRect wShootI[4] = { IntRect(19,646,135,79), IntRect(157,646,137,81), IntRect(299,646,133,81), IntRect(437,646,137,83) };
    IntRect wMelee[6] = { IntRect(31,135,54,84), IntRect(89,135,52,84), IntRect(145,135,58,86), IntRect(207,137,88,82), IntRect(299,128,102,89), IntRect(403,128,104,89) };
    mWeaponAnims[W_IDLE].setup("w_idle", wIdle, 4, 6.f, true);
    mWeaponAnims[W_WALK].setup("w_walk", wWalk, 6, 20.f, true);
    mWeaponAnims[W_JUMP].setup("w_jump", wJump, 4, 6.f, false);
    mWeaponAnims[W_SHOOT_IDLE].setup("w_s_idle", wShootI, 6, 24.f, true);
    mWeaponAnims[W_MELEE].setup("w_melee", wMelee, 6, 15.f, false);
    mWeaponTextureLoaded = true;
}
void PlayerSoldier::setupPlayer1Flameshot()
{
    if (!mWeaponTexture.loadFromFile("Sprites/Group 24.png")) return;
    IntRect wIdle[4] = { IntRect(15,767,83,84), IntRect(99,767,83,86), IntRect(185,767,83,86), IntRect(271,767,85,84) };
    IntRect wWalk[6] = { IntRect(19,877,90,91), IntRect(113,875,85,91), IntRect(201,875,83,92), IntRect(286,887,88,92), IntRect(374,887,88,92), IntRect(467,877,90,90) };
    IntRect wJump[6] = { IntRect(19,985,90,99), IntRect(113,983,87,99), IntRect(201,983,83,101), IntRect(287,985,84,95), IntRect(375,985,88,89), IntRect(467,985,91,84) };
    IntRect wShootI[4] = { IntRect(19,1096,85,79), IntRect(105,1098,83,77), IntRect(189,1096,81,79), IntRect(271,1096,83,79) };
    IntRect wMelee[6] = { IntRect(31,135,54,84), IntRect(89,135,52,84), IntRect(145,135,58,86), IntRect(207,137,88,82), IntRect(299,128,102,89), IntRect(403,128,104,89) };
    mWeaponAnims[W_IDLE].setup("w_idle", wIdle, 4, 6.f, true);
    mWeaponAnims[W_WALK].setup("w_walk", wWalk, 6, 20.f, true);
    mWeaponAnims[W_JUMP].setup("w_jump", wJump, 6, 6.f, false);
    mWeaponAnims[W_SHOOT_IDLE].setup("w_s_idle", wShootI, 4, 20.f, true);
    mWeaponAnims[W_MELEE].setup("w_melee", wMelee, 6, 15.f, false);
    mWeaponTextureLoaded = true;
}
void PlayerSoldier::setupPlayer1Laser()
{
    if (!mWeaponTexture.loadFromFile("Sprites/Group 24.png")) return;
    IntRect wIdle[4] = { IntRect(21,1201,83,86), IntRect(104,1201,83,86), IntRect(191,1201,84,86), IntRect(277,1201,82,84) };
    IntRect wWalk[6] = { IntRect(25,1311,90,91), IntRect(119,1309,84,91), IntRect(207,1309,82,92), IntRect(292,1311,88,92), IntRect(380,1311,88,92), IntRect(473,1311,90,90) };
    IntRect wJump[6] = { IntRect(25,1419,90,99), IntRect(119,1417,84,99), IntRect(207,1417,82,101), IntRect(293,1419,84,95), IntRect(381,1419,88,89), IntRect(473,1419,90,84) };
    IntRect wShootI[4] = { IntRect(25,1530,84,80), IntRect(111,1532,80,78), IntRect(195,1530,78,80), IntRect(277,1530,85,80) };
    IntRect wMelee[6] = { IntRect(31,135,54,84), IntRect(89,135,52,84), IntRect(145,135,58,86), IntRect(207,137,88,82), IntRect(299,128,102,89), IntRect(403,128,104,89) };
    mWeaponAnims[W_IDLE].setup("w_idle", wIdle, 4, 6.f, true);
    mWeaponAnims[W_WALK].setup("w_walk", wWalk, 6, 20.f, true);
    mWeaponAnims[W_JUMP].setup("w_jump", wJump, 4, 6.f, false);
    mWeaponAnims[W_SHOOT_IDLE].setup("w_s_idle", wShootI, 4, 12.f, false);
    mWeaponAnims[W_MELEE].setup("w_melee", wMelee, 6, 15.f, false);
    mWeaponTextureLoaded = true;
}
void PlayerSoldier::setupPlayer1RL()
{
    if (!mWeaponTexture.loadFromFile("Sprites/Group 24.png")) return;
    IntRect wIdle[4] = { IntRect(24,1641,83,84), IntRect(110,1641,83,86), IntRect(197,1641,82,86), IntRect(282,1641,84,84) };
    IntRect wWalk[6] = { IntRect(31,1751,90,91), IntRect(125,1749,84,91), IntRect(212,1749,83,92), IntRect(296,1751,89,92), IntRect(385,1751,89,92), IntRect(479,1751,90,90) };
    IntRect wJump[6] = { IntRect(31,1859,90,99), IntRect(125,1857,84,99), IntRect(213,1857,82,101), IntRect(296,1859,87,95), IntRect(387,1859,88,89), IntRect(479,1859,92,84) };
    IntRect wShootI[4] = { IntRect(30,1970,83,80), IntRect(116,1972,81,78), IntRect(200,1970,79,80), IntRect(283,1970,85,80) };
    IntRect wMelee[6] = { IntRect(31,135,54,84), IntRect(89,135,52,84), IntRect(145,135,58,86), IntRect(207,137,88,82), IntRect(299,128,102,89), IntRect(403,128,104,89) };
    mWeaponAnims[W_IDLE].setup("w_idle", wIdle, 4, 6.f, true);
    mWeaponAnims[W_WALK].setup("w_walk", wWalk, 6, 20.f, true);
    mWeaponAnims[W_JUMP].setup("w_jump", wJump, 4, 6.f, false);
    mWeaponAnims[W_SHOOT_IDLE].setup("w_s_idle", wShootI, 4, 4.f, false);
    mWeaponAnims[W_MELEE].setup("w_melee", wMelee, 6, 15.f, false);
    mWeaponTextureLoaded = true;
}

void PlayerSoldier::setupPlayer2Pistol()
{
    if (!mWeaponTexture.loadFromFile("Sprites/Group 27.png")) return;
    IntRect wShootI[9] = { IntRect(35,9,106,81), IntRect(147,11,108,79), IntRect(259,9,110,81), IntRect(373,7,80,83), IntRect(457,3,80,87), IntRect(541,3,80,85), IntRect(624,7,81,79), IntRect(707,7,74,81), IntRect(861,5,70,81) };
    IntRect wMelee[6] = { IntRect(29,115,54,83), IntRect(95,115,52,85), IntRect(157,115,58,85), IntRect(227,115,88,83), IntRect(325,107,102,91), IntRect(437,107,102,91) };
    mWeaponAnims[W_SHOOT_IDLE].setup("w_s_idle", wShootI, 4, 12.f, false);
    mWeaponAnims[W_MELEE].setup("w_melee", wMelee, 6, 15.f, false);
    mWeaponTextureLoaded = true;
}
void PlayerSoldier::setupPlayer2HMG()
{
    if (!mWeaponTexture.loadFromFile("Sprites/Group 27.png")) return;
    IntRect wIdle[4] = { IntRect(37,225,78,101), IntRect(129,227,80,99), IntRect(223,229,82,97), IntRect(313,231,84,93) };
    IntRect wWalk[9] = { IntRect(43,363,86,104), IntRect(43,363,86,104), IntRect(233,361,88,99), IntRect(337,361,82,106), IntRect(431,367,84,101), IntRect(527,369,88,99), IntRect(627,371,84,99), IntRect(716,367,89,103), IntRect(921,365,90,105) };
    IntRect wJump[6] = { IntRect(37,479,84,113), IntRect(135,477,82,113), IntRect(233,477,80,111), IntRect(329,477,82,109), IntRect(423,483,84,99), IntRect(519,485,88,93) };
    IntRect wShootI[4] = { IntRect(40,628,133,80), IntRect(180,628,135,82), IntRect(322,628,133,82), IntRect(466,630,137,80) };
    IntRect wMelee[6] = { IntRect(29,115,54,83), IntRect(95,115,52,85), IntRect(157,115,58,85), IntRect(227,115,88,83), IntRect(325,107,102,91), IntRect(437,107,102,91) };
    mWeaponAnims[W_IDLE].setup("w_idle", wIdle, 4, 6.f, true);
    mWeaponAnims[W_WALK].setup("w_walk", wWalk, 6, 20.f, true);
    mWeaponAnims[W_JUMP].setup("w_jump", wJump, 4, 6.f, false);
    mWeaponAnims[W_SHOOT_IDLE].setup("w_s_idle", wShootI, 6, 24.f, true);
    mWeaponAnims[W_MELEE].setup("w_melee", wMelee, 6, 15.f, false);
    mWeaponTextureLoaded = true;
}
void PlayerSoldier::setupPlayer2Flameshot()
{
    if (!mWeaponTexture.loadFromFile("Sprites/Group 27.png")) return;
    IntRect wIdle[4] = { IntRect(26,741,83,79), IntRect(114,741,83,79), IntRect(206,741,83,83), IntRect(299,741,82,83) };
    IntRect wWalk[9] = { IntRect(31,845,92,86), IntRect(125,843,90,88), IntRect(217,841,88,83), IntRect(311,843,84,88), IntRect(403,843,88,89), IntRect(501,845,90,87), IntRect(899,845,90,89), IntRect(997,845,92,88), IntRect(1095,845,92,88) };
    IntRect wJump[6] = { IntRect(23,972,89,96), IntRect(121,970,84,96), IntRect(213,968,82,98), IntRect(301,970,83,96), IntRect(393,970,87,88), IntRect(491,972,89,84) };
    IntRect wShootI[4] = { IntRect(28,1071,83,80), IntRect(120,1073,79,78), IntRect(210,1071,77,80), IntRect(298,1071,81,80) };
    IntRect wMelee[6] = { IntRect(29,115,54,83), IntRect(95,115,52,85), IntRect(157,115,58,85), IntRect(227,115,88,83), IntRect(325,107,102,91), IntRect(437,107,102,91) };
    mWeaponAnims[W_IDLE].setup("w_idle", wIdle, 4, 6.f, true);
    mWeaponAnims[W_WALK].setup("w_walk", wWalk, 6, 20.f, true);
    mWeaponAnims[W_JUMP].setup("w_jump", wJump, 4, 6.f, false);
    mWeaponAnims[W_SHOOT_IDLE].setup("w_s_idle", wShootI, 6, 20.f, true);
    mWeaponAnims[W_MELEE].setup("w_melee", wMelee, 6, 15.f, false);
    mWeaponTextureLoaded = true;
}
void PlayerSoldier::setupPlayer2Laser()
{
    if (!mWeaponTexture.loadFromFile("Sprites/Group 27.png")) return;
    IntRect wIdle[4] = { IntRect(8,1179,83,79), IntRect(96,1179,85,79), IntRect(188,1179,83,83), IntRect(281,1179,84,83) };
    IntRect wWalk[9] = { IntRect(13,1283,92,86), IntRect(107,1281,90,88), IntRect(199,1279,88,83), IntRect(293,1281,84,88), IntRect(385,1281,88,89), IntRect(483,1283,90,87), IntRect(579,1283,90,89), IntRect(979,1283,92,88), IntRect(1077,1283,92,88) };
    IntRect wJump[6] = { IntRect(5,1410,89,96), IntRect(103,1408,84,96), IntRect(195,1406,81,98), IntRect(283,1408,83,96), IntRect(375,1408,87,88), IntRect(473,1410,89,84) };
    IntRect wShootI[4] = { IntRect(10,1509,83,80), IntRect(102,1511,79,78), IntRect(192,1509,79,80), IntRect(280,1509,81,80) };
    IntRect wMelee[6] = { IntRect(29,115,54,83), IntRect(95,115,52,85), IntRect(157,115,58,85), IntRect(227,115,88,83), IntRect(325,107,102,91), IntRect(437,107,102,91) };
    mWeaponAnims[W_IDLE].setup("w_idle", wIdle, 4, 6.f, true);
    mWeaponAnims[W_WALK].setup("w_walk", wWalk, 6, 20.f, true);
    mWeaponAnims[W_JUMP].setup("w_jump", wJump, 4, 6.f, false);
    mWeaponAnims[W_SHOOT_IDLE].setup("w_s_idle", wShootI, 4, 12.f, false);
    mWeaponAnims[W_MELEE].setup("w_melee", wMelee, 6, 15.f, false);
    mWeaponTextureLoaded = true;
}
void PlayerSoldier::setupPlayer2RL()
{
    if (!mWeaponTexture.loadFromFile("Sprites/Group 27.png")) return;
    IntRect wIdle[4] = { IntRect(12,1609,81,79), IntRect(100,1609,83,79), IntRect(192,1609,83,83), IntRect(284,1609,83,83) };
    IntRect wWalk[8] = { IntRect(17,1713,92,86), IntRect(111,1711,91,88), IntRect(203,1709,88,83), IntRect(294,1711,87,88), IntRect(389,1711,88,89), IntRect(885,1713,90,89), IntRect(983,1713,92,88), IntRect(1081,1713,92,88) };
    IntRect wJump[6] = { IntRect(9,1840,89,96), IntRect(106,1838,84,96), IntRect(196,1836,84,98), IntRect(282,1838,88,96), IntRect(379,1838,87,88), IntRect(477,1840,89,84) };
    IntRect wShootI[4] = { IntRect(12,1939,83,80), IntRect(104,1941,82,78), IntRect(192,1939,84,80), IntRect(279,1939,88,80) };
    IntRect wMelee[6] = { IntRect(29,115,54,83), IntRect(95,115,52,85), IntRect(157,115,58,85), IntRect(227,115,88,83), IntRect(325,107,102,91), IntRect(437,107,102,91) };
    mWeaponAnims[W_IDLE].setup("w_idle", wIdle, 4, 6.f, true);
    mWeaponAnims[W_WALK].setup("w_walk", wWalk, 6, 20.f, true);
    mWeaponAnims[W_JUMP].setup("w_jump", wJump, 4, 6.f, false);
    mWeaponAnims[W_SHOOT_IDLE].setup("w_s_idle", wShootI, 4, 4.f, false);
    mWeaponAnims[W_MELEE].setup("w_melee", wMelee, 6, 15.f, false);
    mWeaponTextureLoaded = true;
}

void PlayerSoldier::setupPlayer3Pistol()
{
    if (!mWeaponTexture.loadFromFile("Sprites/Group 28.png")) return;
    IntRect wShootI[9] = { IntRect(81,11,108,80), IntRect(195,9,112,82), IntRect(311,11,74,80), IntRect(429,11,70,80), IntRect(505,9,68,82), IntRect(583,13,78,78), IntRect(829,11,78,80) };
    mWeaponAnims[W_SHOOT_IDLE].setup("w_s_idle", wShootI, 4, 12.f, false);
    mWeaponTextureLoaded = true;
}
void PlayerSoldier::setupPlayer3HMG()
{
    if (!mWeaponTexture.loadFromFile("Sprites/Group 28.png")) return;
    IntRect wIdle[4] = { IntRect(69,135,76,96), IntRect(157,133,76,98), IntRect(245,131,74,100), IntRect(329,129,74,102) };
    IntRect wWalk[12] = { IntRect(68,256,77,95), IntRect(154,258,77,92), IntRect(240,262,75,90), IntRect(326,264,75,87), IntRect(410,262,77,89), IntRect(494,262,81,95), IntRect(582,262,77,93), IntRect(670,260,77,96), IntRect(760,256,79,101), IntRect(852,258,81,97), IntRect(940,262,81,91), IntRect(1023,258,82,98) };
    IntRect wJump[6] = { IntRect(58,370,77,108), IntRect(144,372,77,104), IntRect(230,376,75,100), IntRect(316,378,75,96), IntRect(400,376,77,93), IntRect(486,376,79,87) };
    IntRect wShootI[4] = { IntRect(45,607,88,82), IntRect(133,607,88,82), IntRect(221,613,90,84), IntRect(313,615,90,82) };
    mWeaponAnims[W_IDLE].setup("w_idle", wIdle, 4, 6.f, true);
    mWeaponAnims[W_WALK].setup("w_walk", wWalk, 6, 20.f, true);
    mWeaponAnims[W_JUMP].setup("w_jump", wJump, 4, 6.f, false);
    mWeaponAnims[W_SHOOT_IDLE].setup("w_s_idle", wShootI, 6, 24.f, true);
    mWeaponTextureLoaded = true;
}
void PlayerSoldier::setupPlayer3Flameshot()
{
    if (!mWeaponTexture.loadFromFile("Sprites/Group 28.png")) return;
    IntRect wIdle[4] = { IntRect(45,607,88,82), IntRect(133,607,88,82), IntRect(221,613,90,84), IntRect(313,615,90,82) };
    IntRect wWalk[8] = { IntRect(235,717,90,81), IntRect(335,717,90,82), IntRect(429,717,90,84), IntRect(523,717,90,90), IntRect(619,717,90,88), IntRect(715,717,90,87), IntRect(1002,717,93,82), IntRect(1099,717,92,87) };
    IntRect wJump[6] = { IntRect(37,813,92,95), IntRect(135,813,92,93), IntRect(233,813,90,91), IntRect(333,813,90,89), IntRect(427,813,90,86), IntRect(521,813,89,80) };
    IntRect wShootI[4] = { IntRect(40,919,83,77), IntRect(138,921,85,75), IntRect(236,921,87,75), IntRect(340,921,91,75) };
    mWeaponAnims[W_IDLE].setup("w_idle", wIdle, 4, 6.f, true);
    mWeaponAnims[W_WALK].setup("w_walk", wWalk, 6, 20.f, true);
    mWeaponAnims[W_JUMP].setup("w_jump", wJump, 4, 6.f, false);
    mWeaponAnims[W_SHOOT_IDLE].setup("w_s_idle", wShootI, 6, 20.f, true);
    mWeaponTextureLoaded = true;
}
void PlayerSoldier::setupPlayer3Laser()
{
    if (!mWeaponTexture.loadFromFile("Sprites/Group 28.png")) return;
    IntRect wIdle[4] = { IntRect(37,1025,88,82), IntRect(125,1025,88,82), IntRect(213,1031,90,84), IntRect(305,1033,92,82) };
    IntRect wWalk[8] = { IntRect(327,1135,90,82), IntRect(421,1135,90,84), IntRect(515,1135,90,90), IntRect(611,1135,90,88), IntRect(707,1135,90,87), IntRect(896,1135,95,86), IntRect(994,1135,93,82), IntRect(1091,1135,96,87) };
    IntRect wJump[6] = { IntRect(29,1231,92,95), IntRect(127,1231,92,93), IntRect(225,1231,90,91), IntRect(325,1231,90,89), IntRect(419,1231,90,86), IntRect(513,1231,90,80) };
    IntRect wShootI[4] = { IntRect(32,1337,83,77), IntRect(130,1339,87,75), IntRect(228,1339,91,75), IntRect(332,1339,93,75) };
    mWeaponAnims[W_IDLE].setup("w_idle", wIdle, 4, 6.f, true);
    mWeaponAnims[W_WALK].setup("w_walk", wWalk, 6, 20.f, true);
    mWeaponAnims[W_JUMP].setup("w_jump", wJump, 4, 6.f, false);
    mWeaponAnims[W_SHOOT_IDLE].setup("w_s_idle", wShootI, 4, 12.f, false);
    mWeaponTextureLoaded = true;
}
void PlayerSoldier::setupPlayer3RL()
{
    if (!mWeaponTexture.loadFromFile("Sprites/Group 28.png")) return;
    IntRect wIdle[4] = { IntRect(27,1433,88,82), IntRect(115,1433,88,82), IntRect(203,1439,90,84), IntRect(295,1441,92,82) };
    IntRect wWalk[8] = { IntRect(317,1543,90,82), IntRect(411,1543,90,84), IntRect(505,1543,90,90), IntRect(601,1543,90,88), IntRect(697,1543,90,87), IntRect(791,1543,92,88), IntRect(984,1543,93,82), IntRect(1081,1543,94,87) };
    IntRect wJump[6] = { IntRect(19,1639,92,95), IntRect(117,1639,92,93), IntRect(215,1639,90,91), IntRect(315,1639,90,89), IntRect(409,1639,90,86), IntRect(503,1639,92,80) };
    IntRect wShootI[4] = { IntRect(22,1745,85,77), IntRect(120,1747,87,75), IntRect(218,1747,89,75), IntRect(322,1747,93,75) };
    mWeaponAnims[W_IDLE].setup("w_idle", wIdle, 4, 6.f, true);
    mWeaponAnims[W_WALK].setup("w_walk", wWalk, 6, 20.f, true);
    mWeaponAnims[W_JUMP].setup("w_jump", wJump, 4, 6.f, false);
    mWeaponAnims[W_SHOOT_IDLE].setup("w_s_idle", wShootI, 4, 4.f, false);
    mWeaponTextureLoaded = true;
}

void PlayerSoldier::setupPlayer4Pistol()
{
    if (!mWeaponTexture.loadFromFile("Sprites/Group 29.png")) return;
    IntRect wShootI[9] = { IntRect(43,18,108,75), IntRect(155,18,108,75), IntRect(265,16,112,77), IntRect(377,14,66,79), IntRect(445,12,66,81), IntRect(511,18,76,75), IntRect(587,18,76,75), IntRect(665,18,74,77), IntRect(743,18,70,77) };
    IntRect wMelee[6] = { IntRect(51,137,52,82), IntRect(107,137,56,80), IntRect(167,139,70,76), IntRect(241,133,102,88), IntRect(615,139,54,76), IntRect(673,139,62,76) };
    mWeaponAnims[W_SHOOT_IDLE].setup("w_s_idle", wShootI, 4, 12.f, false);
    mWeaponAnims[W_MELEE].setup("w_melee", wMelee, 6, 15.f, false);
    mWeaponTextureLoaded = true;
}
void PlayerSoldier::setupPlayer4HMG()
{
    if (!mWeaponTexture.loadFromFile("Sprites/Group 29.png")) return;
    IntRect wIdle[4] = { IntRect(47,245,70,94), IntRect(119,243,70,96), IntRect(191,241,70,98), IntRect(263,239,70,100) };
    IntRect wWalk[6] = { IntRect(28,344,76,99), IntRect(104,334,76,99), IntRect(181,348,71,94), IntRect(252,348,71,94), IntRect(327,350,71,94), IntRect(403,350,73,94) };
    IntRect wJump[6] = { IntRect(41,464,73,107), IntRect(114,464,73,107), IntRect(187,464,73,107), IntRect(260,464,73,107), IntRect(336,470,72,92), IntRect(412,470,74,84) };
    IntRect wShootI[4] = { IntRect(53,594,130,79), IntRect(183,596,134,77), IntRect(319,596,130,77), IntRect(455,594,130,79) };
    IntRect wMelee[6] = { IntRect(51,137,52,82), IntRect(107,137,56,80), IntRect(167,139,70,76), IntRect(241,133,102,88), IntRect(615,139,54,76), IntRect(673,139,62,76) };
    mWeaponAnims[W_IDLE].setup("w_idle", wIdle, 4, 6.f, true);
    mWeaponAnims[W_WALK].setup("w_walk", wWalk, 6, 20.f, true);
    mWeaponAnims[W_JUMP].setup("w_jump", wJump, 4, 6.f, false);
    mWeaponAnims[W_SHOOT_IDLE].setup("w_s_idle", wShootI, 6, 24.f, true);
    mWeaponAnims[W_MELEE].setup("w_melee", wMelee, 6, 15.f, false);
    mWeaponTextureLoaded = true;
}
void PlayerSoldier::setupPlayer4Flameshot()
{
    if (!mWeaponTexture.loadFromFile("Sprites/Group 29.png")) return;
    IntRect wIdle[4] = { IntRect(31,735,84,80), IntRect(117,735,84,80), IntRect(203,735,84,80), IntRect(289,735,84,80) };
    IntRect wWalk[6] = { IntRect(22,842,87,83), IntRect(109,842,87,83), IntRect(197,842,87,83), IntRect(289,842,87,83), IntRect(373,842,85,84), IntRect(986,842,88,84) };
    IntRect wJump[6] = { IntRect(19,926,87,91), IntRect(105,926,87,91), IntRect(192,926,87,91), IntRect(279,926,87,91), IntRect(366,926,86,82), IntRect(456,926,81,74) };
    IntRect wShootI[6] = { IntRect(24,1028,82,74), IntRect(110,1030,82,72), IntRect(196,1030,82,72), IntRect(282,1028,84,74) };
    IntRect wMelee[6] = { IntRect(51,137,52,82), IntRect(107,137,56,80), IntRect(167,139,70,76), IntRect(241,133,102,88), IntRect(615,139,54,76), IntRect(673,139,62,76) };
    mWeaponAnims[W_IDLE].setup("w_idle", wIdle, 4, 6.f, true);
    mWeaponAnims[W_WALK].setup("w_walk", wWalk, 6, 20.f, true);
    mWeaponAnims[W_JUMP].setup("w_jump", wJump, 4, 6.f, false);
    mWeaponAnims[W_SHOOT_IDLE].setup("w_s_idle", wShootI, 6, 20.f, true);
    mWeaponAnims[W_MELEE].setup("w_melee", wMelee, 6, 15.f, false);
    mWeaponTextureLoaded = true;
}
void PlayerSoldier::setupPlayer4Laser()
{
    if (!mWeaponTexture.loadFromFile("Sprites/Group 29.png")) return;
    IntRect wIdle[4] = { IntRect(27,1129,84,80), IntRect(113,1129,84,80), IntRect(199,1129,84,80), IntRect(285,1129,84,80) };
    IntRect wWalk[6] = { IntRect(18,1236,87,83), IntRect(105,1236,87,83), IntRect(192,1236,87,83), IntRect(279,1236,87,83), IntRect(369,1236,85,84), IntRect(982,1236,88,84) };
    IntRect wJump[6] = { IntRect(15,1320,86,91), IntRect(101,1320,86,91), IntRect(187,1320,86,91), IntRect(274,1320,86,91), IntRect(362,1320,86,82), IntRect(452,1320,81,74) };
    IntRect wShootI[4] = { IntRect(20,1422,82,74), IntRect(106,1424,82,72), IntRect(192,1424,82,72), IntRect(278,1422,84,74) };
    IntRect wMelee[6] = { IntRect(51,137,52,82), IntRect(107,137,56,80), IntRect(167,139,70,76), IntRect(241,133,102,88), IntRect(615,139,54,76), IntRect(673,139,62,76) };
    mWeaponAnims[W_IDLE].setup("w_idle", wIdle, 4, 6.f, true);
    mWeaponAnims[W_WALK].setup("w_walk", wWalk, 6, 20.f, true);
    mWeaponAnims[W_JUMP].setup("w_jump", wJump, 4, 6.f, false);
    mWeaponAnims[W_SHOOT_IDLE].setup("w_s_idle", wShootI, 4, 12.f, false);
    mWeaponAnims[W_MELEE].setup("w_melee", wMelee, 6, 15.f, false);
    mWeaponTextureLoaded = true;
}
void PlayerSoldier::setupPlayer4RL()
{
    if (!mWeaponTexture.loadFromFile("Sprites/Group 29.png")) return;
    IntRect wIdle[4] = { IntRect(17,1513,84,80), IntRect(103,1513,84,80), IntRect(189,1513,84,80), IntRect(275,1513,84,80) };
    IntRect wWalk[6] = { IntRect(8,1620,87,83), IntRect(95,1620,87,83), IntRect(182,1620,87,83), IntRect(269,1620,87,83), IntRect(359,1620,85,84), IntRect(972,1620,88,84) };
    IntRect wJump[6] = { IntRect(5,1704,86,91), IntRect(91,1704,86,91), IntRect(177,1704,86,91), IntRect(263,1704,86,91), IntRect(352,1704,86,82), IntRect(442,1704,84,74) };
    IntRect wShootI[4] = { IntRect(10,1806,82,74), IntRect(96,1808,82,72), IntRect(182,1808,82,72), IntRect(268,1806,84,74) };
    IntRect wMelee[6] = { IntRect(51,137,52,82), IntRect(107,137,56,80), IntRect(167,139,70,76), IntRect(241,133,102,88), IntRect(615,139,54,76), IntRect(673,139,62,76) };
    mWeaponAnims[W_IDLE].setup("w_idle", wIdle, 4, 6.f, true);
    mWeaponAnims[W_WALK].setup("w_walk", wWalk, 6, 20.f, true);
    mWeaponAnims[W_JUMP].setup("w_jump", wJump, 4, 6.f, false);
    mWeaponAnims[W_SHOOT_IDLE].setup("w_s_idle", wShootI, 4, 4.f, false);
    mWeaponAnims[W_MELEE].setup("w_melee", wMelee, 6, 15.f, false);
    mWeaponTextureLoaded = true;
}