//#include "DamageableEntity.h"
//#include <SFML/Graphics.hpp>
//using namespace sf;
//
//DamageableEntity::DamageableEntity(int hp) : health(hp), damageState(0), damageTimer(0) {
//}
//
//
//void DamageableEntity::takeDamage(int dmg)
//{
//
//    
//
//    if (!active) return;
//
//    health -= dmg;
//
//    if (damageState == 0)
//    {
//        damageState = 1;       // injured
//        damageTimer = 1.0f;
//    }
//
//    else if (damageState == 1)
//    {
//        damageState = 2;       // critical
//        damageTimer = 1.0f;
//    }
//
//    else
//    {
//        health = 0;
//        destroy();
//    }
//}
//
//
//void DamageableEntity::updateDamage(float dt)
//{
//    if (damageState > 0)
//    {
//        damageTimer -= dt;
//
//        if (damageTimer <= 0)
//        {
//            damageState = 0; // back to original state
//        }
//    }
//}
//
//
//bool DamageableEntity::isAlive() const
//{
//    return health > 0;
//}
//
//
//int DamageableEntity::getDamageState() const
//{
//    return damageState;
//}