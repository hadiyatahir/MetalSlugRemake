#pragma once
#ifndef ENTITY_H
#define ENTITY_H
#include <SFML/Graphics.hpp>

class Entity {
protected:
    float x, y;
    sf::Sprite sprite;
public:
    virtual void update(float dt, char** lvl, int cellSize, int w, int h) = 0;
    virtual void draw(sf::RenderWindow& window) = 0;
    sf::Sprite& getSprite() { return sprite; }
    void setPosition(float px, float py);
    virtual ~Entity() {}
};


/////////////////////////////////////////////damageable entity//////////////////////////////////////////////////

class DamagableEntity : public Entity {
protected:
    int hp;
    int maxHp;
    enum DamageState { HEALTHY, INJURED, CRITICAL, DEAD } damageState;
    float damageTimer;
public:
    virtual void takeDamage(int amount);
    bool isDead() const { return damageState == DEAD; }
};
//
//class Soldier : public DamagableEntity {
//protected:
//    Weapon* currentWeapon;
//    Weapon* pistol;       // always available
//    int grenades;
//    float speed;
//public:
//    virtual void fire() = 0;
//    virtual void throwGrenade() = 0;
//    virtual void melee() = 0;
//};
//
//class PlayerSoldier : public Soldier {
//    int characterID;     // 0=Marco, 1=Tarma, 2=Eri, 3=Fio
//    TransformationState* transformState;  // State Pattern
//    bool specialActive;
//    float specialTimer;
//public:
//    PlayerSoldier(int id);
//    void update(float dt, char** lvl, int cellSize, int w, int h) override;
//    void draw(sf::RenderWindow& window) override;
//    void fire() override;
//    void throwGrenade() override;
//    void melee() override;
//    void activateSpecial();
//};

#endif // !ENTITY_H
