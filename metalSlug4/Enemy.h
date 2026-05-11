/*#pragma once
#include <SFML/Graphics.hpp>
#include <cstring>
#include "Animation.h"

enum class EnemyType
{
    RebelSoldier,
    ShieldedSoldier,
    GrenadeSoldier,
    BazookaSoldier,
    Zombie,
    Mummy,
    Marine
};

class Enemies
{
public:
    Enemies(int dmg, float spd, int hp);
    virtual ~Enemies();

    virtual void update(float dt, char** lvl, int cell_size, int mapWidth, int mapHeight);
    virtual void updateAI(float dt);

    virtual void move(float dt) = 0;
    virtual void attack() = 0;

    void addAnimation(const char* name, const Animation& anim);
    void setAnimation(const char* name);

    Animation* getCurrentAnimation();
    sf::Sprite& getSprite();

    bool isAlive() const;

    static Enemies* createEnemy(EnemyType type);

protected:
    void copyName(char* dest, const char* src, int maxLen);
    void physicsUpdate(float dt, char** lvl, int cell_size, int mapWidth, int mapHeight);

protected:
    int   damage;
    float speed;
    int   health;
    bool  alive;

    int   m_state;
    float m_stateTimer;

    // physics
    float velocityX = 0.f;
    float velocityY = 0.f;
    bool  onGround = false;
    float gravity = 0.8f;

    // collision box offsets/size — tweak per enemy if needed
    float collisionOffsetX = 4.f;
    float collisionOffsetY = 2.f;
    float collisionWidth = 24.f;
    float collisionHeight = 36.f;

    Animation  m_animations[10];
    char       m_animNames[10][32];
    int        m_animCount;

    Animation* m_currentAnimation;

    sf::Sprite  m_sprite;
    sf::Texture m_texture;
};

#pragma once

// Forward declare so EnemyAI.h doesn't pull in all of Animation.h
class Animation;

class EnemyAI
{
public:
    EnemyAI();

    // Updates m_stateTimer and writes a new state (0=idle 1=walk 2=attack)
    // into stateOut. Respects non-looping animations (e.g. attack) so they
    // aren't cut short.
    void update(float dt, int& stateOut, const Animation* currentAnim);

    void reset();

private:
    float m_stateTimer;
};

enum class EnemyType
{
    RebelSoldier,
    ShieldedSoldier,
    GrenadeSoldier,
    BazookaSoldier,
    Zombie,
    Mummy,
    Marine
};

class Enemies
{
public:
    Enemies(int dmg, float spd, int hp);
    virtual ~Enemies();

    virtual void update(float dt, char** lvl, int cell_size, int mapWidth, int mapHeight);
    virtual void updateAI(float dt);      // delegates to m_ai

    virtual void move(float dt) = 0;
    virtual void attack() = 0;

    void addAnimation(const char* name, const Animation& anim);
    void setAnimation(const char* name);

    Animation* getCurrentAnimation();
    sf::Sprite& getSprite();
    bool isAlive() const;

    static Enemies* createEnemy(EnemyType type);

protected:
    void copyName(char* dest, const char* src, int maxLen);
    void physicsUpdate(float dt, char** lvl, int cell_size, int mapWidth, int mapHeight);

protected:
    int   damage;
    float speed;
    int   health;
    bool  alive;

    int   m_state;          // 0 = idle, 1 = walk, 2 = attack

    // composed AI controller
    EnemyAI m_ai;

    // sprite direction
    bool  facingRight;      // enemies default false — sprites naturally face left
    float baseScale;        // subclasses override if needed

    // physics
    float velocityX = 0.f;
    float velocityY = 0.f;
    bool  onGround = false;
    float gravity = 0.8f;

    float collisionOffsetX = 4.f;
    float collisionOffsetY = 2.f;
    float collisionWidth = 24.f;
    float collisionHeight = 36.f;

    Animation  m_animations[10];
    char       m_animNames[10][32];
    int        m_animCount;

    Animation* m_currentAnimation;

    sf::Sprite  m_sprite;
    sf::Texture m_texture;
};*/

#pragma once


#include <SFML/Graphics.hpp>
#include <cstring>
#include "Animation.h"


class Animation;

class EnemyAI
{
public:
    EnemyAI(float detectionRange = 500.f, float attackRange = 70.f);        //dtection range is how close th eplyaer is for enemy to walk towards
                                                                            //attakc range is how close the player is when enemy atacks

  
    // 0 = idle, 1 = walk, 2 = attack
    void update(float dt, int& stateOut, float distToPlayer, const Animation* currentAnim);

    void reset();

    float detectionRange;
    float attackRange;

private:
    float m_stateTimer;
};

enum class EnemyType
{
    RebelSoldier,
    ShieldedSoldier,
    GrenadeSoldier,
    BazookaSoldier,
    Zombie,
    Mummy,
    Marine,
    Paratrooper
};

class Enemies
{
public:
    Enemies(int dmg, float spd, int hp);
    virtual ~Enemies();

    virtual void update(float dt, char** lvl, int cell_size, int mapWidth, int mapHeight, int biomeId);
    virtual void updateAI(float dt);
    virtual void updateCollisionForAnimation() {}

    virtual Enemies* getLandedReplacement() { return nullptr; }

    virtual void move(float dt) = 0;
    virtual void attack() = 0;

   
    void setPosition(float x, float y);         // spawn position
    void setScale(float scale);                  // uniform spr
    void setTarget(sf::Vector2f targetPos);      // call every frame with player position

    void addAnimation(const char* name, const Animation& anim);
    void setAnimation(const char* name);
    void          takeDamage(int dmg);
    int           getScoreValue()  const { return mScoreValue; }
    sf::FloatRect getBounds()      const { return m_sprite.getGlobalBounds(); }
    bool          wasJustKilled()  const { return mJustKilled; }
    void          clearKillFlag() { mJustKilled = false; }

    Animation* getCurrentAnimation();
    sf::Sprite& getSprite();
    bool        isAlive() const;

    static Enemies* createEnemy(EnemyType type);

protected:
    void copyName(char* dest, const char* src, int maxLen);
    void physicsUpdate(float dt, char** lvl, int cell_size, int mapWidth, int mapHeight, int biomeId);
  
    int   mScoreValue = 50;
    bool  mJustKilled = false;

protected:
    int   damage;
    float speed;
    int   health;
    bool  alive;

    int m_state;    // 0 = idle, 1 = walk, 2 = attack

    
    EnemyAI m_ai;

  
    sf::Vector2f m_targetPos;
    bool         m_hasTarget;

    // facing
    bool  facingRight;
    float baseScale;

    // physics
    float velocityX = 0.f;
    float velocityY = 0.f;
    bool  onGround = false;
    float gravity = 0.8f;

    // collision 
    float collisionOffsetX = 4.f;
    float collisionOffsetY = 2.f;
    float collisionWidth = 24.f;
    float collisionHeight = 36.f;

    Animation  m_animations[10];
    char       m_animNames[10][32];
    int        m_animCount;

    Animation* m_currentAnimation;

    sf::Sprite  m_sprite;
    sf::Texture m_texture;
};