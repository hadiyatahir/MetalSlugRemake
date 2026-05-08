//#pragma once
//#include <SFML/Graphics.hpp>
//
//
//
//class Entity
//{
//protected:
//    float x, y;           // world position (top-left of bounding box)
//    float w, h;           // bounding box size in pixels
//    bool  active;         // false = ready to be removed
//
//public:
//    Entity(float x, float y, float w, float h)
//        : x(x), y(y), w(w), h(h), active(true) {
//    }
//
//    virtual ~Entity() {}
//
//    // Every entity MUST implement these three
//    virtual void handleInput() {}  // optional — only player uses this
//    virtual void update(float dt, class Level* level) = 0;
//    virtual void draw(sf::RenderWindow& window, int camX, int camY) = 0;
//    bool  isActive()  const { return active; }
//    void  deactivate() { active = false; }
//
//    float getX()      const { return x; }
//    float getY()      const { return y; }
//    float getW()      const { return w; }
//    float getH()      const { return h; }
//
//    // Convenience: right edge and bottom edge of bounding box
//    float getRight()  const { return x + w; }
//    float getBottom() const { return y + h; }
//
//    // --------------------------------------------------------
//    // Simple AABB (Axis-Aligned Bounding Box) collision check
//    // Returns true if THIS entity overlaps with OTHER entity.
//    // AABB = rectangle overlap: do the boxes intersect?
//    // --------------------------------------------------------
//    bool overlaps(const Entity* other) const
//    {
//        return x < other->getRight() &&
//            getRight()  > other->x &&
//            y < other->getBottom() &&
//            getBottom() > other->y;
//    }
//};
//
//
//// ============================================================
//// DamagableEntity.h  —  Entities that have HP
//// ============================================================
//// Separating this from Entity is important because some
//// entities (like particles, decorations) don't have health.
//// Only Soldiers, Enemies, and Vehicles need HP logic.
////
//// 'die()' is still pure virtual — each subclass decides
//// what happens on death (drop item, play sound, etc.)
//// ============================================================
//
//class DamagableEntity : public Entity
//{
//protected:
//    int hp;
//    int maxHp;
//
//public:
//    DamagableEntity(float x, float y, float w, float h, int maxHp)
//        : Entity(x, y, w, h), maxHp(maxHp), hp(maxHp) {
//    }
//
//    virtual ~DamagableEntity() {}
//
//    // --------------------------------------------------------
//    // takeDamage: reduce HP, call die() if <= 0
//    // Virtual so subclasses can override (e.g. shielded enemy
//    // ignores frontal damage — it overrides takeDamage).
//    // --------------------------------------------------------
//    virtual void takeDamage(int amount)
//    {
//        hp -= amount;
//        if (hp <= 0)
//        {
//            hp = 0;
//            die();
//        }
//    }
//
//    virtual void die() = 0;  // subclass decides what death means
//
//    bool isDead()    const { return hp <= 0; }
//    int  getHp()     const { return hp; }
//    int  getMaxHp()  const { return maxHp; }
//};



#pragma once
#ifndef ENTITY_H
#define ENTITY_H
#include <SFML/Graphics.hpp>
#include "Level.h"
using namespace sf;

class Entity {
protected:
    float x;         //positon of any object
    float y;

    bool active;     //tells whether object is present or not

public:
    Entity();

    virtual void update(float dt, Level* level) = 0;    //pure virtual function (base class meaning that each child class will have its own update function)

    virtual void draw(sf::RenderWindow& window, int camX, int camY) = 0;   //same case

    void setPosition(float px, float py);


    float getX() const;
    float getY() const;

    bool isActive() const;

    void destroy();

    virtual ~Entity();

};









#endif // !ENTITY_H

