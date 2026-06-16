

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
   
    if (currentAnim && !currentAnim->isLooping() && !currentAnim->isFinished())
        return;

    // deciding state from distance instead of random
    if (distToPlayer <= attackRange)
        stateOut = 2;           // close enough to attack
    else if (distToPlayer <= detectionRange)
        stateOut = 1;           //walking towards player
    else
        stateOut = 0;           // out of range 
}


static bool isTileE(char** lvl, int x, int y, int mapWidth, int mapHeight)
{
    if (x < 0 || x >= mapWidth || y < 0 || y >= mapHeight)
        return false;
    return (lvl[y][x] == 'g' || lvl[y][x]=='s');
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


void Enemies::setPosition(float x, float y)
{
    m_sprite.setPosition(x, y);
}

void Enemies::setScale(float scale)
{
    baseScale = scale;

   
    collisionWidth = 24.f * scale;
    collisionHeight = 36.f * scale;
    collisionOffsetX = 4.f * scale;
    collisionOffsetY = 2.f * scale;
}

void Enemies::setTarget(sf::Vector2f targetPos)
{
    m_targetPos = targetPos;
    m_hasTarget = true;
}

void Enemies::takeDamage(int dmg)
{
    if (!alive) return;
    health -= dmg;
    if (health <= 0)
    {
        health = 0;
        alive = false;
        mJustKilled = true;
    }
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

// ---- AI-------------------------------------------

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

void Enemies::physicsUpdate(float dt, char** lvl, int cell_size, int mapWidth, int mapHeight, int biomeId)
{
    /*if (!onGround)
    {
        velocityY += gravity;
        if (velocityY > 20.f) velocityY = 20.f;
    }
    else
    {
        velocityY = 0.f;
    }*/

    if (biomeId == 2)
    {
        // aquatic biome → no gravity
        velocityY *= 0.95f;  // optional floaty damping
    }
    else
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

void Enemies::update(float dt, char** lvl, int cell_size, int mapWidth, int mapHeight, int biomeId)
{
    updateAI(dt);
    move(dt);
    updateCollisionForAnimation();
    physicsUpdate(dt, lvl, cell_size, mapWidth, mapHeight, biomeId);

   
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

/// <summary>
/// /////////////////////////////////////////////////////////////INFATRY//////////////////////
/// </summary>

class Infantry : public Enemies
{
public:
    Infantry(int dmg, float spd, int hp) : Enemies(dmg, spd, hp) {}
    virtual void move(float dt) = 0;
};

/// <summary>
/// //////////////////////////REBELSOLDIER////////////////////////////////////////////////
/// </summary>

class RebelSoldier : public Infantry
{
public:/*
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
    }*/
    EnemyType getType() const override { return EnemyType::RebelSoldier; }
    RebelSoldier() : Infantry(10, 1.0f, 120)
    {
        static sf::Texture sharedTex;
        static bool loaded = false;
        if (!loaded) { sharedTex.loadFromFile("Sprites/normalrebelsoldier.png"); loaded = true; }
        m_sprite.setTexture(sharedTex);
        setupAnimations();
        setAnimation("idle");
        setScale(3.0f);
        collisionWidth = 40.f * 3.f;
        collisionHeight = 38.f * 3.f;
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

/////////////////////////////////////////////////SHILEDED SOLDIER////////////////////////////////////////////////////////

class ShieldedSoldier : public Infantry
{
public:
    EnemyType getType() const override {
        return EnemyType::ShieldedSoldier;
    }
   /* ShieldedSoldier() : Infantry(10, 2.0f, 120)
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
    }*/

    ShieldedSoldier() : Infantry(10, 1.0f, 120)
    {
        static sf::Texture sharedTex;
        static bool loaded = false;
        if (!loaded) { sharedTex.loadFromFile("Sprites/shieldedrebelsoldier.png"); loaded = true; }
        m_sprite.setTexture(sharedTex);
        setupAnimations();
        setAnimation("idle");
        setScale(3.0f);
        collisionWidth = 32.f * 2.75f;
        collisionHeight = 38.f * 2.75f;
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

/// <summary>
/// ///////////////////////////////////////////GREDNADE SOLDEIR/////////////////////////////////////////
/// </summary>

class GrenadeSoldier : public Infantry
{
public:
    EnemyType getType() const override {
        return EnemyType::GrenadeSoldier;
    }
    /*GrenadeSoldier() : Infantry(20, 1.5f, 180)
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
    }*/

    GrenadeSoldier() : Infantry(20, 0.5f, 180)
    {
        static sf::Texture sharedTex;
        static bool loaded = false;
        if (!loaded) { sharedTex.loadFromFile("Sprites/grenaderebelsoldier.png"); loaded = true; }
        m_sprite.setTexture(sharedTex);
        setupAnimations();
        setAnimation("idle");
        setScale(1.25f);
        collisionWidth = 64.f * 1.1f;
        collisionHeight = 91.f * 1.1f;
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


///////////////////////////////////////////BAZOOKA SOKDIER/////////////////////////////////////

class BazookaSoldier : public Infantry
{
public:
    EnemyType getType() const override { return EnemyType::BazookaSoldier; }
   /* BazookaSoldier() : Infantry(20, 1.5f, 80)
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
    }*/

    BazookaSoldier() : Infantry(20, 0.5f, 80)
    {
        static sf::Texture sharedTex;
        static bool loaded = false;
        if (!loaded) { sharedTex.loadFromFile("Sprites/bazookarebelsoldier.png"); loaded = true; }
        m_sprite.setTexture(sharedTex);
        setupAnimations();
        setAnimation("idle");
        setScale(3.0f);
        collisionWidth = 36.f * 3.0f;
        collisionHeight = 36.f * 3.0f;
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

///////////////////////////////////////////UNDEAD///////////////////////

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

/// <summary>
/// ///////////////////////////////////////////ZOMBIE//////////////////////////////////////////
/// </summary>

class Zombie : public Undead
{
public:
    EnemyType getType() const override { return EnemyType::Zombie; }
 /*   Zombie() : Undead(5, 1.0f, 60)
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
    }*/

    Zombie() : Undead(5, 0.5f, 60)
    {
        static sf::Texture sharedTex;
        static bool loaded = false;
        if (!loaded) { sharedTex.loadFromFile("Sprites/zombieenemy.png"); loaded = true; }
        m_sprite.setTexture(sharedTex);
        setupAnimations();
        setAnimation("idle");
        setScale(2.75f);
        collisionWidth = 30.f * 2.75f;
        collisionHeight = 39.f * 2.75f;
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

//////////////////////////////////////////MUMMY//////////////////////////////////////////////////

class Mummy : public Undead
{
public:
    EnemyType getType() const override { return EnemyType::Mummy; }
  /*  Mummy() : Undead(8, 0.8f, 80)
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
    }*/

    Mummy() : Undead(8, 0.5f, 80)
    {
        static sf::Texture sharedTex;
        static bool loaded = false;
        if (!loaded) { sharedTex.loadFromFile("Sprites/mummyenemy.png"); loaded = true; }
        m_sprite.setTexture(sharedTex);
        setupAnimations();
        setAnimation("idle");
        setScale(2.5f);
        collisionWidth = 30.f * 2.5f;
        collisionHeight = 45.f * 2.5f;
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

/////////////////////////////////////////////MARINE//////MARTIAN ACTUALLY///////////////////////////////

class Marine : public Enemies
{
public:
    EnemyType getType() const override { return EnemyType::Marine; }
    bool isHovering() const override { return m_hovering; }

    Marine() : Enemies(15, 1.0f, 100), m_hovering(true), m_landing(false)
    {
        static sf::Texture sharedTex;
        static bool loaded = false;
        if (!loaded) { sharedTex.loadFromFile("Sprites/marineenemy.png"); loaded = true; }

        static sf::Texture sharedHoverTex;
        static bool hoverLoaded = false;
        if (!hoverLoaded) { sharedHoverTex.loadFromFile("Sprites/hovermarine.png"); hoverLoaded = true; }

        m_hoverTexture = &sharedHoverTex;
        m_normalTexture = &sharedTex;

        // Start on hover sheet
        m_sprite.setTexture(*m_hoverTexture);
        setupAnimations();
        setAnimation("hover");
        setScale(2.75f);
        collisionWidth = 49.f * 2.75f;
        collisionHeight = 43.f * 2.75f;
        collisionOffsetX = 4.f;
        collisionOffsetY = 0.f;
    }

    void attack() override { if (!m_hovering) setAnimation("attack"); }

    // Called every frame — suppress normal AI while hovering/landing
    void updateAI(float dt) override
    {
        if (m_hovering || m_landing)
            return;

        Enemies::updateAI(dt);   // normal idle/walk/attack logic resumes
    }

    void move(float dt) override
    {
        if (m_hovering || m_landing)
        {
            velocityX = 0.f;    // frozen in air while UFO phase is active
            return;
        }
        chaseMove(*this, speed, m_state, m_hasTarget, m_targetPos, velocityX, facingRight);
    }

    // takeDamage is intercepted while hovering so shooting the UFO triggers landing
    // In Marine (Enemy.cpp), already shown in previous answer — just confirm it has override:
    void takeDamage(int dmg) override
    {
        if (m_hovering)
        {
            m_hovering = false;
            m_landing = true;
            velocityX = 0.f;
            m_sprite.setTexture(*m_normalTexture);
            setAnimation("land");
            return;
        }
        Enemies::takeDamage(dmg);
    }

    void update(float dt, char** lvl, int cell_size, int mapWidth, int mapHeight, int biomeId) override
    {
        // While hovering: play hover anim, stay frozen, skip physics gravity
        if (m_hovering)
        {
            // Apply scale manually since we skip Enemies::update()
            if (facingRight)
                m_sprite.setScale(-3.f, 3.f);
            else
                m_sprite.setScale(-3.f, 3.f);

            if (m_currentAnimation)
            {
                m_currentAnimation->update(dt);
                m_sprite.setTextureRect(m_currentAnimation->getCurrentFrame());
            }
            return;   // skip physics entirely so it floats in place
        }

        // Landing phase: wait for land animation to finish, then go normal
        if (m_landing)
        {
            if (m_currentAnimation &&
                std::strcmp(m_currentAnimation->name, "land") == 0 &&
                m_currentAnimation->isFinished())
            {
                m_landing = false;
                setAnimation("idle");   // hand off to normal AI from here
            }
        }

        Enemies::update(dt, lvl, cell_size, mapWidth, mapHeight, biomeId);
    }

private:
    bool         m_hovering;
    bool         m_landing;
    sf::Texture* m_hoverTexture;
    sf::Texture* m_normalTexture;

    void setupAnimations()
    {
        // ---- hover frames (hovermarine.png) — fill in your own IntRects ----
        sf::IntRect hover[16];
        hover[0] = sf::IntRect(5, 7, 44, 38);   // TODO: replace with real rects
        hover[1] = sf::IntRect(52, 7, 44, 38);
        hover[2] = sf::IntRect(99, 7, 44, 38);
        hover[3] = sf::IntRect(146, 7, 43, 38);
        hover[4] = sf::IntRect(192, 7, 43, 38);
        hover[5] = sf::IntRect(238, 7, 43, 38);
        hover[6] = sf::IntRect(284, 8, 43, 37);
        hover[7] = sf::IntRect(330, 8, 44, 37);
        hover[8] = sf::IntRect(5, 49, 44, 37);
        hover[9] = sf::IntRect(52, 49, 44, 37);
        hover[10] = sf::IntRect(99, 49, 43, 37);
        hover[11] = sf::IntRect(145, 49, 44, 37);
        hover[12] = sf::IntRect(192, 48, 43, 38);
        hover[13] = sf::IntRect(238, 47, 43, 39);
        hover[14] = sf::IntRect(284, 48, 44, 38);
        hover[15] = sf::IntRect(331, 47, 44, 39);

        // ---- land frames (hovermarine.png) — fill in your own IntRects ----
        sf::IntRect land[16];/*
        land[0] = sf::IntRect(5, 342, 54, 52);
        land[1] = sf::IntRect(70, 346, 54, 48);
        land[2] = sf::IntRect(128, 341, 55, 53);
        land[3] = sf::IntRect(187, 341, 46, 53);
        land[4] = sf::IntRect(237, 342, 44, 52);
        land[5] = sf::IntRect(285, 342, 44, 52);
        land[6] = sf::IntRect(333, 341, 43, 53);*/
        land[0] = sf::IntRect(10, 18, 49, 43);
        land[1] = sf::IntRect(64, 18, 49, 43);
        land[2] = sf::IntRect(118, 19, 48, 42);
        land[3] = sf::IntRect(171, 19, 48, 42);
        land[4] = sf::IntRect(224, 19, 48, 42);
        land[5] = sf::IntRect(277, 19, 48, 42);
        land[6] = sf::IntRect(330, 19, 48, 42);
        land[7] = sf::IntRect(383, 19, 48, 42);
        land[8] = sf::IntRect(436, 19, 48, 42);
        land[9] = sf::IntRect(489, 19, 48, 42);
        land[10] = sf::IntRect(542, 19, 48, 42);
        land[11] = sf::IntRect(595, 19, 48, 42);
        land[12] = sf::IntRect(648, 19, 48, 42);
        land[13] = sf::IntRect(701, 19, 48, 42);
        land[14] = sf::IntRect(754, 19, 48, 42);
        land[15] = sf::IntRect(807, 19, 48, 42);

        Animation a;
        a.setup("hover", hover, 16, 16.f, true);   addAnimation("hover", a);
        a.setup("land", land, 16, 10.f, false);  addAnimation("land", a);

        // ---- normal combat animations (marineenemy.png) ----
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

        a.setup("idle", idle, 16, 16.f, true);   addAnimation("idle", a);
        a.setup("walk", walk, 16, 10.f, true);   addAnimation("walk", a);
        a.setup("attack", attack, 18, 10.f, false);  addAnimation("attack", a);
    }
};

//PARATROOPER HELPER

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

/// <summary>
/// ////////////////////////////////PARATROOPER///////////////////////////////////////////////////////
/// </summary>
class Paratrooper : public Infantry
{
public:
    EnemyType getType() const override { return EnemyType::Paratrooper; }
  /*  Paratrooper()
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
    }*/

    Paratrooper() : Infantry(0, 0.f, 999), m_landAs(randomInfantryType()), m_landed(false), m_landing(false), m_replacement(nullptr)
    {
        static sf::Texture sharedTex;
        static bool loaded = false;
        if (!loaded) { sharedTex.loadFromFile("Sprites/paratrooper.png"); loaded = true; }
        m_sprite.setTexture(sharedTex);
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
            velocityX = 0.f;  
    }

    Enemies* getLandedReplacement() override
    {
        if (!m_landed) return nullptr;
        Enemies* r = m_replacement;
        m_replacement = nullptr;
        return r;
    }

    void update(float dt, char** lvl, int cell_size, int mapWidth, int mapHeight, int biomeId) override
    {
        if (m_landed) return;

        // Cap parachute fall speed
        if (velocityY > 3.f) velocityY = 3.f;   // ← add this line

        Enemies::update(dt, lvl, cell_size, mapWidth, mapHeight, biomeId);

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



    Enemies* Enemies::createEnemy(EnemyType type)
    {
        Enemies* e = nullptr;

        switch (type)
        {
        case EnemyType::RebelSoldier:   e = new RebelSoldier(); break;
        case EnemyType::ShieldedSoldier: e = new ShieldedSoldier(); break;
        case EnemyType::GrenadeSoldier:  e = new GrenadeSoldier(); break;
        case EnemyType::BazookaSoldier:  e = new BazookaSoldier(); break;
        case EnemyType::Zombie:          e = new Zombie(); break;
        case EnemyType::Mummy:           e = new Mummy(); break;
        case EnemyType::Marine:          e = new Marine(); break;
        case EnemyType::Paratrooper:     e = new Paratrooper(); break;
        default: return nullptr;
        }

        switch (type)
        {
        case EnemyType::RebelSoldier:
            e->mScoreValue = 50;
            break;

        case EnemyType::ShieldedSoldier:
            e->mScoreValue = 75;
            break;

        case EnemyType::GrenadeSoldier:
        case EnemyType::BazookaSoldier:
        case EnemyType::Zombie:
            e->mScoreValue = 100;
            break;

        case EnemyType::Mummy:
            e->mScoreValue = 150;
            break;

        case EnemyType::Marine:
            e->mScoreValue = 120;
            break;

        case EnemyType::Paratrooper:
            e->mScoreValue = 200;
            break;
        }

        return e;
    }
