#include "Soldier.h"
#include <SFML/Window/Keyboard.hpp>

bool isTile(char** lvl, int x, int y, int mapWidth, int mapHeight)
{
    if (x < 0 || x >= mapWidth || y < 0 || y >= mapHeight)
        return false;

    return lvl[y][x] == 'g';
}

Soldier::Soldier(float spd, int hp)
{
    speed = spd;
    health = hp;
    mcurrentAnim = IDLE;
}

void Soldier::setPosition(float x, float y)
{
    msprite.setPosition(x, y);
}

void Soldier::update(float dt, char** lvl, int cell_size, int mapWidth, int mapHeight)
{


    // ---------------- APPLY GRAVITY ----------------
    if (!onGround)
    {
        velocityY += gravity;
        if (velocityY > 20.f)
            velocityY = 20.f;
    }
    else
    {
        velocityY = 0.f;
    }

    // fixed size box — never changes with animation frame
    float bx = msprite.getPosition().x + collisionOffsetX;
    float by = msprite.getPosition().y + collisionOffsetY;
    float bw = collisionWidth;
    float bh = collisionHeight;

    // ---------------- MOVE X ----------------
    msprite.move(velocityX, 0);
    bx = msprite.getPosition().x + collisionOffsetX;

    int leftTile = (int)(bx / cell_size);
    int rightTile = (int)((bx + bw - 1) / cell_size);
    int topTile = (int)(by / cell_size);
    int bottomTile = (int)((by + bh - 1) / cell_size);

    if (leftTile < 0) leftTile = 0; else if (leftTile >= mapWidth)   leftTile = mapWidth - 1;
    if (rightTile < 0) rightTile = 0; else if (rightTile >= mapWidth)  rightTile = mapWidth - 1;
    if (topTile < 0) topTile = 0; else if (topTile >= mapHeight)  topTile = mapHeight - 1;
    if (bottomTile < 0) bottomTile = 0; else if (bottomTile >= mapHeight) bottomTile = mapHeight - 1;

    if (velocityX > 0)
    {
        for (int i = topTile; i <= bottomTile; i++)
        {
            if (isTile(lvl, rightTile, i, mapWidth, mapHeight))
            {
                msprite.setPosition(rightTile * cell_size - bw - collisionOffsetX, msprite.getPosition().y);
                velocityX = 0;
                bx = msprite.getPosition().x + collisionOffsetX;
                break;
            }
        }
    }
    else if (velocityX < 0)
    {
        for (int i = topTile; i <= bottomTile; i++)
        {
            if (isTile(lvl, leftTile, i, mapWidth, mapHeight))
            {
                msprite.setPosition((leftTile + 1) * cell_size - collisionOffsetX, msprite.getPosition().y);
                velocityX = 0;
                bx = msprite.getPosition().x + collisionOffsetX;
                break;
            }
        }
    }

    // ---------------- MOVE Y ----------------
    msprite.move(0, velocityY);
    by = msprite.getPosition().y + collisionOffsetY;

    leftTile = (int)(bx / cell_size);
    rightTile = (int)((bx + bw - 1) / cell_size);
    topTile = (int)(by / cell_size);
    bottomTile = (int)((by + bh - 1) / cell_size);

    if (leftTile < 0) leftTile = 0; else if (leftTile >= mapWidth)   leftTile = mapWidth - 1;
    if (rightTile < 0) rightTile = 0; else if (rightTile >= mapWidth)  rightTile = mapWidth - 1;
    if (topTile < 0) topTile = 0; else if (topTile >= mapHeight)  topTile = mapHeight - 1;
    if (bottomTile < 0) bottomTile = 0; else if (bottomTile >= mapHeight) bottomTile = mapHeight - 1;

    onGround = false;

    if (velocityY >= 0)
    {
        for (int j = leftTile; j <= rightTile; j++)
        {
            if (isTile(lvl, j, bottomTile, mapWidth, mapHeight))
            {
                msprite.setPosition(msprite.getPosition().x, bottomTile * cell_size - bh - collisionOffsetY);
                velocityY = 0;
                onGround = true;
                isJumping = false;
                by = msprite.getPosition().y + collisionOffsetY;
                break;
            }
        }
    }
    else if (velocityY < 0)
    {
        for (int j = leftTile; j <= rightTile; j++)
        {
            if (isTile(lvl, j, topTile, mapWidth, mapHeight))
            {
                msprite.setPosition(msprite.getPosition().x, (topTile + 1) * cell_size - collisionOffsetY);
                velocityY = 0;
                isJumping = false;
                by = msprite.getPosition().y + collisionOffsetY;
                break;
            }
        }
    }

    // ---------------- CLAMP TO MAP ----------------
    bx = msprite.getPosition().x + collisionOffsetX;
    by = msprite.getPosition().y + collisionOffsetY;

    float clampedX = bx;
    float clampedY = by;

    if (clampedX < 0.f) clampedX = 0.f;
    else if (clampedX > mapWidth * cell_size - bw)
        clampedX = mapWidth * cell_size - bw;

    if (clampedY < 0.f) clampedY = 0.f;
    else if (clampedY > mapHeight * cell_size - bh)
    {
        clampedY = mapHeight * cell_size - bh;
        velocityY = 0;
        onGround = true;
        isJumping = false;
    }

    msprite.setPosition(clampedX - collisionOffsetX, clampedY - collisionOffsetY);

    // ---------------- ANIMATION ----------------
    bool isRunning = (velocityX > 0.f || velocityX < 0.f);

    if (isJumping)
    {
        if (isRunning)
            setAnimation(JUMP_RUN);
        else
            setAnimation(JUMP_IDLE);
    }
    else if (isRunning)
    {
        setAnimation(WALK);
    }
    else
    {
        setAnimation(IDLE);
    }

    manimations[mcurrentAnim].update(dt);
    msprite.setTextureRect(manimations[mcurrentAnim].getCurrentFrame());
    if (facingRight)
    {
        msprite.setScale(baseScale, baseScale);
    }
    else
    {
        msprite.setScale(-baseScale, baseScale);
    }
}

Sprite& Soldier::getSprite()
{
    return msprite;
}

void Soldier::setAnimation(AnimType type)
{
    if (mcurrentAnim == type && !manimations[type].isFinished())
        return;

    mcurrentAnim = type;
    manimations[type].reset();
    manimations[type].play();
}

// ---------------- CONSTRUCTOR ----------------

PlayerSoldier::PlayerSoldier(int index)
    : Soldier(3.0f, 100)
{
    mplayerIndex = index;
    loadPlayerData();
    setAnimation(IDLE);
}

// ---------------- UPDATE ----------------

void PlayerSoldier::update(float dt, char** lvl, int cell_size, int mapWidth, int mapHeight)
{
    handleInput(dt);
    Soldier::update(dt, lvl, cell_size, mapWidth, mapHeight);
}

// ---------------- INPUT ----------------

void PlayerSoldier::handleInput(float dt)
{
    velocityX = 0;

    if (mplayerIndex == 0)
    {
        if (Keyboard::isKeyPressed(Keyboard::Right))
        {
            velocityX = speed;
            facingRight = true;
        }
        if (Keyboard::isKeyPressed(Keyboard::Left))
        {
            velocityX = -speed;
            facingRight = false;
        }

        if (Keyboard::isKeyPressed(Keyboard::Up) && onGround)
        {
            velocityY = jumpStrength;
            onGround = false;
            isJumping = true;
        }
    }
    else if (mplayerIndex == 1)
    {
        if (Keyboard::isKeyPressed(Keyboard::D)) {
            velocityX = speed;
            facingRight = true;
        }

        if (Keyboard::isKeyPressed(Keyboard::A)) {
            velocityX = -speed;
            facingRight = false;
        }

        if (Keyboard::isKeyPressed(Keyboard::W) && onGround)
        {
            velocityY = jumpStrength;
            onGround = false;
            isJumping = true;
        }

        if (onGround)
            coyoteTime = coyoteMax;
        else
            coyoteTime -= dt;
    }
}

// ---------------- LOAD PLAYER DATA ----------------

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

// ---------------- PLAYER 1 ANIMATIONS ----------------

void PlayerSoldier::setupPlayer1Animations()
{
    IntRect idle[4] = {
        IntRect(90,4,83,101),
        IntRect(174,4,84,101),
        IntRect(264,4,81,101),
        IntRect(346,4,81,101)
    };

    IntRect walk[12] = {
        IntRect(88,126,79,90),
        IntRect(168,126,78,92),
        IntRect(246,126,78,92),
        IntRect(325,128,77,99),
        IntRect(402,128,77,99),
        IntRect(485,128,77,99),
        IntRect(567,130,77,97),
        IntRect(647,130,77,99),
        IntRect(727,130,79,97),
        IntRect(811,130,78,97),
        IntRect(889,130,78,97),
        IntRect(971,130,74,97)
    };

    IntRect jumpIdle[6] = {
        IntRect(88,238,66,106),
        IntRect(157,242,70,102),
        IntRect(232,244,71,98),
        IntRect(308,242,69,98),
        IntRect(384,242,69,92),
        IntRect(459,244,67,84)
    };

    IntRect jumpRun[6] = {
        IntRect(86,362,73,97),
        IntRect(159,362,73,97),
        IntRect(239,362,79,88),
        IntRect(318,360,73,90),
        IntRect(391,360,73,90),
        IntRect(464,360,73,90)
    };

    IntRect swimIdle[6] = {
        IntRect(22,803,136,77),
        IntRect(165,801,139,75),
        IntRect(312,812,137,67),
        IntRect(457,812,140,64),
        IntRect(598,810,141,64),
        IntRect(758,808,140,64)
    };

    IntRect swim[10] = {
        IntRect(34,916,132,74),
        IntRect(176,911,130,67),
        IntRect(314,916,133,61),
        IntRect(456,906,139,64),
        IntRect(600,908,138,61),
        IntRect(756,905,140,60),
        IntRect(898,900,138,66),
        IntRect(1054,895,136,66),
        IntRect(1196,889,130,77),
        IntRect(1337,890,131,76)
    };

    IntRect die[21] = {
        IntRect(78,687,72,93),
        IntRect(155,694,79,86),
        IntRect(239,694,83,86),
        IntRect(328,692,68,88),
        IntRect(399,685,69,95),
        IntRect(471,682,74,98),
        IntRect(548,680,71,100),
        IntRect(622,680,68,100),
        IntRect(696,680,61,100),
        IntRect(761,687,66,93),
        IntRect(832,690,67,90),
        IntRect(902,694,71,86),
        IntRect(976,706,78,74),
        IntRect(1057,704,74,76),
        IntRect(1136,706,69,74),
        IntRect(1208,709,66,71),
        IntRect(1277,713,69,67),
        IntRect(1351,718,69,62),
        IntRect(1426,728,71,52),
        IntRect(1500,742,88,38),
        IntRect(1588,740,84,40)
    };

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
    IntRect idle[4] = {
        IntRect(8,10,48,60),
        IntRect(61,10,48,61),
        IntRect(115,10,49,61),
        IntRect(170,10,51,61)
    };

    IntRect walk[6] = {
        IntRect(2,82,48,67),
        IntRect(58,82,49,67),
        IntRect(115,82,48,67),
        IntRect(170,82,50,67),
        IntRect(223,82,52,67),
        IntRect(281,82,50,67)
    };

    IntRect jumpIdle[6] = {
        IntRect(3,169,47,70),
        IntRect(53,170,48,69),
        IntRect(107,172,48,67),
        IntRect(158,173,48,66),
        IntRect(213,173,47,63),
        IntRect(264,173,47,62)
    };

    IntRect jumpRun[6] = {
        IntRect(329,170,48,66),
        IntRect(382,170,48,63),
        IntRect(436,170,55,60),
        IntRect(496,169,49,58),
        IntRect(549,169,49,58),
        IntRect(607,169,43,60)
    };

    /*IntRect swimIdle[6] = {
        IntRect(22,803,136,77),
        IntRect(165,801,139,75),
        IntRect(312,812,137,67),
        IntRect(457,812,140,64),
        IntRect(598,810,141,64),
        IntRect(758,808,140,64)
    };

    IntRect swim[10] = {
        IntRect(34,916,132,74),
        IntRect(176,911,130,67),
        IntRect(314,916,133,61),
        IntRect(456,906,139,64),
        IntRect(600,908,138,61),
        IntRect(756,905,140,60),
        IntRect(898,900,138,66),
        IntRect(1054,895,136,66),
        IntRect(1196,889,130,77),
        IntRect(1337,890,131,76)
    };*/

    IntRect die[19] = {
        IntRect(4,253,43,60),
        IntRect(58,256,49,55),
        IntRect(113,257,47,56),
        IntRect(169,256,46,57),
        IntRect(226,254,48,56),
        IntRect(283,260,42,50),
        IntRect(332,260,44,50),
        IntRect(389,263,47,48),
        IntRect(446,268,53,43),
        IntRect(510,266,56,42),
        IntRect(572,271,50,34),
        IntRect(631,265,48,40),
        IntRect(687,265,40,42),
        IntRect(736,260,37,48),
        IntRect(778,265,39,45),
        IntRect(826,268,43,45),
        IntRect(874,278,48,35),
        IntRect(927,284,53,29),
        IntRect(985,289,57,24)
    };

    manimations[IDLE].setup("idle", idle, 4, 6.f, true);
    manimations[WALK].setup("walk", walk, 6, 15.f, true);
    // manimations[SWIM_IDLE].setup("swim_idle", swimIdle, 6, 8.f, false);
    // manimations[SWIM].setup("swim", swim, 10, 8.f, false);
    manimations[JUMP_IDLE].setup("jump_idle", jumpIdle, 6, 6.f, false);
    manimations[JUMP_RUN].setup("jump_run", jumpRun, 6, 6.f, false);
    manimations[DIE].setup("die", die, 19, 5.f, false);
}

void PlayerSoldier::setupPlayer3Animations()
{
    IntRect idle[4] = {
        IntRect(0,1,63,75),
        IntRect(68,1,61,75),
        IntRect(136,1,63,75),
        IntRect(206,1,66,75)
    };

    IntRect walk[6] = {
        IntRect(0,82,67,85),
        IntRect(67,82,67,85),
        IntRect(135,82,63,85),
        IntRect(195,82,63,67),
        IntRect(262,82,64,84),
        IntRect(326,82,67,83)
    };

    IntRect jumpIdle[6] = {
        IntRect(7,170,60,99),
        IntRect(75,172,60,98),
        IntRect(145,175,60,91),
        IntRect(213,175,60,87),
        IntRect(281,175,60,77),
        IntRect(349,175,60,78)
    };

    IntRect jumpRun[6] = {
        IntRect(1,272,61,94),
        IntRect(71,272,63,87),
        IntRect(143,272,73,78),
        IntRect(237,272,66,73),
        IntRect(310,272,68,75),
        IntRect(393,272,57,81)
    };

    /*IntRect swimIdle[6] = {
        IntRect(22,803,136,77),
        IntRect(165,801,139,75),
        IntRect(312,812,137,67),
        IntRect(457,812,140,64),
        IntRect(598,810,141,64),
        IntRect(758,808,140,64)
    };

    IntRect swim[10] = {
        IntRect(34,916,132,74),
        IntRect(176,911,130,67),
        IntRect(314,916,133,61),
        IntRect(456,906,139,64),
        IntRect(600,908,138,61),
        IntRect(756,905,140,60),
        IntRect(898,900,138,66),
        IntRect(1054,895,136,66),
        IntRect(1196,889,130,77),
        IntRect(1337,890,131,76)
    };*/

    IntRect die[19] = {
        IntRect(4,253,43,60),
        IntRect(58,256,49,55),
        IntRect(113,257,47,56),
        IntRect(169,256,46,57),
        IntRect(226,254,48,56),
        IntRect(283,260,42,50),
        IntRect(332,260,44,50),
        IntRect(389,263,47,48),
        IntRect(446,268,53,43),
        IntRect(510,266,56,42),
        IntRect(572,271,50,34),
        IntRect(631,265,48,40),
        IntRect(687,265,40,42),
        IntRect(736,260,37,48),
        IntRect(778,265,39,45),
        IntRect(826,268,43,45),
        IntRect(874,278,48,35),
        IntRect(927,284,53,29),
        IntRect(985,289,57,24)
    };

    manimations[IDLE].setup("idle", idle, 4, 6.f, true);
    manimations[WALK].setup("walk", walk, 6, 15.f, true);
    // manimations[SWIM_IDLE].setup("swim_idle", swimIdle, 6, 8.f, false);
    // manimations[SWIM].setup("swim", swim, 10, 8.f, false);
    manimations[JUMP_IDLE].setup("jump_idle", jumpIdle, 6, 6.f, false);
    manimations[JUMP_RUN].setup("jump_run", jumpRun, 6, 6.f, false);
    manimations[DIE].setup("die", die, 19, 5.f, false);
}


void PlayerSoldier::setupPlayer4Animations()
{
    IntRect idle[4] = {
        IntRect(4,0,31,37),
        IntRect(37,0,31,38),
        IntRect(70,0,31,38),
        IntRect(103,0,31,38)
    };

    IntRect walk[12] = {
        IntRect(0,54,32,39),
        IntRect(32,54,32,39),
        IntRect(65,56,31,35),
        IntRect(96,55,32,37),
        IntRect(132,54,28,38),
        IntRect(164,54,29,39),
        IntRect(196,55,30,38),
        IntRect(227,56,33,39),
        IntRect(260,56,33,39),
        IntRect(293,56,33,39),
        IntRect(328,55,29,38),
        IntRect(359,55,30,38)
    };

    IntRect jumpIdle[6] = {
        IntRect(6,105,29,45),
        IntRect(40,107,30,43),
        IntRect(75,109,30,40),
        IntRect(110,109,30,39),
        IntRect(145,109,29,37),
        IntRect(178,109,29,35)
    };

    IntRect jumpRun[6] = {
        IntRect(9,163,29,43),
        IntRect(38,163,29,43),
        IntRect(71,163,34,32),
        IntRect(106,163,30,35),
        IntRect(138,163,30,36),
        IntRect(168,163,26,37)
    };

    /*IntRect swimIdle[6] = {
        IntRect(22,803,136,77),
        IntRect(165,801,139,75),
        IntRect(312,812,137,67),
        IntRect(457,812,140,64),
        IntRect(598,810,141,64),
        IntRect(758,808,140,64)
    };

    IntRect swim[10] = {
        IntRect(34,916,132,74),
        IntRect(176,911,130,67),
        IntRect(314,916,133,61),
        IntRect(456,906,139,64),
        IntRect(600,908,138,61),
        IntRect(756,905,140,60),
        IntRect(898,900,138,66),
        IntRect(1054,895,136,66),
        IntRect(1196,889,130,77),
        IntRect(1337,890,131,76)
    };*/

    IntRect die[19] = {
        IntRect(4,253,43,60),
        IntRect(58,256,49,55),
        IntRect(113,257,47,56),
        IntRect(169,256,46,57),
        IntRect(226,254,48,56),
        IntRect(283,260,42,50),
        IntRect(332,260,44,50),
        IntRect(389,263,47,48),
        IntRect(446,268,53,43),
        IntRect(510,266,56,42),
        IntRect(572,271,50,34),
        IntRect(631,265,48,40),
        IntRect(687,265,40,42),
        IntRect(736,260,37,48),
        IntRect(778,265,39,45),
        IntRect(826,268,43,45),
        IntRect(874,278,48,35),
        IntRect(927,284,53,29),
        IntRect(985,289,57,24)
    };

    manimations[IDLE].setup("idle", idle, 4, 6.f, true);
    manimations[WALK].setup("walk", walk, 6, 20.f, true);
    // manimations[SWIM_IDLE].setup("swim_idle", swimIdle, 6, 8.f, false);
    // manimations[SWIM].setup("swim", swim, 10, 8.f, false);
    manimations[JUMP_IDLE].setup("jump_idle", jumpIdle, 6, 6.f, false);
    manimations[JUMP_RUN].setup("jump_run", jumpRun, 6, 6.f, false);
    manimations[DIE].setup("die", die, 19, 5.f, false);
}


// ---------------- OTHER PLAYERS ----------------

//void PlayerSoldier::setupPlayer2Animations() { setupPlayer2Animations(); }
//void PlayerSoldier::setupPlayer3Animations() { setupPlayer1Animations(); }
//void PlayerSoldier::setupPlayer4Animations() { setupPlayer1Animations(); }