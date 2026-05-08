/*#include "Enemy.h"
#include <cstdlib>
#include <algorithm>

// ------------------------------------------------------------------ helpers ---

static bool isTileE(char** lvl, int x, int y, int mapWidth, int mapHeight)
{
    if (x < 0 || x >= mapWidth || y < 0 || y >= mapHeight)
        return false;
    return lvl[y][x] == 'g';
}

// ------------------------------------------------------------------ base ---

Enemies::Enemies(int dmg, float spd, int hp)
{
    damage = dmg;
    speed = spd;
    health = hp;
    alive = true;
    m_state = 0;
    m_stateTimer = 0.f;
    m_animCount = 0;
    m_currentAnimation = nullptr;
    m_sprite.setPosition(0.f, 0.f);
}

Enemies::~Enemies() {}

void Enemies::copyName(char* dest, const char* src, int maxLen)
{
    int i = 0;
    for (; i < maxLen - 1 && src[i] != '\0'; i++)
        dest[i] = src[i];
    dest[i] = '\0';
}

void Enemies::addAnimation(const char* name, const Animation& anim)
{
    if (m_animCount >= 10) return;
    copyName(m_animNames[m_animCount], name, 32);
    m_animations[m_animCount] = anim;
    m_animCount++;
}

// Only resets + restarts if we're switching to a DIFFERENT animation.
void Enemies::setAnimation(const char* name)
{
    for (int i = 0; i < m_animCount; i++)
    {
        if (std::strcmp(m_animNames[i], name) == 0)
        {
            if (m_currentAnimation == &m_animations[i])
                return;                         // already playing – do nothing

            m_currentAnimation = &m_animations[i];
            m_currentAnimation->reset();
            m_currentAnimation->play();
            return;
        }
    }
}

Animation* Enemies::getCurrentAnimation() { return m_currentAnimation; }
sf::Sprite& Enemies::getSprite() { return m_sprite; }
bool        Enemies::isAlive() const { return alive; }

// ------------------------------------------------------------------ physics ---

void Enemies::physicsUpdate(float dt, char** lvl, int cell_size, int mapWidth, int mapHeight)
{
    // --- gravity ---
    if (!onGround)
    {
        velocityY += gravity;
        if (velocityY > 20.f) velocityY = 20.f;
    }
    else
    {
        velocityY = 0.f;
    }

    float bx = m_sprite.getPosition().x + collisionOffsetX;
    float by = m_sprite.getPosition().y + collisionOffsetY;
    float bw = collisionWidth;
    float bh = collisionHeight;

    // --- move X ---
    m_sprite.move(velocityX, 0.f);
    bx = m_sprite.getPosition().x + collisionOffsetX;

    int L = std::max(0, (int)(bx / cell_size));
    int R = std::min(mapWidth - 1, (int)((bx + bw - 1) / cell_size));
    int T = std::max(0, (int)(by / cell_size));
    int B = std::min(mapHeight - 1, (int)((by + bh - 1) / cell_size));

    if (velocityX > 0.f)
    {
        for (int i = T; i <= B; i++)
        {
            if (isTileE(lvl, R, i, mapWidth, mapHeight))
            {
                m_sprite.setPosition(R * cell_size - bw - collisionOffsetX,
                    m_sprite.getPosition().y);
                velocityX = 0.f;
                bx = m_sprite.getPosition().x + collisionOffsetX;
                break;
            }
        }
    }
    else if (velocityX < 0.f)
    {
        for (int i = T; i <= B; i++)
        {
            if (isTileE(lvl, L, i, mapWidth, mapHeight))
            {
                m_sprite.setPosition((L + 1) * cell_size - collisionOffsetX,
                    m_sprite.getPosition().y);
                velocityX = 0.f;
                bx = m_sprite.getPosition().x + collisionOffsetX;
                break;
            }
        }
    }

    // --- move Y ---
    m_sprite.move(0.f, velocityY);
    by = m_sprite.getPosition().y + collisionOffsetY;

    L = std::max(0, (int)(bx / cell_size));
    R = std::min(mapWidth - 1, (int)((bx + bw - 1) / cell_size));
    T = std::max(0, (int)(by / cell_size));
    B = std::min(mapHeight - 1, (int)((by + bh - 1) / cell_size));

    onGround = false;

    if (velocityY >= 0.f)
    {
        for (int j = L; j <= R; j++)
        {
            if (isTileE(lvl, j, B, mapWidth, mapHeight))
            {
                m_sprite.setPosition(m_sprite.getPosition().x,
                    B * cell_size - bh - collisionOffsetY);
                velocityY = 0.f;
                onGround = true;
                break;
            }
        }
    }
    else
    {
        for (int j = L; j <= R; j++)
        {
            if (isTileE(lvl, j, T, mapWidth, mapHeight))
            {
                m_sprite.setPosition(m_sprite.getPosition().x,
                    (T + 1) * cell_size - collisionOffsetY);
                velocityY = 0.f;
                break;
            }
        }
    }

    // --- clamp to map bounds ---
    bx = m_sprite.getPosition().x + collisionOffsetX;
    by = m_sprite.getPosition().y + collisionOffsetY;

    float cx = bx, cy = by;

    if (cx < 0.f)                              cx = 0.f;
    else if (cx > mapWidth * cell_size - bw)  cx = mapWidth * cell_size - bw;

    if (cy < 0.f)                              cy = 0.f;
    else if (cy > mapHeight * cell_size - bh)
    {
        cy = mapHeight * cell_size - bh;
        velocityY = 0.f;
        onGround = true;
    }

    m_sprite.setPosition(cx - collisionOffsetX, cy - collisionOffsetY);
}

// ------------------------------------------------------------------ update / AI ---

void Enemies::update(float dt, char** lvl, int cell_size, int mapWidth, int mapHeight)
{
    updateAI(dt);
    move(dt);                                                   // sets velocityX
    physicsUpdate(dt, lvl, cell_size, mapWidth, mapHeight);     // applies movement + collision

    if (m_currentAnimation)
    {
        m_currentAnimation->update(dt);
        m_sprite.setTextureRect(m_currentAnimation->getCurrentFrame());
    }
}

void Enemies::updateAI(float dt)
{
    // Don't interrupt a non-looping animation (e.g. attack)
    if (m_currentAnimation &&
        !m_currentAnimation->isLooping() &&
        !m_currentAnimation->isFinished())
        return;

    m_stateTimer += dt;
    if (m_stateTimer > 0.2f)
    {
        m_stateTimer = 0.f;

        int r = rand() % 3;
        if (r == 0) m_state = 0;   // idle
        else if (r == 1) m_state = 1;   // walk
        else             m_state = 2;   // attack

        if (m_state == 0) setAnimation("idle");
        else if (m_state == 2) setAnimation("attack");
        // walk animation is set inside move()
    }
}

// ------------------------------------------------------------------ Infantry ---

class Infantry : public Enemies
{
public:
    Infantry(int dmg, float spd, int hp) : Enemies(dmg, spd, hp) {}
    virtual void move(float dt) = 0;
};

// ------------------------------------------------------------------ RebelSoldier ---

class RebelSoldier : public Infantry
{
public:
    RebelSoldier() : Infantry(10, 2.0f, 120)
    {
        m_texture.loadFromFile("Sprites/normalrebelsoldier.png");
        m_sprite.setTexture(m_texture);
        setupAnimations();
        setAnimation("idle");
    }

    void attack() override { setAnimation("attack"); }

    void move(float dt) override
    {
        if (m_state == 1)
        {
            velocityX = speed;
            setAnimation("walk");
        }
        else
        {
            velocityX = 0.f;
        }
    }

private:
    void setupAnimations()
    {
        sf::IntRect idle[4];
        idle[0] = sf::IntRect(3, 3, 40, 38);
        idle[1] = sf::IntRect(46, 3, 40, 38);
        idle[2] = sf::IntRect(131, 3, 40, 38);
        idle[3] = sf::IntRect(89, 4, 39, 37);

        sf::IntRect walk[15];
        walk[0] = sf::IntRect(3, 44, 34, 37);
        walk[1] = sf::IntRect(40, 44, 37, 37);
        walk[2] = sf::IntRect(80, 45, 42, 31);
        walk[3] = sf::IntRect(125, 46, 42, 33);
        walk[4] = sf::IntRect(170, 45, 37, 35);
        walk[5] = sf::IntRect(210, 44, 35, 37);
        walk[6] = sf::IntRect(131, 3, 40, 38);
        walk[7] = sf::IntRect(248, 44, 34, 37);
        walk[8] = sf::IntRect(285, 44, 38, 36);
        walk[9] = sf::IntRect(326, 45, 40, 30);
        walk[10] = sf::IntRect(369, 46, 38, 32);
        walk[11] = sf::IntRect(410, 45, 37, 35);
        walk[12] = sf::IntRect(450, 44, 35, 37);
        walk[13] = sf::IntRect(3, 85, 39, 37);
        walk[14] = sf::IntRect(45, 84, 40, 38);

        sf::IntRect attack[10];
        attack[0] = sf::IntRect(3, 298, 44, 38);
        attack[1] = sf::IntRect(50, 298, 48, 38);
        attack[2] = sf::IntRect(101, 298, 47, 38);
        attack[3] = sf::IntRect(151, 297, 42, 39);
        attack[4] = sf::IntRect(196, 289, 41, 47);
        attack[5] = sf::IntRect(240, 289, 41, 47);
        attack[6] = sf::IntRect(284, 292, 43, 44);
        attack[7] = sf::IntRect(330, 298, 47, 38);
        attack[8] = sf::IntRect(380, 298, 46, 38);
        attack[9] = sf::IntRect(429, 298, 40, 38);

        Animation a;
        a.setup("idle", idle, 4, 10.f, true);  addAnimation("idle", a);
        a.setup("walk", walk, 15, 10.f, true);  addAnimation("walk", a);
        a.setup("attack", attack, 10, 10.f, false); addAnimation("attack", a);
    }
};

// ------------------------------------------------------------------ ShieldedSoldier ---

class ShieldedSoldier : public Infantry
{
public:
    ShieldedSoldier() : Infantry(10, 2.0f, 120)
    {
        m_texture.loadFromFile("Sprites/shieldedrebelsoldier.png");
        m_sprite.setTexture(m_texture);
        setupAnimations();
        setAnimation("idle");
    }

    void attack() override { setAnimation("attack"); }

    void move(float dt) override
    {
        if (m_state == 1)
        {
            velocityX = speed;
            setAnimation("walk");
        }
        else
        {
            velocityX = 0.f;
        }
    }

private:
    void setupAnimations()
    {
        sf::IntRect idle[6];
        idle[0] = sf::IntRect(3, 5, 32, 38);
        idle[1] = sf::IntRect(38, 5, 32, 38);
        idle[2] = sf::IntRect(73, 5, 31, 38);
        idle[3] = sf::IntRect(107, 4, 32, 39);
        idle[4] = sf::IntRect(143, 5, 31, 38);
        idle[5] = sf::IntRect(177, 5, 32, 38);

        sf::IntRect walk[16];
        walk[0] = sf::IntRect(3, 47, 29, 40);
        walk[1] = sf::IntRect(35, 46, 28, 41);
        walk[2] = sf::IntRect(66, 46, 30, 40);
        walk[3] = sf::IntRect(99, 47, 31, 39);
        walk[4] = sf::IntRect(133, 49, 31, 38);
        walk[5] = sf::IntRect(167, 48, 32, 39);
        walk[6] = sf::IntRect(202, 47, 30, 40);
        walk[7] = sf::IntRect(235, 46, 29, 41);
        walk[8] = sf::IntRect(267, 46, 28, 40);
        walk[9] = sf::IntRect(298, 47, 28, 39);
        walk[10] = sf::IntRect(329, 49, 27, 38);
        walk[11] = sf::IntRect(359, 48, 27, 39);
        walk[12] = sf::IntRect(3, 90, 32, 38);
        walk[13] = sf::IntRect(38, 90, 32, 38);
        walk[14] = sf::IntRect(73, 90, 36, 38);
        walk[15] = sf::IntRect(112, 90, 32, 38);

        sf::IntRect attack[10];
        attack[0] = sf::IntRect(3, 305, 30, 38);
        attack[1] = sf::IntRect(36, 305, 28, 38);
        attack[2] = sf::IntRect(67, 305, 31, 38);
        attack[3] = sf::IntRect(101, 305, 31, 38);
        attack[4] = sf::IntRect(135, 305, 38, 38);
        attack[5] = sf::IntRect(176, 305, 37, 38);
        attack[6] = sf::IntRect(216, 305, 35, 38);
        attack[7] = sf::IntRect(254, 305, 34, 38);
        attack[8] = sf::IntRect(291, 305, 35, 38);
        attack[9] = sf::IntRect(329, 302, 37, 41);

        Animation a;
        a.setup("idle", idle, 6, 10.f, true);  addAnimation("idle", a);
        a.setup("walk", walk, 16, 10.f, true);  addAnimation("walk", a);
        a.setup("attack", attack, 10, 10.f, false); addAnimation("attack", a);
    }
};

// ------------------------------------------------------------------ GrenadeSoldier ---

class GrenadeSoldier : public Infantry
{
public:
    GrenadeSoldier() : Infantry(20, 2.0f, 80)
    {
        m_texture.loadFromFile("Sprites/grenaderebelsoldier.png");
        m_sprite.setTexture(m_texture);
        setupAnimations();
        setAnimation("idle");
    }

    void attack() override { setAnimation("attack"); }

    void move(float dt) override
    {
        if (m_state == 1)
        {
            velocityX = speed;
            setAnimation("walk");
        }
        else
        {
            velocityX = 0.f;
        }
    }

private:
    void setupAnimations()
    {
        sf::IntRect idle[4];
        idle[0] = sf::IntRect(18, 63, 64, 91);
        idle[1] = sf::IntRect(94, 66, 65, 88);
        idle[2] = sf::IntRect(171, 68, 62, 86);
        idle[3] = sf::IntRect(245, 70, 57, 84);

        sf::IntRect walk[12];
        walk[0] = sf::IntRect(25, 216, 48, 91);
        walk[1] = sf::IntRect(85, 211, 47, 96);
        walk[2] = sf::IntRect(144, 211, 55, 96);
        walk[3] = sf::IntRect(211, 213, 63, 94);
        walk[4] = sf::IntRect(286, 216, 57, 91);
        walk[5] = sf::IntRect(355, 216, 53, 91);
        walk[6] = sf::IntRect(420, 216, 50, 91);
        walk[7] = sf::IntRect(484, 211, 50, 96);
        walk[8] = sf::IntRect(546, 211, 53, 96);
        walk[9] = sf::IntRect(611, 213, 60, 94);
        walk[10] = sf::IntRect(683, 216, 52, 91);
        walk[11] = sf::IntRect(747, 216, 46, 91);

        sf::IntRect attack[16];
        attack[0] = sf::IntRect(8, 371, 62, 89);
        attack[1] = sf::IntRect(82, 374, 64, 86);
        attack[2] = sf::IntRect(158, 374, 67, 86);
        attack[3] = sf::IntRect(237, 371, 75, 89);
        attack[4] = sf::IntRect(324, 366, 83, 94);
        attack[5] = sf::IntRect(419, 359, 96, 101);
        attack[6] = sf::IntRect(527, 366, 101, 94);
        attack[7] = sf::IntRect(640, 369, 102, 91);
        attack[8] = sf::IntRect(754, 359, 87, 101);
        attack[9] = sf::IntRect(853, 359, 78, 101);
        attack[10] = sf::IntRect(943, 376, 84, 84);
        attack[11] = sf::IntRect(1039, 378, 70, 82);
        attack[12] = sf::IntRect(1121, 381, 67, 79);
        attack[13] = sf::IntRect(1200, 374, 67, 86);
        attack[14] = sf::IntRect(1279, 374, 69, 86);
        attack[15] = sf::IntRect(1360, 371, 65, 89);

        Animation a;
        a.setup("idle", idle, 4, 10.f, true);  addAnimation("idle", a);
        a.setup("walk", walk, 12, 10.f, true);  addAnimation("walk", a);
        a.setup("attack", attack, 16, 10.f, false); addAnimation("attack", a);
    }
};

// ------------------------------------------------------------------ BazookaSoldier ---

class BazookaSoldier : public Infantry
{
public:
    BazookaSoldier() : Infantry(20, 2.0f, 80)
    {
        m_texture.loadFromFile("Sprites/bazookarebelsoldier.png");
        m_sprite.setTexture(m_texture);
        setupAnimations();
        setAnimation("idle");
    }

    void attack() override { setAnimation("attack"); }

    void move(float dt) override
    {
        if (m_state == 1)
        {
            velocityX = speed;
            setAnimation("walk");
        }
        else
        {
            velocityX = 0.f;
        }
    }

private:
    void setupAnimations()
    {
        sf::IntRect idle[6];
        idle[0] = sf::IntRect(3, 3, 36, 45);
        idle[1] = sf::IntRect(42, 3, 38, 45);
        idle[2] = sf::IntRect(83, 5, 38, 43);
        idle[3] = sf::IntRect(124, 6, 36, 42);
        idle[4] = sf::IntRect(163, 5, 38, 43);
        idle[5] = sf::IntRect(204, 3, 38, 45);

        sf::IntRect walk[16];
        walk[0] = sf::IntRect(3, 55, 41, 39);
        walk[1] = sf::IntRect(47, 54, 41, 40);
        walk[2] = sf::IntRect(91, 53, 40, 40);
        walk[3] = sf::IntRect(134, 54, 41, 39);
        walk[4] = sf::IntRect(178, 56, 40, 38);
        walk[5] = sf::IntRect(221, 55, 41, 39);
        walk[6] = sf::IntRect(265, 54, 41, 40);
        walk[7] = sf::IntRect(309, 53, 38, 41);
        walk[8] = sf::IntRect(350, 52, 37, 41);
        walk[9] = sf::IntRect(390, 51, 36, 42);
        walk[10] = sf::IntRect(429, 53, 37, 41);
        walk[11] = sf::IntRect(3, 102, 38, 40);
        walk[12] = sf::IntRect(46, 99, 38, 43);
        walk[13] = sf::IntRect(87, 98, 38, 44);
        walk[14] = sf::IntRect(128, 98, 37, 44);
        walk[15] = sf::IntRect(168, 97, 36, 45);

        sf::IntRect attack[5];
        attack[0] = sf::IntRect(3, 241, 39, 44);
        attack[1] = sf::IntRect(45, 243, 41, 42);
        attack[2] = sf::IntRect(89, 245, 41, 40);
        attack[3] = sf::IntRect(45, 243, 41, 42);
        attack[4] = sf::IntRect(3, 241, 39, 44);

        Animation a;
        a.setup("idle", idle, 6, 10.f, true);  addAnimation("idle", a);
        a.setup("walk", walk, 16, 10.f, true);  addAnimation("walk", a);
        a.setup("attack", attack, 5, 10.f, false); addAnimation("attack", a);
    }
};

// ------------------------------------------------------------------ Undead ---

class Undead : public Enemies
{
public:
    Undead(int dmg, float spd, int hp) : Enemies(dmg, spd, hp) {}

    void attack() override {}

    void move(float dt) override
    {
        if (m_state == 1)
        {
            velocityX = speed;
            setAnimation("walk");
        }
        else
        {
            velocityX = 0.f;
        }
    }
};

// ------------------------------------------------------------------ Zombie ---

class Zombie : public Undead
{
public:
    Zombie() : Undead(5, 2.0f, 60)
    {
        m_texture.loadFromFile("Sprites/zombieenemy.png");
        m_sprite.setTexture(m_texture);
        setupAnimations();
        setAnimation("idle");
    }

    void attack() override {}

private:
    void setupAnimations()
    {
        sf::IntRect idle[7];
        idle[0] = sf::IntRect(5, 4, 30, 39);
        idle[1] = sf::IntRect(39, 4, 30, 39);
        idle[2] = sf::IntRect(73, 4, 30, 39);
        idle[3] = sf::IntRect(106, 3, 31, 40);
        idle[4] = sf::IntRect(139, 3, 32, 40);
        idle[5] = sf::IntRect(176, 3, 33, 40);
        idle[6] = sf::IntRect(217, 3, 35, 40);

        sf::IntRect walk[16];
        walk[0] = sf::IntRect(7, 46, 28, 39);
        walk[1] = sf::IntRect(41, 46, 28, 39);
        walk[2] = sf::IntRect(73, 46, 30, 39);
        walk[3] = sf::IntRect(107, 46, 30, 39);
        walk[4] = sf::IntRect(141, 46, 30, 39);
        walk[5] = sf::IntRect(174, 46, 31, 39);
        walk[6] = sf::IntRect(213, 46, 26, 39);
        walk[7] = sf::IntRect(246, 46, 27, 39);
        walk[8] = sf::IntRect(281, 46, 26, 39);
        walk[9] = sf::IntRect(316, 46, 25, 39);
        walk[10] = sf::IntRect(343, 46, 25, 39);
        walk[11] = sf::IntRect(377, 47, 25, 38);
        walk[12] = sf::IntRect(411, 48, 28, 37);
        walk[13] = sf::IntRect(452, 47, 31, 38);
        walk[14] = sf::IntRect(3, 88, 27, 39);
        walk[15] = sf::IntRect(37, 88, 27, 39);

        Animation a;
        a.setup("idle", idle, 7, 10.f, true); addAnimation("idle", a);
        a.setup("walk", walk, 16, 10.f, true); addAnimation("walk", a);
    }
};

// ------------------------------------------------------------------ Mummy ---

class Mummy : public Undead
{
public:
    Mummy() : Undead(8, 2.0f, 80)
    {
        m_texture.loadFromFile("Sprites/mummyenemy.png");
        m_sprite.setTexture(m_texture);
        setupAnimations();
        setAnimation("idle");
    }

    void attack() override { setAnimation("attack"); }

private:
    void setupAnimations()
    {
        sf::IntRect idle[4];
        idle[0] = sf::IntRect(27, 1010, 30, 45);
        idle[1] = sf::IntRect(68, 1010, 30, 45);
        idle[2] = sf::IntRect(106, 1010, 29, 45);
        idle[3] = sf::IntRect(144, 1010, 29, 45);

        sf::IntRect walk[18];
        walk[0] = sf::IntRect(195, 1010, 31, 45);
        walk[1] = sf::IntRect(235, 1010, 31, 45);
        walk[2] = sf::IntRect(275, 1010, 31, 45);
        walk[3] = sf::IntRect(314, 1010, 33, 45);
        walk[4] = sf::IntRect(355, 1011, 31, 44);
        walk[5] = sf::IntRect(394, 1012, 31, 43);
        walk[6] = sf::IntRect(432, 1011, 31, 44);
        walk[7] = sf::IntRect(471, 1010, 31, 45);
        walk[8] = sf::IntRect(508, 1010, 32, 45);
        walk[9] = sf::IntRect(548, 1010, 32, 45);
        walk[10] = sf::IntRect(593, 1010, 31, 45);
        walk[11] = sf::IntRect(638, 1010, 31, 45);
        walk[12] = sf::IntRect(680, 1011, 33, 44);
        walk[13] = sf::IntRect(722, 1012, 36, 43);
        walk[14] = sf::IntRect(767, 1011, 33, 44);
        walk[15] = sf::IntRect(805, 1010, 31, 45);
        walk[16] = sf::IntRect(844, 1010, 31, 45);
        walk[17] = sf::IntRect(883, 1010, 31, 45);

        sf::IntRect attack[28];
        attack[0] = sf::IntRect(13, 1074, 28, 45);
        attack[1] = sf::IntRect(49, 1074, 27, 45);
        attack[2] = sf::IntRect(87, 1074, 23, 45);
        attack[3] = sf::IntRect(118, 1075, 23, 44);
        attack[4] = sf::IntRect(148, 1074, 29, 45);
        attack[5] = sf::IntRect(185, 1073, 25, 46);
        attack[6] = sf::IntRect(217, 1073, 25, 46);
        attack[7] = sf::IntRect(251, 1074, 26, 45);
        attack[8] = sf::IntRect(286, 1075, 27, 44);
        attack[9] = sf::IntRect(320, 1076, 28, 43);
        attack[10] = sf::IntRect(354, 1077, 28, 42);
        attack[11] = sf::IntRect(387, 1078, 28, 41);
        attack[12] = sf::IntRect(421, 1078, 29, 41);
        attack[13] = sf::IntRect(456, 1078, 30, 41);
        attack[14] = sf::IntRect(492, 1078, 30, 41);
        attack[15] = sf::IntRect(530, 1077, 30, 42);
        attack[16] = sf::IntRect(567, 1076, 29, 43);
        attack[17] = sf::IntRect(602, 1076, 28, 43);
        attack[18] = sf::IntRect(637, 1076, 28, 43);
        attack[19] = sf::IntRect(671, 1076, 27, 43);
        attack[20] = sf::IntRect(705, 1075, 27, 44);
        attack[21] = sf::IntRect(738, 1074, 27, 45);
        attack[22] = sf::IntRect(774, 1074, 26, 45);
        attack[23] = sf::IntRect(809, 1074, 26, 45);
        attack[24] = sf::IntRect(842, 1074, 25, 45);
        attack[25] = sf::IntRect(876, 1074, 23, 45);
        attack[26] = sf::IntRect(908, 1074, 21, 45);
        attack[27] = sf::IntRect(937, 1074, 28, 45);

        Animation a;
        a.setup("idle", idle, 4, 10.f, true);  addAnimation("idle", a);
        a.setup("walk", walk, 18, 10.f, true);  addAnimation("walk", a);
        a.setup("attack", attack, 28, 10.f, false); addAnimation("attack", a);
    }
};

// ------------------------------------------------------------------ Marine ---

class Marine : public Enemies
{
public:
    Marine() : Enemies(15, 2.0f, 100)
    {
        m_texture.loadFromFile("Sprites/marineenemy.png");
        m_sprite.setTexture(m_texture);
        setupAnimations();
        setAnimation("idle");
    }

    void attack() override { setAnimation("attack"); }

    void move(float dt) override
    {
        if (m_state == 1)
        {
            velocityX = speed;
            setAnimation("walk");
        }
        else
        {
            velocityX = 0.f;
        }
    }

private:
    void setupAnimations()
    {
        sf::IntRect idle[16];
        idle[0] = sf::IntRect(10, 18, 49, 43);
        idle[1] = sf::IntRect(64, 18, 49, 43);
        idle[2] = sf::IntRect(118, 19, 48, 42);
        idle[3] = sf::IntRect(171, 19, 48, 42);
        idle[4] = sf::IntRect(224, 19, 48, 42);
        idle[5] = sf::IntRect(277, 19, 48, 42);
        idle[6] = sf::IntRect(330, 19, 48, 42);
        idle[7] = sf::IntRect(383, 19, 48, 42);
        idle[8] = sf::IntRect(436, 19, 48, 42);
        idle[9] = sf::IntRect(489, 19, 48, 42);
        idle[10] = sf::IntRect(542, 19, 48, 42);
        idle[11] = sf::IntRect(595, 19, 48, 42);
        idle[12] = sf::IntRect(648, 19, 48, 42);
        idle[13] = sf::IntRect(701, 19, 48, 42);
        idle[14] = sf::IntRect(754, 19, 48, 42);
        idle[15] = sf::IntRect(807, 19, 48, 42);

        sf::IntRect walk[16];
        walk[0] = sf::IntRect(10, 79, 49, 43);
        walk[1] = sf::IntRect(64, 79, 50, 43);
        walk[2] = sf::IntRect(119, 80, 50, 42);
        walk[3] = sf::IntRect(174, 80, 51, 42);
        walk[4] = sf::IntRect(230, 80, 52, 42);
        walk[5] = sf::IntRect(287, 80, 51, 42);
        walk[6] = sf::IntRect(343, 80, 51, 42);
        walk[7] = sf::IntRect(399, 80, 52, 42);
        walk[8] = sf::IntRect(456, 80, 52, 42);
        walk[9] = sf::IntRect(513, 80, 52, 42);
        walk[10] = sf::IntRect(570, 80, 52, 42);
        walk[11] = sf::IntRect(627, 80, 50, 42);
        walk[12] = sf::IntRect(683, 80, 48, 42);
        walk[13] = sf::IntRect(736, 80, 48, 42);
        walk[14] = sf::IntRect(789, 80, 48, 42);
        walk[15] = sf::IntRect(842, 80, 48, 42);

        sf::IntRect attack[18];
        attack[0] = sf::IntRect(10, 309, 49, 43);
        attack[1] = sf::IntRect(64, 309, 49, 43);
        attack[2] = sf::IntRect(118, 309, 47, 43);
        attack[3] = sf::IntRect(170, 309, 47, 43);
        attack[4] = sf::IntRect(222, 309, 46, 43);
        attack[5] = sf::IntRect(273, 309, 46, 43);
        attack[6] = sf::IntRect(324, 309, 45, 43);
        attack[7] = sf::IntRect(374, 309, 44, 43);
        attack[8] = sf::IntRect(423, 309, 53, 43);
        attack[9] = sf::IntRect(481, 313, 55, 39);
        attack[10] = sf::IntRect(541, 314, 54, 38);
        attack[11] = sf::IntRect(10, 357, 69, 38);
        attack[12] = sf::IntRect(84, 358, 70, 37);
        attack[13] = sf::IntRect(167, 359, 67, 36);
        attack[14] = sf::IntRect(262, 360, 55, 35);
        attack[15] = sf::IntRect(322, 359, 52, 36);
        attack[16] = sf::IntRect(379, 358, 53, 37);
        attack[17] = sf::IntRect(10, 400, 53, 42);

        Animation a;
        a.setup("idle", idle, 16, 10.f, true);  addAnimation("idle", a);
        a.setup("walk", walk, 16, 10.f, true);  addAnimation("walk", a);
        a.setup("attack", attack, 18, 10.f, false); addAnimation("attack", a);
    }
};

// ------------------------------------------------------------------ Factory ---

Enemies* Enemies::createEnemy(EnemyType type)
{
    switch (type)
    {
    case EnemyType::RebelSoldier:   return new RebelSoldier();
    case EnemyType::ShieldedSoldier:return new ShieldedSoldier();
    case EnemyType::GrenadeSoldier: return new GrenadeSoldier();
    case EnemyType::BazookaSoldier: return new BazookaSoldier();
    case EnemyType::Zombie:         return new Zombie();
    case EnemyType::Mummy:          return new Mummy();
    case EnemyType::Marine:         return new Marine();
    default:                        return nullptr;
    }
}

#include "Enemy.h"
#include "Animation.h"
#include <cstdlib>

EnemyAI::EnemyAI() : m_stateTimer(0.f) {}

void EnemyAI::reset()
{
    m_stateTimer = 0.f;
}

void EnemyAI::update(float dt, int& stateOut, const Animation* currentAnim)
{
    // Don't interrupt a non-looping animation (e.g. attack) until it finishes
    if (currentAnim &&
        !currentAnim->isLooping() &&
        !currentAnim->isFinished())
        return;

    m_stateTimer += dt;
    if (m_stateTimer > 0.2f)
    {
        m_stateTimer = 0.f;

        int r = rand() % 3;
        if (r == 0) stateOut = 0;   // idle
        else if (r == 1) stateOut = 1;   // walk
        else             stateOut = 2;   // attack
    }
}

// ------------------------------------------------------------------ helpers ---

static bool isTileE(char** lvl, int x, int y, int mapWidth, int mapHeight)
{
    if (x < 0 || x >= mapWidth || y < 0 || y >= mapHeight)
        return false;
    return lvl[y][x] == 'g';
}

// ------------------------------------------------------------------ base ---

Enemies::Enemies(int dmg, float spd, int hp)
{
    damage = dmg;
    speed = spd;
    health = hp;
    alive = true;
    m_state = 0;
    m_animCount = 0;
    m_currentAnimation = nullptr;
    facingRight = false;   // enemy sprites face LEFT by default
    baseScale = 1.f;
    m_sprite.setPosition(0.f, 0.f);
}

Enemies::~Enemies() {}

void Enemies::copyName(char* dest, const char* src, int maxLen)
{
    int i = 0;
    for (; i < maxLen - 1 && src[i] != '\0'; i++)
        dest[i] = src[i];
    dest[i] = '\0';
}

void Enemies::addAnimation(const char* name, const Animation& anim)
{
    if (m_animCount >= 10) return;
    copyName(m_animNames[m_animCount], name, 32);
    m_animations[m_animCount] = anim;
    m_animCount++;
}

void Enemies::setAnimation(const char* name)
{
    for (int i = 0; i < m_animCount; i++)
    {
        if (std::strcmp(m_animNames[i], name) == 0)
        {
            if (m_currentAnimation == &m_animations[i])
                return;                         // already playing

            m_currentAnimation = &m_animations[i];
            m_currentAnimation->reset();
            m_currentAnimation->play();
            return;
        }
    }
}

Animation* Enemies::getCurrentAnimation() { return m_currentAnimation; }
sf::Sprite& Enemies::getSprite() { return m_sprite; }
bool        Enemies::isAlive() const { return alive; }

// ------------------------------------------------------------------ AI (delegates to m_ai) ---

void Enemies::updateAI(float dt)
{
    m_ai.update(dt, m_state, m_currentAnimation);

    // Trigger the right animation for idle / attack states.
    // Walk animation is handled inside move() so it only plays when actually moving.
    if (m_state == 0) setAnimation("idle");
    if (m_state == 2) setAnimation("attack");
}

// ------------------------------------------------------------------ physics ---

void Enemies::physicsUpdate(float dt, char** lvl, int cell_size, int mapWidth, int mapHeight)
{
    // gravity
    if (!onGround)
    {
        velocityY += gravity;
        if (velocityY > 20.f) velocityY = 20.f;
    }
    else
    {
        velocityY = 0.f;
    }

    float bx = m_sprite.getPosition().x + collisionOffsetX;
    float by = m_sprite.getPosition().y + collisionOffsetY;
    float bw = collisionWidth;
    float bh = collisionHeight;

    // move X
    m_sprite.move(velocityX, 0.f);
    bx = m_sprite.getPosition().x + collisionOffsetX;

    int L = std::max(0, (int)(bx / cell_size));
    int R = std::min(mapWidth - 1, (int)((bx + bw - 1) / cell_size));
    int T = std::max(0, (int)(by / cell_size));
    int B = std::min(mapHeight - 1, (int)((by + bh - 1) / cell_size));

    if (velocityX > 0.f)
    {
        for (int i = T; i <= B; i++)
        {
            if (isTileE(lvl, R, i, mapWidth, mapHeight))
            {
                m_sprite.setPosition(R * cell_size - bw - collisionOffsetX,
                    m_sprite.getPosition().y);
                velocityX = 0.f;
                bx = m_sprite.getPosition().x + collisionOffsetX;
                break;
            }
        }
    }
    else if (velocityX < 0.f)
    {
        for (int i = T; i <= B; i++)
        {
            if (isTileE(lvl, L, i, mapWidth, mapHeight))
            {
                m_sprite.setPosition((L + 1) * cell_size - collisionOffsetX,
                    m_sprite.getPosition().y);
                velocityX = 0.f;
                bx = m_sprite.getPosition().x + collisionOffsetX;
                break;
            }
        }
    }

    // move Y
    m_sprite.move(0.f, velocityY);
    by = m_sprite.getPosition().y + collisionOffsetY;

    L = std::max(0, (int)(bx / cell_size));
    R = std::min(mapWidth - 1, (int)((bx + bw - 1) / cell_size));
    T = std::max(0, (int)(by / cell_size));
    B = std::min(mapHeight - 1, (int)((by + bh - 1) / cell_size));

    onGround = false;

    if (velocityY >= 0.f)
    {
        for (int j = L; j <= R; j++)
        {
            if (isTileE(lvl, j, B, mapWidth, mapHeight))
            {
                m_sprite.setPosition(m_sprite.getPosition().x,
                    B * cell_size - bh - collisionOffsetY);
                velocityY = 0.f;
                onGround = true;
                break;
            }
        }
    }
    else
    {
        for (int j = L; j <= R; j++)
        {
            if (isTileE(lvl, j, T, mapWidth, mapHeight))
            {
                m_sprite.setPosition(m_sprite.getPosition().x,
                    (T + 1) * cell_size - collisionOffsetY);
                velocityY = 0.f;
                break;
            }
        }
    }

    // clamp to map
    bx = m_sprite.getPosition().x + collisionOffsetX;
    by = m_sprite.getPosition().y + collisionOffsetY;

    float cx = bx, cy = by;
    if (cx < 0.f)                             cx = 0.f;
    else if (cx > mapWidth * cell_size - bw) cx = mapWidth * cell_size - bw;
    if (cy < 0.f)                             cy = 0.f;
    else if (cy > mapHeight * cell_size - bh)
    {
        cy = mapHeight * cell_size - bh;
        velocityY = 0.f;
        onGround = true;
    }

    m_sprite.setPosition(cx - collisionOffsetX, cy - collisionOffsetY);
}

// ------------------------------------------------------------------ update ---

void Enemies::update(float dt, char** lvl, int cell_size, int mapWidth, int mapHeight)
{
    updateAI(dt);          // m_ai decides state → sets idle/attack animation
    move(dt);              // subclass sets velocityX and facing direction
    physicsUpdate(dt, lvl, cell_size, mapWidth, mapHeight);

    // flip sprite to match facing direction
    // Sprites naturally face LEFT, so facingRight = true means we flip on X.
    // Origin stays top-left; the negative scale flips around that corner.
    // If sprites appear offset when flipped, set the origin to the sprite's
    // centre: m_sprite.setOrigin(width/2, 0) after loading the texture.
    if (facingRight)
        m_sprite.setScale(-baseScale, baseScale);
    else
        m_sprite.setScale(baseScale, baseScale);

    if (m_currentAnimation)
    {
        m_currentAnimation->update(dt);
        m_sprite.setTextureRect(m_currentAnimation->getCurrentFrame());
    }
}

// ------------------------------------------------------------------ Infantry ---

class Infantry : public Enemies
{
public:
    Infantry(int dmg, float spd, int hp) : Enemies(dmg, spd, hp) {}
    virtual void move(float dt) = 0;
};

// ------------------------------------------------------------------ RebelSoldier ---

class RebelSoldier : public Infantry
{
public:
    RebelSoldier() : Infantry(10, 2.0f, 120)
    {
        m_texture.loadFromFile("Sprites/normalrebelsoldier.png");
        m_sprite.setTexture(m_texture);
        setupAnimations();
        setAnimation("idle");
    }

    void attack() override { setAnimation("attack"); }

    void move(float dt) override
    {
        if (m_state == 1)
        {
            velocityX = -speed;   // move LEFT toward player
            facingRight = false;
            setAnimation("walk");
        }
        else
        {
            velocityX = 0.f;
        }
    }

private:
    void setupAnimations()
    {
        sf::IntRect idle[4];
        idle[0] = sf::IntRect(3, 3, 40, 38);
        idle[1] = sf::IntRect(46, 3, 40, 38);
        idle[2] = sf::IntRect(131, 3, 40, 38);
        idle[3] = sf::IntRect(89, 4, 39, 37);

        sf::IntRect walk[15];
        walk[0] = sf::IntRect(3, 44, 34, 37);
        walk[1] = sf::IntRect(40, 44, 37, 37);
        walk[2] = sf::IntRect(80, 45, 42, 31);
        walk[3] = sf::IntRect(125, 46, 42, 33);
        walk[4] = sf::IntRect(170, 45, 37, 35);
        walk[5] = sf::IntRect(210, 44, 35, 37);
        walk[6] = sf::IntRect(131, 3, 40, 38);
        walk[7] = sf::IntRect(248, 44, 34, 37);
        walk[8] = sf::IntRect(285, 44, 38, 36);
        walk[9] = sf::IntRect(326, 45, 40, 30);
        walk[10] = sf::IntRect(369, 46, 38, 32);
        walk[11] = sf::IntRect(410, 45, 37, 35);
        walk[12] = sf::IntRect(450, 44, 35, 37);
        walk[13] = sf::IntRect(3, 85, 39, 37);
        walk[14] = sf::IntRect(45, 84, 40, 38);

        sf::IntRect attack[10];
        attack[0] = sf::IntRect(3, 298, 44, 38);
        attack[1] = sf::IntRect(50, 298, 48, 38);
        attack[2] = sf::IntRect(101, 298, 47, 38);
        attack[3] = sf::IntRect(151, 297, 42, 39);
        attack[4] = sf::IntRect(196, 289, 41, 47);
        attack[5] = sf::IntRect(240, 289, 41, 47);
        attack[6] = sf::IntRect(284, 292, 43, 44);
        attack[7] = sf::IntRect(330, 298, 47, 38);
        attack[8] = sf::IntRect(380, 298, 46, 38);
        attack[9] = sf::IntRect(429, 298, 40, 38);

        Animation a;
        a.setup("idle", idle, 4, 10.f, true);  addAnimation("idle", a);
        a.setup("walk", walk, 15, 10.f, true);  addAnimation("walk", a);
        a.setup("attack", attack, 10, 10.f, false); addAnimation("attack", a);
    }
};

// ------------------------------------------------------------------ ShieldedSoldier ---

class ShieldedSoldier : public Infantry
{
public:
    ShieldedSoldier() : Infantry(10, 2.0f, 120)
    {
        m_texture.loadFromFile("Sprites/shieldedrebelsoldier.png");
        m_sprite.setTexture(m_texture);
        setupAnimations();
        setAnimation("idle");
    }

    void attack() override { setAnimation("attack"); }

    void move(float dt) override
    {
        if (m_state == 1)
        {
            velocityX = -speed;
            facingRight = false;
            setAnimation("walk");
        }
        else
        {
            velocityX = 0.f;
        }
    }

private:
    void setupAnimations()
    {
        sf::IntRect idle[6];
        idle[0] = sf::IntRect(3, 5, 32, 38);
        idle[1] = sf::IntRect(38, 5, 32, 38);
        idle[2] = sf::IntRect(73, 5, 31, 38);
        idle[3] = sf::IntRect(107, 4, 32, 39);
        idle[4] = sf::IntRect(143, 5, 31, 38);
        idle[5] = sf::IntRect(177, 5, 32, 38);

        sf::IntRect walk[16];
        walk[0] = sf::IntRect(3, 47, 29, 40);
        walk[1] = sf::IntRect(35, 46, 28, 41);
        walk[2] = sf::IntRect(66, 46, 30, 40);
        walk[3] = sf::IntRect(99, 47, 31, 39);
        walk[4] = sf::IntRect(133, 49, 31, 38);
        walk[5] = sf::IntRect(167, 48, 32, 39);
        walk[6] = sf::IntRect(202, 47, 30, 40);
        walk[7] = sf::IntRect(235, 46, 29, 41);
        walk[8] = sf::IntRect(267, 46, 28, 40);
        walk[9] = sf::IntRect(298, 47, 28, 39);
        walk[10] = sf::IntRect(329, 49, 27, 38);
        walk[11] = sf::IntRect(359, 48, 27, 39);
        walk[12] = sf::IntRect(3, 90, 32, 38);
        walk[13] = sf::IntRect(38, 90, 32, 38);
        walk[14] = sf::IntRect(73, 90, 36, 38);
        walk[15] = sf::IntRect(112, 90, 32, 38);

        sf::IntRect attack[10];
        attack[0] = sf::IntRect(3, 305, 30, 38);
        attack[1] = sf::IntRect(36, 305, 28, 38);
        attack[2] = sf::IntRect(67, 305, 31, 38);
        attack[3] = sf::IntRect(101, 305, 31, 38);
        attack[4] = sf::IntRect(135, 305, 38, 38);
        attack[5] = sf::IntRect(176, 305, 37, 38);
        attack[6] = sf::IntRect(216, 305, 35, 38);
        attack[7] = sf::IntRect(254, 305, 34, 38);
        attack[8] = sf::IntRect(291, 305, 35, 38);
        attack[9] = sf::IntRect(329, 302, 37, 41);

        Animation a;
        a.setup("idle", idle, 6, 10.f, true);  addAnimation("idle", a);
        a.setup("walk", walk, 16, 10.f, true);  addAnimation("walk", a);
        a.setup("attack", attack, 10, 10.f, false); addAnimation("attack", a);
    }
};

// ------------------------------------------------------------------ GrenadeSoldier ---

class GrenadeSoldier : public Infantry
{
public:
    GrenadeSoldier() : Infantry(20, 1.5f, 80)
    {
        m_texture.loadFromFile("Sprites/grenaderebelsoldier.png");
        m_sprite.setTexture(m_texture);
        setupAnimations();
        setAnimation("idle");
    }

    void attack() override { setAnimation("attack"); }

    void move(float dt) override
    {
        if (m_state == 1)
        {
            velocityX = -speed;
            facingRight = false;
            setAnimation("walk");
        }
        else
        {
            velocityX = 0.f;
        }
    }

private:
    void setupAnimations()
    {
        sf::IntRect idle[4];
        idle[0] = sf::IntRect(18, 63, 64, 91);
        idle[1] = sf::IntRect(94, 66, 65, 88);
        idle[2] = sf::IntRect(171, 68, 62, 86);
        idle[3] = sf::IntRect(245, 70, 57, 84);

        sf::IntRect walk[12];
        walk[0] = sf::IntRect(25, 216, 48, 91);
        walk[1] = sf::IntRect(85, 211, 47, 96);
        walk[2] = sf::IntRect(144, 211, 55, 96);
        walk[3] = sf::IntRect(211, 213, 63, 94);
        walk[4] = sf::IntRect(286, 216, 57, 91);
        walk[5] = sf::IntRect(355, 216, 53, 91);
        walk[6] = sf::IntRect(420, 216, 50, 91);
        walk[7] = sf::IntRect(484, 211, 50, 96);
        walk[8] = sf::IntRect(546, 211, 53, 96);
        walk[9] = sf::IntRect(611, 213, 60, 94);
        walk[10] = sf::IntRect(683, 216, 52, 91);
        walk[11] = sf::IntRect(747, 216, 46, 91);

        sf::IntRect attack[16];
        attack[0] = sf::IntRect(8, 371, 62, 89);
        attack[1] = sf::IntRect(82, 374, 64, 86);
        attack[2] = sf::IntRect(158, 374, 67, 86);
        attack[3] = sf::IntRect(237, 371, 75, 89);
        attack[4] = sf::IntRect(324, 366, 83, 94);
        attack[5] = sf::IntRect(419, 359, 96, 101);
        attack[6] = sf::IntRect(527, 366, 101, 94);
        attack[7] = sf::IntRect(640, 369, 102, 91);
        attack[8] = sf::IntRect(754, 359, 87, 101);
        attack[9] = sf::IntRect(853, 359, 78, 101);
        attack[10] = sf::IntRect(943, 376, 84, 84);
        attack[11] = sf::IntRect(1039, 378, 70, 82);
        attack[12] = sf::IntRect(1121, 381, 67, 79);
        attack[13] = sf::IntRect(1200, 374, 67, 86);
        attack[14] = sf::IntRect(1279, 374, 69, 86);
        attack[15] = sf::IntRect(1360, 371, 65, 89);

        Animation a;
        a.setup("idle", idle, 4, 10.f, true);  addAnimation("idle", a);
        a.setup("walk", walk, 12, 10.f, true);  addAnimation("walk", a);
        a.setup("attack", attack, 16, 10.f, false); addAnimation("attack", a);
    }
};

// ------------------------------------------------------------------ BazookaSoldier ---

class BazookaSoldier : public Infantry
{
public:
    BazookaSoldier() : Infantry(20, 1.5f, 80)
    {
        m_texture.loadFromFile("Sprites/bazookarebelsoldier.png");
        m_sprite.setTexture(m_texture);
        setupAnimations();
        setAnimation("idle");
    }

    void attack() override { setAnimation("attack"); }

    void move(float dt) override
    {
        if (m_state == 1)
        {
            velocityX = -speed;
            facingRight = false;
            setAnimation("walk");
        }
        else
        {
            velocityX = 0.f;
        }
    }

private:
    void setupAnimations()
    {
        sf::IntRect idle[6];
        idle[0] = sf::IntRect(3, 3, 36, 45);
        idle[1] = sf::IntRect(42, 3, 38, 45);
        idle[2] = sf::IntRect(83, 5, 38, 43);
        idle[3] = sf::IntRect(124, 6, 36, 42);
        idle[4] = sf::IntRect(163, 5, 38, 43);
        idle[5] = sf::IntRect(204, 3, 38, 45);

        sf::IntRect walk[16];
        walk[0] = sf::IntRect(3, 55, 41, 39);
        walk[1] = sf::IntRect(47, 54, 41, 40);
        walk[2] = sf::IntRect(91, 53, 40, 40);
        walk[3] = sf::IntRect(134, 54, 41, 39);
        walk[4] = sf::IntRect(178, 56, 40, 38);
        walk[5] = sf::IntRect(221, 55, 41, 39);
        walk[6] = sf::IntRect(265, 54, 41, 40);
        walk[7] = sf::IntRect(309, 53, 38, 41);
        walk[8] = sf::IntRect(350, 52, 37, 41);
        walk[9] = sf::IntRect(390, 51, 36, 42);
        walk[10] = sf::IntRect(429, 53, 37, 41);
        walk[11] = sf::IntRect(3, 102, 38, 40);
        walk[12] = sf::IntRect(46, 99, 38, 43);
        walk[13] = sf::IntRect(87, 98, 38, 44);
        walk[14] = sf::IntRect(128, 98, 37, 44);
        walk[15] = sf::IntRect(168, 97, 36, 45);

        sf::IntRect attack[5];
        attack[0] = sf::IntRect(3, 241, 39, 44);
        attack[1] = sf::IntRect(45, 243, 41, 42);
        attack[2] = sf::IntRect(89, 245, 41, 40);
        attack[3] = sf::IntRect(45, 243, 41, 42);
        attack[4] = sf::IntRect(3, 241, 39, 44);

        Animation a;
        a.setup("idle", idle, 6, 10.f, true);  addAnimation("idle", a);
        a.setup("walk", walk, 16, 10.f, true);  addAnimation("walk", a);
        a.setup("attack", attack, 5, 10.f, false); addAnimation("attack", a);
    }
};

// ------------------------------------------------------------------ Undead ---

class Undead : public Enemies
{
public:
    Undead(int dmg, float spd, int hp) : Enemies(dmg, spd, hp) {}

    void attack() override {}

    void move(float dt) override
    {
        if (m_state == 1)
        {
            velocityX = -speed;
            facingRight = false;
            setAnimation("walk");
        }
        else
        {
            velocityX = 0.f;
        }
    }
};

// ------------------------------------------------------------------ Zombie ---

class Zombie : public Undead
{
public:
    Zombie() : Undead(5, 1.0f, 60)
    {
        m_texture.loadFromFile("Sprites/zombieenemy.png");
        m_sprite.setTexture(m_texture);
        setupAnimations();
        setAnimation("idle");
    }

    void attack() override {}

private:
    void setupAnimations()
    {
        sf::IntRect idle[7];
        idle[0] = sf::IntRect(5, 4, 30, 39);
        idle[1] = sf::IntRect(39, 4, 30, 39);
        idle[2] = sf::IntRect(73, 4, 30, 39);
        idle[3] = sf::IntRect(106, 3, 31, 40);
        idle[4] = sf::IntRect(139, 3, 32, 40);
        idle[5] = sf::IntRect(176, 3, 33, 40);
        idle[6] = sf::IntRect(217, 3, 35, 40);

        sf::IntRect walk[16];
        walk[0] = sf::IntRect(7, 46, 28, 39);
        walk[1] = sf::IntRect(41, 46, 28, 39);
        walk[2] = sf::IntRect(73, 46, 30, 39);
        walk[3] = sf::IntRect(107, 46, 30, 39);
        walk[4] = sf::IntRect(141, 46, 30, 39);
        walk[5] = sf::IntRect(174, 46, 31, 39);
        walk[6] = sf::IntRect(213, 46, 26, 39);
        walk[7] = sf::IntRect(246, 46, 27, 39);
        walk[8] = sf::IntRect(281, 46, 26, 39);
        walk[9] = sf::IntRect(316, 46, 25, 39);
        walk[10] = sf::IntRect(343, 46, 25, 39);
        walk[11] = sf::IntRect(377, 47, 25, 38);
        walk[12] = sf::IntRect(411, 48, 28, 37);
        walk[13] = sf::IntRect(452, 47, 31, 38);
        walk[14] = sf::IntRect(3, 88, 27, 39);
        walk[15] = sf::IntRect(37, 88, 27, 39);

        Animation a;
        a.setup("idle", idle, 7, 10.f, true); addAnimation("idle", a);
        a.setup("walk", walk, 16, 10.f, true); addAnimation("walk", a);
    }
};

// ------------------------------------------------------------------ Mummy ---

class Mummy : public Undead
{
public:
    Mummy() : Undead(8, 0.8f, 80)
    {
        m_texture.loadFromFile("Sprites/mummyenemy.png");
        m_sprite.setTexture(m_texture);
        setupAnimations();
        setAnimation("idle");
    }

    void attack() override { setAnimation("attack"); }

private:
    void setupAnimations()
    {
        sf::IntRect idle[4];
        idle[0] = sf::IntRect(27, 1010, 30, 45);
        idle[1] = sf::IntRect(68, 1010, 30, 45);
        idle[2] = sf::IntRect(106, 1010, 29, 45);
        idle[3] = sf::IntRect(144, 1010, 29, 45);

        sf::IntRect walk[18];
        walk[0] = sf::IntRect(195, 1010, 31, 45);
        walk[1] = sf::IntRect(235, 1010, 31, 45);
        walk[2] = sf::IntRect(275, 1010, 31, 45);
        walk[3] = sf::IntRect(314, 1010, 33, 45);
        walk[4] = sf::IntRect(355, 1011, 31, 44);
        walk[5] = sf::IntRect(394, 1012, 31, 43);
        walk[6] = sf::IntRect(432, 1011, 31, 44);
        walk[7] = sf::IntRect(471, 1010, 31, 45);
        walk[8] = sf::IntRect(508, 1010, 32, 45);
        walk[9] = sf::IntRect(548, 1010, 32, 45);
        walk[10] = sf::IntRect(593, 1010, 31, 45);
        walk[11] = sf::IntRect(638, 1010, 31, 45);
        walk[12] = sf::IntRect(680, 1011, 33, 44);
        walk[13] = sf::IntRect(722, 1012, 36, 43);
        walk[14] = sf::IntRect(767, 1011, 33, 44);
        walk[15] = sf::IntRect(805, 1010, 31, 45);
        walk[16] = sf::IntRect(844, 1010, 31, 45);
        walk[17] = sf::IntRect(883, 1010, 31, 45);

        sf::IntRect attack[28];
        attack[0] = sf::IntRect(13, 1074, 28, 45);
        attack[1] = sf::IntRect(49, 1074, 27, 45);
        attack[2] = sf::IntRect(87, 1074, 23, 45);
        attack[3] = sf::IntRect(118, 1075, 23, 44);
        attack[4] = sf::IntRect(148, 1074, 29, 45);
        attack[5] = sf::IntRect(185, 1073, 25, 46);
        attack[6] = sf::IntRect(217, 1073, 25, 46);
        attack[7] = sf::IntRect(251, 1074, 26, 45);
        attack[8] = sf::IntRect(286, 1075, 27, 44);
        attack[9] = sf::IntRect(320, 1076, 28, 43);
        attack[10] = sf::IntRect(354, 1077, 28, 42);
        attack[11] = sf::IntRect(387, 1078, 28, 41);
        attack[12] = sf::IntRect(421, 1078, 29, 41);
        attack[13] = sf::IntRect(456, 1078, 30, 41);
        attack[14] = sf::IntRect(492, 1078, 30, 41);
        attack[15] = sf::IntRect(530, 1077, 30, 42);
        attack[16] = sf::IntRect(567, 1076, 29, 43);
        attack[17] = sf::IntRect(602, 1076, 28, 43);
        attack[18] = sf::IntRect(637, 1076, 28, 43);
        attack[19] = sf::IntRect(671, 1076, 27, 43);
        attack[20] = sf::IntRect(705, 1075, 27, 44);
        attack[21] = sf::IntRect(738, 1074, 27, 45);
        attack[22] = sf::IntRect(774, 1074, 26, 45);
        attack[23] = sf::IntRect(809, 1074, 26, 45);
        attack[24] = sf::IntRect(842, 1074, 25, 45);
        attack[25] = sf::IntRect(876, 1074, 23, 45);
        attack[26] = sf::IntRect(908, 1074, 21, 45);
        attack[27] = sf::IntRect(937, 1074, 28, 45);

        Animation a;
        a.setup("idle", idle, 4, 10.f, true);  addAnimation("idle", a);
        a.setup("walk", walk, 18, 10.f, true);  addAnimation("walk", a);
        a.setup("attack", attack, 28, 10.f, false); addAnimation("attack", a);
    }
};

// ------------------------------------------------------------------ Marine ---

class Marine : public Enemies
{
public:
    Marine() : Enemies(15, 3.0f, 100)
    {
        m_texture.loadFromFile("Sprites/marineenemy.png");
        m_sprite.setTexture(m_texture);
        setupAnimations();
        setAnimation("idle");
    }

    void attack() override { setAnimation("attack"); }

    void move(float dt) override
    {
        if (m_state == 1)
        {
            velocityX = -speed;
            facingRight = false;
            setAnimation("walk");
        }
        else
        {
            velocityX = 0.f;
        }
    }

private:
    void setupAnimations()
    {
        sf::IntRect idle[16];
        idle[0] = sf::IntRect(10, 18, 49, 43);
        idle[1] = sf::IntRect(64, 18, 49, 43);
        idle[2] = sf::IntRect(118, 19, 48, 42);
        idle[3] = sf::IntRect(171, 19, 48, 42);
        idle[4] = sf::IntRect(224, 19, 48, 42);
        idle[5] = sf::IntRect(277, 19, 48, 42);
        idle[6] = sf::IntRect(330, 19, 48, 42);
        idle[7] = sf::IntRect(383, 19, 48, 42);
        idle[8] = sf::IntRect(436, 19, 48, 42);
        idle[9] = sf::IntRect(489, 19, 48, 42);
        idle[10] = sf::IntRect(542, 19, 48, 42);
        idle[11] = sf::IntRect(595, 19, 48, 42);
        idle[12] = sf::IntRect(648, 19, 48, 42);
        idle[13] = sf::IntRect(701, 19, 48, 42);
        idle[14] = sf::IntRect(754, 19, 48, 42);
        idle[15] = sf::IntRect(807, 19, 48, 42);

        sf::IntRect walk[16];
        walk[0] = sf::IntRect(10, 79, 49, 43);
        walk[1] = sf::IntRect(64, 79, 50, 43);
        walk[2] = sf::IntRect(119, 80, 50, 42);
        walk[3] = sf::IntRect(174, 80, 51, 42);
        walk[4] = sf::IntRect(230, 80, 52, 42);
        walk[5] = sf::IntRect(287, 80, 51, 42);
        walk[6] = sf::IntRect(343, 80, 51, 42);
        walk[7] = sf::IntRect(399, 80, 52, 42);
        walk[8] = sf::IntRect(456, 80, 52, 42);
        walk[9] = sf::IntRect(513, 80, 52, 42);
        walk[10] = sf::IntRect(570, 80, 52, 42);
        walk[11] = sf::IntRect(627, 80, 50, 42);
        walk[12] = sf::IntRect(683, 80, 48, 42);
        walk[13] = sf::IntRect(736, 80, 48, 42);
        walk[14] = sf::IntRect(789, 80, 48, 42);
        walk[15] = sf::IntRect(842, 80, 48, 42);

        sf::IntRect attack[18];
        attack[0] = sf::IntRect(10, 309, 49, 43);
        attack[1] = sf::IntRect(64, 309, 49, 43);
        attack[2] = sf::IntRect(118, 309, 47, 43);
        attack[3] = sf::IntRect(170, 309, 47, 43);
        attack[4] = sf::IntRect(222, 309, 46, 43);
        attack[5] = sf::IntRect(273, 309, 46, 43);
        attack[6] = sf::IntRect(324, 309, 45, 43);
        attack[7] = sf::IntRect(374, 309, 44, 43);
        attack[8] = sf::IntRect(423, 309, 53, 43);
        attack[9] = sf::IntRect(481, 313, 55, 39);
        attack[10] = sf::IntRect(541, 314, 54, 38);
        attack[11] = sf::IntRect(10, 357, 69, 38);
        attack[12] = sf::IntRect(84, 358, 70, 37);
        attack[13] = sf::IntRect(167, 359, 67, 36);
        attack[14] = sf::IntRect(262, 360, 55, 35);
        attack[15] = sf::IntRect(322, 359, 52, 36);
        attack[16] = sf::IntRect(379, 358, 53, 37);
        attack[17] = sf::IntRect(10, 400, 53, 42);

        Animation a;
        a.setup("idle", idle, 16, 10.f, true);  addAnimation("idle", a);
        a.setup("walk", walk, 16, 10.f, true);  addAnimation("walk", a);
        a.setup("attack", attack, 18, 10.f, false); addAnimation("attack", a);
    }
};

// ------------------------------------------------------------------ Factory ---

Enemies* Enemies::createEnemy(EnemyType type)
{
    switch (type)
    {
    case EnemyType::RebelSoldier:    return new RebelSoldier();
    case EnemyType::ShieldedSoldier: return new ShieldedSoldier();
    case EnemyType::GrenadeSoldier:  return new GrenadeSoldier();
    case EnemyType::BazookaSoldier:  return new BazookaSoldier();
    case EnemyType::Zombie:          return new Zombie();
    case EnemyType::Mummy:           return new Mummy();
    case EnemyType::Marine:          return new Marine();
    default:                         return nullptr;
    }
}*/

#include "Enemy.h"
#include "Animation.h"
#include <cmath>


EnemyAI::EnemyAI(float detectionRange, float attackRange)
    : detectionRange(detectionRange)
    , attackRange(attackRange)
    , m_stateTimer(0.f)
{
}

void EnemyAI::reset()
{
    m_stateTimer = 0.f;
}

void EnemyAI::update(float dt, int& stateOut, float distToPlayer, const Animation* currentAnim)
{
    // Never cut short a non-looping animation (e.g. attack mid-swing)
    if (currentAnim &&
        !currentAnim->isLooping() &&
        !currentAnim->isFinished())
        return;

    // Decide state purely from distance — no random timer needed
    if (distToPlayer <= attackRange)
        stateOut = 2;           // close enough to attack
    else if (distToPlayer <= detectionRange)
        stateOut = 1;           // detected — walk toward player
    else
        stateOut = 0;           // out of range — idle
}
static bool isTileE(char** lvl, int x, int y, int mapWidth, int mapHeight)
{
    if (x < 0 || x >= mapWidth || y < 0 || y >= mapHeight)
        return false;
    return lvl[y][x] == 'g';
}

// ------------------------------------------------------------------ base ---

Enemies::Enemies(int dmg, float spd, int hp)
{
    damage = dmg;
    speed = spd;
    health = hp;
    alive = true;
    m_state = 0;
    m_animCount = 0;
    m_currentAnimation = nullptr;
    facingRight = false;
    baseScale = 1.f;
    m_hasTarget = false;
    m_targetPos = { 0.f, 0.f };
    m_sprite.setPosition(0.f, 0.f);
}

Enemies::~Enemies() {}

// ---- convenience setters -----------------------------------------------

void Enemies::setPosition(float x, float y)
{
    m_sprite.setPosition(x, y);
}

// Call this after loading the texture so the collision box scales too.
// baseScale = 2.f means the sprite is drawn at 2× its original pixel size.
void Enemies::setScale(float scale)
{
    baseScale = scale;

    // Scale the collision box proportionally so it still wraps the sprite.
    // Tweak the base values (24, 36, 4, 2) to match your sprite's actual size
    // at scale 1.0 and everything else follows automatically.
    collisionWidth = 24.f * scale;
    collisionHeight = 36.f * scale;
    collisionOffsetX = 4.f * scale;
    collisionOffsetY = 2.f * scale;
}

// Call every frame from main with the player's centre position.
void Enemies::setTarget(sf::Vector2f targetPos)
{
    m_targetPos = targetPos;
    m_hasTarget = true;
}

// ---- animation helpers -------------------------------------------------

void Enemies::copyName(char* dest, const char* src, int maxLen)
{
    int i = 0;
    for (; i < maxLen - 1 && src[i] != '\0'; i++)
        dest[i] = src[i];
    dest[i] = '\0';
}

void Enemies::addAnimation(const char* name, const Animation& anim)
{
    if (m_animCount >= 10) return;
    copyName(m_animNames[m_animCount], name, 32);
    m_animations[m_animCount] = anim;
    m_animCount++;
}

void Enemies::setAnimation(const char* name)
{
    for (int i = 0; i < m_animCount; i++)
    {
        if (std::strcmp(m_animNames[i], name) == 0)
        {
            if (m_currentAnimation == &m_animations[i])
            {
                // Only skip if it's still actively playing
                if (!m_currentAnimation->isFinished())
                    return;
            }
            m_currentAnimation = &m_animations[i];
            m_currentAnimation->reset();
            m_currentAnimation->play();
            return;
        }
    }
}

/*void Enemies::setAnimation(const char* name)
{
    for (int i = 0; i < m_animCount; i++)
    {
        if (std::strcmp(m_animNames[i], name) == 0)
        {
            if (m_currentAnimation == &m_animations[i])
                return;

            m_currentAnimation = &m_animations[i];
            m_currentAnimation->reset();
            m_currentAnimation->play();
            return;
        }
    }
}*/

Animation* Enemies::getCurrentAnimation() { return m_currentAnimation; }
sf::Sprite& Enemies::getSprite() { return m_sprite; }
bool        Enemies::isAlive() const { return alive; }

// ---- AI (delegates to m_ai) -------------------------------------------

void Enemies::updateAI(float dt)
{
    float dist = 9999.f;
    if (m_hasTarget)
    {
        float dx = m_targetPos.x - m_sprite.getPosition().x;
        float dy = m_targetPos.y - m_sprite.getPosition().y;
        dist = std::sqrt(dx * dx + dy * dy);
    }

    m_ai.update(dt, m_state, dist, m_currentAnimation);

    if (m_state == 0) setAnimation("idle");
    if (m_state == 2) setAnimation("attack");
    // walk animation is set inside move() so it only plays when actually moving
}

// ---- physics -----------------------------------------------------------

void Enemies::physicsUpdate(float dt, char** lvl, int cell_size, int mapWidth, int mapHeight)
{
    if (!onGround)
    {
        velocityY += gravity;
        if (velocityY > 20.f) velocityY = 20.f;
    }
    else
    {
        velocityY = 0.f;
    }

    float bx = m_sprite.getPosition().x + collisionOffsetX;
    float by = m_sprite.getPosition().y + collisionOffsetY;
    float bw = collisionWidth;
    float bh = collisionHeight;

    // move X
    m_sprite.move(velocityX, 0.f);
    bx = m_sprite.getPosition().x + collisionOffsetX;

    int L = std::max(0, (int)(bx / cell_size));
    int R = std::min(mapWidth - 1, (int)((bx + bw - 1) / cell_size));
    int T = std::max(0, (int)(by / cell_size));
    int B = std::min(mapHeight - 1, (int)((by + bh - 1) / cell_size));

    if (velocityX > 0.f)
    {
        for (int i = T; i <= B; i++)
        {
            if (isTileE(lvl, R, i, mapWidth, mapHeight))
            {
                m_sprite.setPosition(R * cell_size - bw - collisionOffsetX,
                    m_sprite.getPosition().y);
                velocityX = 0.f;
                bx = m_sprite.getPosition().x + collisionOffsetX;
                break;
            }
        }
    }
    else if (velocityX < 0.f)
    {
        for (int i = T; i <= B; i++)
        {
            if (isTileE(lvl, L, i, mapWidth, mapHeight))
            {
                m_sprite.setPosition((L + 1) * cell_size - collisionOffsetX,
                    m_sprite.getPosition().y);
                velocityX = 0.f;
                bx = m_sprite.getPosition().x + collisionOffsetX;
                break;
            }
        }
    }

    // move Y
    m_sprite.move(0.f, velocityY);
    by = m_sprite.getPosition().y + collisionOffsetY;

    L = std::max(0, (int)(bx / cell_size));
    R = std::min(mapWidth - 1, (int)((bx + bw - 1) / cell_size));
    T = std::max(0, (int)(by / cell_size));
    B = std::min(mapHeight - 1, (int)((by + bh - 1) / cell_size));

    onGround = false;

    if (velocityY >= 0.f)
    {
        for (int j = L; j <= R; j++)
        {
            if (isTileE(lvl, j, B, mapWidth, mapHeight))
            {
                m_sprite.setPosition(m_sprite.getPosition().x,
                    B * cell_size - bh - collisionOffsetY);
                velocityY = 0.f;
                onGround = true;
                break;
            }
        }
    }
    else
    {
        for (int j = L; j <= R; j++)
        {
            if (isTileE(lvl, j, T, mapWidth, mapHeight))
            {
                m_sprite.setPosition(m_sprite.getPosition().x,
                    (T + 1) * cell_size - collisionOffsetY);
                velocityY = 0.f;
                break;
            }
        }
    }

    // clamp to map
    bx = m_sprite.getPosition().x + collisionOffsetX;
    by = m_sprite.getPosition().y + collisionOffsetY;
    float cx = bx, cy = by;
    if (cx < 0.f)                             cx = 0.f;
    else if (cx > mapWidth * cell_size - bw) cx = mapWidth * cell_size - bw;
    if (cy < 0.f)                             cy = 0.f;
    else if (cy > mapHeight * cell_size - bh)
    {
        cy = mapHeight * cell_size - bh;
        velocityY = 0.f;
        onGround = true;
    }
    m_sprite.setPosition(cx - collisionOffsetX, cy - collisionOffsetY);
}

// ---- main update -------------------------------------------------------

void Enemies::update(float dt, char** lvl, int cell_size, int mapWidth, int mapHeight)
{
    updateAI(dt);
    move(dt);
    updateCollisionForAnimation();
    physicsUpdate(dt, lvl, cell_size, mapWidth, mapHeight);

    // Apply facing direction.
    // Sprites face LEFT by default (facingRight = false → normal scale).
    // When facing right we flip on X around the sprite's left edge.
    // If the sprite jumps sideways when flipped, set the origin to its
    // horizontal centre after loading the texture:
    //   m_sprite.setOrigin(m_texture.getSize().x / 2.f, 0.f);
    if (facingRight)
        m_sprite.setScale(-baseScale, baseScale);
    else
        m_sprite.setScale(baseScale, baseScale);

    if (m_currentAnimation)
    {
        m_currentAnimation->update(dt);
        m_sprite.setTextureRect(m_currentAnimation->getCurrentFrame());
    }
}

// ================================================================== subclasses ==

// Shared move() logic: walk toward the target, flip to face it.
// Each subclass just calls this helper — avoids copy-pasting into every class.
// The only thing subclasses customise is speed (set in their constructor).
namespace
{
    void chaseMove(Enemies& e, float speed, int state,
        bool hasTarget, sf::Vector2f targetPos,
        float& velocityX, bool& facingRight)
    {
        if (state == 1 && hasTarget)
        {
            float dx = targetPos.x - e.getSprite().getPosition().x;
            facingRight = (dx > 0.f);           // face the player
            velocityX = (dx > 0.f) ? speed : -speed;
            e.setAnimation("walk");
        }
        else
        {
            velocityX = 0.f;
        }
    }
}

// ------------------------------------------------------------------ Infantry ---

class Infantry : public Enemies
{
public:
    Infantry(int dmg, float spd, int hp) : Enemies(dmg, spd, hp) {}
    virtual void move(float dt) = 0;
};

// ------------------------------------------------------------------ RebelSoldier ---

class RebelSoldier : public Infantry
{
public:
    RebelSoldier() : Infantry(10, 2.0f, 120)
    {
        m_texture.loadFromFile("Sprites/normalrebelsoldier.png");
        m_sprite.setTexture(m_texture);
        setupAnimations();
        setAnimation("idle");
        setScale(2.75f);   // ← uncomment and adjust to resize this enemy
        collisionWidth = 40.f * 3.f;   // idle frame width  = 40
        collisionHeight = 38.f * 3.f;   // idle frame height = 38
        collisionOffsetX = 4.f;
        collisionOffsetY = 0.f;
    }

    void attack() override { setAnimation("attack"); }

    void move(float dt) override
    {
        chaseMove(*this, speed, m_state, m_hasTarget, m_targetPos, velocityX, facingRight);
    }

    void updateCollisionForAnimation() override {
        if (!m_currentAnimation) return;

        if (std::strcmp(m_currentAnimation->name, "attack") == 0)
        {
            collisionWidth = 40.f * 3.f;   // idle frame width  = 40
            collisionHeight = 38.f * 3.f;   // idle frame height = 38
            collisionOffsetX = 4.f;
            collisionOffsetY = 0.f;
        }
        else
        {
            collisionWidth = 40.f * 2.75f;   // idle frame width  = 40
            collisionHeight = 38.f * 2.75f;   // idle frame height = 38
            collisionOffsetX = 4.f;
            collisionOffsetY = 0.f;
        }
    }

private:
    void setupAnimations()
    {
        sf::IntRect idle[4];
        idle[0] = sf::IntRect(3, 3, 40, 38);
        idle[1] = sf::IntRect(46, 3, 40, 38);
        idle[2] = sf::IntRect(131, 3, 40, 38);
        idle[3] = sf::IntRect(89, 4, 39, 37);

        sf::IntRect walk[15];
        walk[0] = sf::IntRect(3, 44, 34, 37);
        walk[1] = sf::IntRect(40, 44, 37, 37);
        walk[2] = sf::IntRect(80, 45, 42, 31);
        walk[3] = sf::IntRect(125, 46, 42, 33);
        walk[4] = sf::IntRect(170, 45, 37, 35);
        walk[5] = sf::IntRect(210, 44, 35, 37);
        walk[6] = sf::IntRect(131, 3, 40, 38);
        walk[7] = sf::IntRect(248, 44, 34, 37);
        walk[8] = sf::IntRect(285, 44, 38, 36);
        walk[9] = sf::IntRect(326, 45, 40, 30);
        walk[10] = sf::IntRect(369, 46, 38, 32);
        walk[11] = sf::IntRect(410, 45, 37, 35);
        walk[12] = sf::IntRect(450, 44, 35, 37);
        walk[13] = sf::IntRect(3, 85, 39, 37);
        walk[14] = sf::IntRect(45, 84, 40, 38);

        sf::IntRect attack[10];
        attack[0] = sf::IntRect(3, 298, 44, 38);
        attack[1] = sf::IntRect(50, 298, 48, 38);
        attack[2] = sf::IntRect(101, 298, 47, 38);
        attack[3] = sf::IntRect(151, 297, 42, 39);
        attack[4] = sf::IntRect(196, 289, 41, 47);
        attack[5] = sf::IntRect(240, 289, 41, 47);
        attack[6] = sf::IntRect(284, 292, 43, 44);
        attack[7] = sf::IntRect(330, 298, 47, 38);
        attack[8] = sf::IntRect(380, 298, 46, 38);
        attack[9] = sf::IntRect(429, 298, 40, 38);

        Animation a;
        a.setup("idle", idle, 4, 4.f, true);  addAnimation("idle", a);
        a.setup("walk", walk, 15, 10.f, true);  addAnimation("walk", a);
        a.setup("attack", attack, 10, 10.f, false); addAnimation("attack", a);
    }
};

// ------------------------------------------------------------------ ShieldedSoldier ---

class ShieldedSoldier : public Infantry
{
public:
    ShieldedSoldier() : Infantry(10, 2.0f, 120)
    {
        m_texture.loadFromFile("Sprites/shieldedrebelsoldier.png");
        m_sprite.setTexture(m_texture);
        setupAnimations();
        setAnimation("idle");
        setScale(2.75f);
        collisionWidth = 32.f * 2.75f;   // idle frame width  = 32
        collisionHeight = 38.f * 2.75f;   // idle frame height = 38
        collisionOffsetX = 4.f;
        collisionOffsetY = 0.f;
    }

    void attack() override { setAnimation("attack"); }

    void move(float dt) override
    {
        chaseMove(*this, speed, m_state, m_hasTarget, m_targetPos, velocityX, facingRight);
    }

private:
    void setupAnimations()
    {
        sf::IntRect idle[6];
        idle[0] = sf::IntRect(3, 5, 32, 38);
        idle[1] = sf::IntRect(38, 5, 32, 38);
        idle[2] = sf::IntRect(73, 5, 31, 38);
        idle[3] = sf::IntRect(107, 4, 32, 39);
        idle[4] = sf::IntRect(143, 5, 31, 38);
        idle[5] = sf::IntRect(177, 5, 32, 38);

        sf::IntRect walk[16];
        walk[0] = sf::IntRect(3, 47, 29, 40);
        walk[1] = sf::IntRect(35, 46, 28, 41);
        walk[2] = sf::IntRect(66, 46, 30, 40);
        walk[3] = sf::IntRect(99, 47, 31, 39);
        walk[4] = sf::IntRect(133, 49, 31, 38);
        walk[5] = sf::IntRect(167, 48, 32, 39);
        walk[6] = sf::IntRect(202, 47, 30, 40);
        walk[7] = sf::IntRect(235, 46, 29, 41);
        walk[8] = sf::IntRect(267, 46, 28, 40);
        walk[9] = sf::IntRect(298, 47, 28, 39);
        walk[10] = sf::IntRect(329, 49, 27, 38);
        walk[11] = sf::IntRect(359, 48, 27, 39);
        walk[12] = sf::IntRect(3, 90, 32, 38);
        walk[13] = sf::IntRect(38, 90, 32, 38);
        walk[14] = sf::IntRect(73, 90, 36, 38);
        walk[15] = sf::IntRect(112, 90, 32, 38);

        sf::IntRect attack[10];
        attack[0] = sf::IntRect(3, 305, 30, 38);
        attack[1] = sf::IntRect(36, 305, 28, 38);
        attack[2] = sf::IntRect(67, 305, 31, 38);
        attack[3] = sf::IntRect(101, 305, 31, 38);
        attack[4] = sf::IntRect(135, 305, 38, 38);
        attack[5] = sf::IntRect(176, 305, 37, 38);
        attack[6] = sf::IntRect(216, 305, 35, 38);
        attack[7] = sf::IntRect(254, 305, 34, 38);
        attack[8] = sf::IntRect(291, 305, 35, 38);
        attack[9] = sf::IntRect(329, 302, 37, 41);

        Animation a;
        a.setup("idle", idle, 6, 6.f, true);  addAnimation("idle", a);
        a.setup("walk", walk, 16, 10.f, true);  addAnimation("walk", a);
        a.setup("attack", attack, 10, 10.f, false); addAnimation("attack", a);
    }
};

// ------------------------------------------------------------------ GrenadeSoldier ---

class GrenadeSoldier : public Infantry
{
public:
    GrenadeSoldier() : Infantry(20, 1.5f, 180)
    {
        m_texture.loadFromFile("Sprites/grenaderebelsoldier.png");
        m_sprite.setTexture(m_texture);
        setupAnimations();
        setAnimation("idle");
        setScale(1.1f);
        collisionWidth = 64.f * 1.1f;   // frame width  from idle[0] = 64
        collisionHeight = 91.f * 1.1f;   // frame height from idle[0] = 91
        collisionOffsetX = 4.f;
        collisionOffsetY = 0.f;
    }

    void attack() override { setAnimation("attack"); }

    void move(float dt) override
    {
        chaseMove(*this, speed, m_state, m_hasTarget, m_targetPos, velocityX, facingRight);
    }

private:
    void setupAnimations()
    {
        sf::IntRect idle[4];
        idle[0] = sf::IntRect(18, 63, 64, 91);
        idle[1] = sf::IntRect(94, 66, 65, 88);
        idle[2] = sf::IntRect(171, 68, 62, 86);
        idle[3] = sf::IntRect(245, 70, 57, 84);

        sf::IntRect walk[12];
        walk[0] = sf::IntRect(25, 216, 48, 91);
        walk[1] = sf::IntRect(85, 211, 47, 96);
        walk[2] = sf::IntRect(144, 211, 55, 96);
        walk[3] = sf::IntRect(211, 213, 63, 94);
        walk[4] = sf::IntRect(286, 216, 57, 91);
        walk[5] = sf::IntRect(355, 216, 53, 91);
        walk[6] = sf::IntRect(420, 216, 50, 91);
        walk[7] = sf::IntRect(484, 211, 50, 96);
        walk[8] = sf::IntRect(546, 211, 53, 96);
        walk[9] = sf::IntRect(611, 213, 60, 94);
        walk[10] = sf::IntRect(683, 216, 52, 91);
        walk[11] = sf::IntRect(747, 216, 46, 91);

        sf::IntRect attack[16];
        attack[0] = sf::IntRect(8, 371, 62, 89);
        attack[1] = sf::IntRect(82, 374, 64, 86);
        attack[2] = sf::IntRect(158, 374, 67, 86);
        attack[3] = sf::IntRect(237, 371, 75, 89);
        attack[4] = sf::IntRect(324, 366, 83, 94);
        attack[5] = sf::IntRect(419, 359, 96, 101);
        attack[6] = sf::IntRect(527, 366, 101, 94);
        attack[7] = sf::IntRect(640, 369, 102, 91);
        attack[8] = sf::IntRect(754, 359, 87, 101);
        attack[9] = sf::IntRect(853, 359, 78, 101);
        attack[10] = sf::IntRect(943, 376, 84, 84);
        attack[11] = sf::IntRect(1039, 378, 70, 82);
        attack[12] = sf::IntRect(1121, 381, 67, 79);
        attack[13] = sf::IntRect(1200, 374, 67, 86);
        attack[14] = sf::IntRect(1279, 374, 69, 86);
        attack[15] = sf::IntRect(1360, 371, 65, 89);

        Animation a;
        a.setup("idle", idle, 4, 4.f, true);  addAnimation("idle", a);
        a.setup("walk", walk, 12, 10.f, true);  addAnimation("walk", a);
        a.setup("attack", attack, 16, 10.f, false); addAnimation("attack", a);
    }
};

// ------------------------------------------------------------------ BazookaSoldier ---

class BazookaSoldier : public Infantry
{
public:
    BazookaSoldier() : Infantry(20, 1.5f, 80)
    {
        m_texture.loadFromFile("Sprites/bazookarebelsoldier.png");
        m_sprite.setTexture(m_texture);
        setupAnimations();
        setAnimation("idle");
        setScale(2.75f);
        collisionWidth = 36.f * 2.75f;   // idle frame width  = 36
        collisionHeight = 45.f * 2.75f;   // idle frame height = 45
        collisionOffsetX = 4.f;
        collisionOffsetY = 0.f;
    }

    void attack() override {
        setAnimation("attack");
    }

    void move(float dt) override
    {
        chaseMove(*this, speed, m_state, m_hasTarget, m_targetPos, velocityX, facingRight);
    }

private:
    void setupAnimations()
    {
        sf::IntRect idle[6];
        idle[0] = sf::IntRect(3, 3, 36, 45);
        idle[1] = sf::IntRect(42, 3, 38, 45);
        idle[2] = sf::IntRect(83, 5, 38, 43);
        idle[3] = sf::IntRect(124, 6, 36, 42);
        idle[4] = sf::IntRect(163, 5, 38, 43);
        idle[5] = sf::IntRect(204, 3, 38, 45);

        sf::IntRect walk[16];
        walk[0] = sf::IntRect(3, 55, 41, 39);
        walk[1] = sf::IntRect(47, 54, 41, 40);
        walk[2] = sf::IntRect(91, 53, 40, 40);
        walk[3] = sf::IntRect(134, 54, 41, 39);
        walk[4] = sf::IntRect(178, 56, 40, 38);
        walk[5] = sf::IntRect(221, 55, 41, 39);
        walk[6] = sf::IntRect(265, 54, 41, 40);
        walk[7] = sf::IntRect(309, 53, 38, 41);
        walk[8] = sf::IntRect(350, 52, 37, 41);
        walk[9] = sf::IntRect(390, 51, 36, 42);
        walk[10] = sf::IntRect(429, 53, 37, 41);
        walk[11] = sf::IntRect(3, 102, 38, 40);
        walk[12] = sf::IntRect(46, 99, 38, 43);
        walk[13] = sf::IntRect(87, 98, 38, 44);
        walk[14] = sf::IntRect(128, 98, 37, 44);
        walk[15] = sf::IntRect(168, 97, 36, 45);

        sf::IntRect attack[5];
        attack[0] = sf::IntRect(3, 241, 39, 44);
        attack[1] = sf::IntRect(45, 243, 41, 42);
        attack[2] = sf::IntRect(89, 245, 41, 40);
        attack[3] = sf::IntRect(45, 243, 41, 42);
        attack[4] = sf::IntRect(3, 241, 39, 44);

        Animation a;
        a.setup("idle", idle, 6, 6.f, true);  addAnimation("idle", a);
        a.setup("walk", walk, 16, 10.f, true);  addAnimation("walk", a);
        a.setup("attack", attack, 5, 10.f, false); addAnimation("attack", a);
    }
};

// ------------------------------------------------------------------ Undead ---

class Undead : public Enemies
{
public:
    Undead(int dmg, float spd, int hp) : Enemies(dmg, spd, hp) {}
    void attack() override {}
    void move(float dt) override
    {
        chaseMove(*this, speed, m_state, m_hasTarget, m_targetPos, velocityX, facingRight);
    }
};

// ------------------------------------------------------------------ Zombie ---

class Zombie : public Undead
{
public:
    Zombie() : Undead(5, 1.0f, 60)
    {
        m_texture.loadFromFile("Sprites/zombieenemy.png");
        m_sprite.setTexture(m_texture);
        setupAnimations();
        setAnimation("idle");
        setScale(2.75f);
        collisionWidth = 30.f * 2.75f;   // idle frame width  = 30
        collisionHeight = 39.f * 2.75f;   // idle frame height = 39
        collisionOffsetX = 4.f;
        collisionOffsetY = 0.f;
    }

    void attack() override {}

private:
    void setupAnimations()
    {
        sf::IntRect idle[7];
        idle[0] = sf::IntRect(5, 4, 30, 39);
        idle[1] = sf::IntRect(39, 4, 30, 39);
        idle[2] = sf::IntRect(73, 4, 30, 39);
        idle[3] = sf::IntRect(106, 3, 31, 40);
        idle[4] = sf::IntRect(139, 3, 32, 40);
        idle[5] = sf::IntRect(176, 3, 33, 40);
        idle[6] = sf::IntRect(217, 3, 35, 40);

        sf::IntRect walk[16];
        walk[0] = sf::IntRect(7, 46, 28, 39);
        walk[1] = sf::IntRect(41, 46, 28, 39);
        walk[2] = sf::IntRect(73, 46, 30, 39);
        walk[3] = sf::IntRect(107, 46, 30, 39);
        walk[4] = sf::IntRect(141, 46, 30, 39);
        walk[5] = sf::IntRect(174, 46, 31, 39);
        walk[6] = sf::IntRect(213, 46, 26, 39);
        walk[7] = sf::IntRect(246, 46, 27, 39);
        walk[8] = sf::IntRect(281, 46, 26, 39);
        walk[9] = sf::IntRect(316, 46, 25, 39);
        walk[10] = sf::IntRect(343, 46, 25, 39);
        walk[11] = sf::IntRect(377, 47, 25, 38);
        walk[12] = sf::IntRect(411, 48, 28, 37);
        walk[13] = sf::IntRect(452, 47, 31, 38);
        walk[14] = sf::IntRect(3, 88, 27, 39);
        walk[15] = sf::IntRect(37, 88, 27, 39);

        Animation a;
        a.setup("idle", idle, 7, 7.f, true); addAnimation("idle", a);
        a.setup("walk", walk, 16, 10.f, true); addAnimation("walk", a);
    }
};

// ------------------------------------------------------------------ Mummy ---

class Mummy : public Undead
{
public:
    Mummy() : Undead(8, 0.8f, 80)
    {
        m_texture.loadFromFile("Sprites/mummyenemy.png");
        m_sprite.setTexture(m_texture);
        setupAnimations();
        setAnimation("idle");
        setScale(2.5f);
        collisionWidth = 30.f * 2.5f;   // idle frame width  = 30
        collisionHeight = 45.f * 2.5f;   // idle frame height = 45
        collisionOffsetX = 4.f;
        collisionOffsetY = 0.f;
    }

    void attack() override { setAnimation("attack"); }

private:
    void setupAnimations()
    {
        sf::IntRect idle[4];
        idle[0] = sf::IntRect(27, 1010, 30, 45);
        idle[1] = sf::IntRect(68, 1010, 30, 45);
        idle[2] = sf::IntRect(106, 1010, 29, 45);
        idle[3] = sf::IntRect(144, 1010, 29, 45);

        sf::IntRect walk[18];
        walk[0] = sf::IntRect(195, 1010, 31, 45);
        walk[1] = sf::IntRect(235, 1010, 31, 45);
        walk[2] = sf::IntRect(275, 1010, 31, 45);
        walk[3] = sf::IntRect(314, 1010, 33, 45);
        walk[4] = sf::IntRect(355, 1011, 31, 44);
        walk[5] = sf::IntRect(394, 1012, 31, 43);
        walk[6] = sf::IntRect(432, 1011, 31, 44);
        walk[7] = sf::IntRect(471, 1010, 31, 45);
        walk[8] = sf::IntRect(508, 1010, 32, 45);
        walk[9] = sf::IntRect(548, 1010, 32, 45);
        walk[10] = sf::IntRect(593, 1010, 31, 45);
        walk[11] = sf::IntRect(638, 1010, 31, 45);
        walk[12] = sf::IntRect(680, 1011, 33, 44);
        walk[13] = sf::IntRect(722, 1012, 36, 43);
        walk[14] = sf::IntRect(767, 1011, 33, 44);
        walk[15] = sf::IntRect(805, 1010, 31, 45);
        walk[16] = sf::IntRect(844, 1010, 31, 45);
        walk[17] = sf::IntRect(883, 1010, 31, 45);

        sf::IntRect attack[28];
        attack[0] = sf::IntRect(13, 1074, 28, 45);
        attack[1] = sf::IntRect(49, 1074, 27, 45);
        attack[2] = sf::IntRect(87, 1074, 23, 45);
        attack[3] = sf::IntRect(118, 1075, 23, 44);
        attack[4] = sf::IntRect(148, 1074, 29, 45);
        attack[5] = sf::IntRect(185, 1073, 25, 46);
        attack[6] = sf::IntRect(217, 1073, 25, 46);
        attack[7] = sf::IntRect(251, 1074, 26, 45);
        attack[8] = sf::IntRect(286, 1075, 27, 44);
        attack[9] = sf::IntRect(320, 1076, 28, 43);
        attack[10] = sf::IntRect(354, 1077, 28, 42);
        attack[11] = sf::IntRect(387, 1078, 28, 41);
        attack[12] = sf::IntRect(421, 1078, 29, 41);
        attack[13] = sf::IntRect(456, 1078, 30, 41);
        attack[14] = sf::IntRect(492, 1078, 30, 41);
        attack[15] = sf::IntRect(530, 1077, 30, 42);
        attack[16] = sf::IntRect(567, 1076, 29, 43);
        attack[17] = sf::IntRect(602, 1076, 28, 43);
        attack[18] = sf::IntRect(637, 1076, 28, 43);
        attack[19] = sf::IntRect(671, 1076, 27, 43);
        attack[20] = sf::IntRect(705, 1075, 27, 44);
        attack[21] = sf::IntRect(738, 1074, 27, 45);
        attack[22] = sf::IntRect(774, 1074, 26, 45);
        attack[23] = sf::IntRect(809, 1074, 26, 45);
        attack[24] = sf::IntRect(842, 1074, 25, 45);
        attack[25] = sf::IntRect(876, 1074, 23, 45);
        attack[26] = sf::IntRect(908, 1074, 21, 45);
        attack[27] = sf::IntRect(937, 1074, 28, 45);

        Animation a;
        a.setup("idle", idle, 4, 4.f, true);  addAnimation("idle", a);
        a.setup("walk", walk, 18, 10.f, true);  addAnimation("walk", a);
        a.setup("attack", attack, 28, 10.f, false); addAnimation("attack", a);
    }
};

// ------------------------------------------------------------------ Marine ---

class Marine : public Enemies
{
public:
    Marine() : Enemies(15, 3.0f, 100)
    {
        m_texture.loadFromFile("Sprites/marineenemy.png");
        m_sprite.setTexture(m_texture);
        setupAnimations();
        setAnimation("idle");
        setScale(2.75f);
        collisionWidth = 49.f * 2.75f;   // idle frame width  = 49
        collisionHeight = 43.f * 2.75f;   // idle frame height = 43
        collisionOffsetX = 4.f;
        collisionOffsetY = 0.f;
    }

    void attack() override { setAnimation("attack"); }

    void move(float dt) override
    {
        chaseMove(*this, speed, m_state, m_hasTarget, m_targetPos, velocityX, facingRight);
    }

private:
    void setupAnimations()
    {
        sf::IntRect idle[16];
        idle[0] = sf::IntRect(10, 18, 49, 43);
        idle[1] = sf::IntRect(64, 18, 49, 43);
        idle[2] = sf::IntRect(118, 19, 48, 42);
        idle[3] = sf::IntRect(171, 19, 48, 42);
        idle[4] = sf::IntRect(224, 19, 48, 42);
        idle[5] = sf::IntRect(277, 19, 48, 42);
        idle[6] = sf::IntRect(330, 19, 48, 42);
        idle[7] = sf::IntRect(383, 19, 48, 42);
        idle[8] = sf::IntRect(436, 19, 48, 42);
        idle[9] = sf::IntRect(489, 19, 48, 42);
        idle[10] = sf::IntRect(542, 19, 48, 42);
        idle[11] = sf::IntRect(595, 19, 48, 42);
        idle[12] = sf::IntRect(648, 19, 48, 42);
        idle[13] = sf::IntRect(701, 19, 48, 42);
        idle[14] = sf::IntRect(754, 19, 48, 42);
        idle[15] = sf::IntRect(807, 19, 48, 42);

        sf::IntRect walk[16];
        walk[0] = sf::IntRect(10, 79, 49, 43);
        walk[1] = sf::IntRect(64, 79, 50, 43);
        walk[2] = sf::IntRect(119, 80, 50, 42);
        walk[3] = sf::IntRect(174, 80, 51, 42);
        walk[4] = sf::IntRect(230, 80, 52, 42);
        walk[5] = sf::IntRect(287, 80, 51, 42);
        walk[6] = sf::IntRect(343, 80, 51, 42);
        walk[7] = sf::IntRect(399, 80, 52, 42);
        walk[8] = sf::IntRect(456, 80, 52, 42);
        walk[9] = sf::IntRect(513, 80, 52, 42);
        walk[10] = sf::IntRect(570, 80, 52, 42);
        walk[11] = sf::IntRect(627, 80, 50, 42);
        walk[12] = sf::IntRect(683, 80, 48, 42);
        walk[13] = sf::IntRect(736, 80, 48, 42);
        walk[14] = sf::IntRect(789, 80, 48, 42);
        walk[15] = sf::IntRect(842, 80, 48, 42);

        sf::IntRect attack[18];
        attack[0] = sf::IntRect(10, 309, 49, 43);
        attack[1] = sf::IntRect(64, 309, 49, 43);
        attack[2] = sf::IntRect(118, 309, 47, 43);
        attack[3] = sf::IntRect(170, 309, 47, 43);
        attack[4] = sf::IntRect(222, 309, 46, 43);
        attack[5] = sf::IntRect(273, 309, 46, 43);
        attack[6] = sf::IntRect(324, 309, 45, 43);
        attack[7] = sf::IntRect(374, 309, 44, 43);
        attack[8] = sf::IntRect(423, 309, 53, 43);
        attack[9] = sf::IntRect(481, 313, 55, 39);
        attack[10] = sf::IntRect(541, 314, 54, 38);
        attack[11] = sf::IntRect(10, 357, 69, 38);
        attack[12] = sf::IntRect(84, 358, 70, 37);
        attack[13] = sf::IntRect(167, 359, 67, 36);
        attack[14] = sf::IntRect(262, 360, 55, 35);
        attack[15] = sf::IntRect(322, 359, 52, 36);
        attack[16] = sf::IntRect(379, 358, 53, 37);
        attack[17] = sf::IntRect(10, 400, 53, 42);

        Animation a;
        a.setup("idle", idle, 16, 16.f, true);  addAnimation("idle", a);
        a.setup("walk", walk, 16, 10.f, true);  addAnimation("walk", a);
        a.setup("attack", attack, 18, 10.f, false); addAnimation("attack", a);
    }
};
// ------------------------------------------------------------------ Paratrooper helper ---

static EnemyType randomInfantryType()
{
    EnemyType types[] = {
        EnemyType::RebelSoldier,
        EnemyType::ShieldedSoldier,
        EnemyType::GrenadeSoldier,
        EnemyType::BazookaSoldier
    };
    return types[rand() % 4];
}

// ------------------------------------------------------------------ Paratrooper ---
class Paratrooper : public Infantry
{
public:
    Paratrooper()
        : Infantry(0, 0.f, 999)
        , m_landAs(randomInfantryType())
        , m_landed(false)
        , m_landing(false)
        , m_replacement(nullptr)
    {
        m_texture.loadFromFile("Sprites/paratrooper.png");
        m_sprite.setTexture(m_texture);
        setupAnimations();
        setAnimation("fall");
        setScale(1.25f);
        collisionWidth = 100.f * 1.25f;
        collisionHeight = 150.f * 1.25f;
        collisionOffsetX = 4.f;
        collisionOffsetY = 0.f;
    }

    void attack() override {}

    void move(float dt) override
    {
        // Stop moving horizontally and vertically once landing starts
        if (m_landing || m_landed)
            velocityX = 0.f;
        else
            velocityX = 0.f;  // always falls straight down
    }

    Enemies* getLandedReplacement() override
    {
        if (!m_landed) return nullptr;
        Enemies* r = m_replacement;
        m_replacement = nullptr;
        return r;
    }

    void update(float dt, char** lvl, int cell_size, int mapWidth, int mapHeight) override
    {
        if (m_landed) return;

        // Cap parachute fall speed
        if (velocityY > 3.f) velocityY = 3.f;   // ← add this line

        Enemies::update(dt, lvl, cell_size, mapWidth, mapHeight);

        if (onGround && !m_landing && !m_landed)
        {
            m_landing = true;
            velocityY = 0.f;
            setAnimation("land");
        }

        if (m_landing && m_currentAnimation &&
            std::strcmp(m_currentAnimation->name, "land") == 0 &&
            m_currentAnimation->isFinished())
        {
            m_landed = true;
            alive = false;   // hide paratrooper immediately
            m_replacement = Enemies::createEnemy(m_landAs);
            m_replacement->setPosition(m_sprite.getPosition().x,
                m_sprite.getPosition().y);
            if (m_hasTarget)
                m_replacement->setTarget(m_targetPos);

            // Force first frame so the spritesheet never flashes
            if (m_replacement->getCurrentAnimation())
                m_replacement->getSprite().setTextureRect(
                    m_replacement->getCurrentAnimation()->getCurrentFrame());
        }
    }
    /*
    void update(float dt, char** lvl, int cell_size, int mapWidth, int mapHeight) override
    {
        if (m_landed) return;

        // Cap parachute fall speed
        if (velocityY > 3.f) velocityY = 3.f;

        Enemies::update(dt, lvl, cell_size, mapWidth, mapHeight);

        // Phase 1: just touched the ground — start landing animation
        if (onGround && !m_landing && !m_landed)
        {
            m_landing = true;
            velocityY = 0.f;
            setAnimation("land");   // plays once (non-looping)
        }

        // Phase 2: landing animation finished — spawn replacement
        if (m_landing && m_currentAnimation &&
            std::strcmp(m_currentAnimation->name, "land") == 0 &&
            m_currentAnimation->isFinished())
        {
            m_landed = true;
            m_replacement = Enemies::createEnemy(m_landAs);
            m_replacement->setPosition(m_sprite.getPosition().x,
                m_sprite.getPosition().y);
            if (m_hasTarget)
                m_replacement->setTarget(m_targetPos);
        }
    }*/

private:
    EnemyType m_landAs;
    bool      m_landed;
    bool      m_landing;
    Enemies* m_replacement;

    void setupAnimations()
    {
        // Replace these IntRects with your actual sprite frames
        sf::IntRect fall[4];
        fall[0] = sf::IntRect(3, 7, 113, 145);
        fall[1] = sf::IntRect(124, 7, 107, 157);
        fall[2] = sf::IntRect(243, 6, 109, 162);
        fall[3] = sf::IntRect(368, 6, 112, 155);

        sf::IntRect land[6];
        land[0] = sf::IntRect(3, 174, 117, 148);
        land[1] = sf::IntRect(152, 178, 120, 140);
        land[2] = sf::IntRect(297, 176, 117, 142);
        land[3] = sf::IntRect(466, 173, 115, 154);
        land[4] = sf::IntRect(605, 176, 119, 151);
        land[5] = sf::IntRect(747, 245, 169, 73);

        Animation a;
        a.setup("fall", fall, 4, 6.f, true);   // looping while falling
        addAnimation("fall", a);
        a.setup("land", land, 6, 8.f, false);  // plays once on touchdown
        addAnimation("land", a);
    }
};

/*
class Paratrooper : public Infantry
{
public:
    Paratrooper()
        : Infantry(0, 0.f, 999)
        , m_landAs(randomInfantryType())
        , m_landed(false)
        , m_replacement(nullptr)
    {
        m_texture.loadFromFile("Sprites/paratrooper.png");
        m_sprite.setTexture(m_texture);
        setupAnimations();
        setAnimation("fall");
        setScale(2.75f);
        collisionWidth = 30.f * 2.75f;
        collisionHeight = 50.f * 2.75f;
        collisionOffsetX = 4.f;
        collisionOffsetY = 0.f;
    }

    void attack() override {}

    void move(float dt) override
    {
        velocityX = 0.f;
    }

    Enemies* getLandedReplacement() override
    {
        if (!m_landed) return nullptr;
        Enemies* r = m_replacement;
        m_replacement = nullptr;
        return r;
    }

    void update(float dt, char** lvl, int cell_size, int mapWidth, int mapHeight) override
    {
        if (m_landed) return;

        Enemies::update(dt, lvl, cell_size, mapWidth, mapHeight);

        if (onGround && !m_landed)
        {
            m_landed = true;
            m_replacement = Enemies::createEnemy(m_landAs);
            m_replacement->setPosition(m_sprite.getPosition().x,
                m_sprite.getPosition().y);
            if (m_hasTarget)
                m_replacement->setTarget(m_targetPos);
        }
    }

private:
    EnemyType m_landAs;
    bool      m_landed;
    Enemies* m_replacement;

    void setupAnimations()
    {
        sf::IntRect fall[10];
        fall[0] = sf::IntRect(243, 6, 109, 162);
        fall[1] = sf::IntRect(48, 0, 48, 64);
        fall[2] = sf::IntRect(96, 0, 48, 64);
        fall[3] = sf::IntRect(144, 0, 48, 64);

        Animation a;
        a.setup("fall", fall, 4, 6.f, true);
        addAnimation("fall", a);
    }
};
*/
// ------------------------------------------------------------------ Factory ---

Enemies* Enemies::createEnemy(EnemyType type)
{
    switch (type)
    {
    case EnemyType::RebelSoldier:    return new RebelSoldier();
    case EnemyType::ShieldedSoldier: return new ShieldedSoldier();
    case EnemyType::GrenadeSoldier:  return new GrenadeSoldier();
    case EnemyType::BazookaSoldier:  return new BazookaSoldier();
    case EnemyType::Zombie:          return new Zombie();
    case EnemyType::Mummy:           return new Mummy();
    case EnemyType::Marine:          return new Marine();
    case EnemyType::Paratrooper: return new Paratrooper();
    default:                         return nullptr;
    }
}
