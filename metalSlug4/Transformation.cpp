#include "TransformationState.h"
#include "Soldier.h"

// ---- NormalState ----
void NormalState::enter(PlayerSoldier& soldier)
{
    soldier.restoreNormalForm();   // restores texture + animations
}

// ---- UndeadState ----
void UndeadState::enter(PlayerSoldier& soldier)
{
    mTimer = 10.f;
    soldier.applyUndeadVisuals();   // swap texture + animations
    soldier.forcePistol();          // lock to pistol
}

void UndeadState::exit(PlayerSoldier& soldier)
{
    soldier.restoreNormalForm();
}

void UndeadState::update(float dt, PlayerSoldier& soldier)
{
    mTimer -= dt;
    if (mTimer <= 0.f)
        soldier.setTransformState(new NormalState());
}

// ---- MummyState ----
void MummyState::enter(PlayerSoldier& soldier)
{
    mTimer = 10.f;
    soldier.applyMummyVisuals();    // swap texture + animations
    soldier.disarm();               // remove all weapons
}

void MummyState::exit(PlayerSoldier& soldier)
{
    soldier.restoreNormalForm();
}

void MummyState::update(float dt, PlayerSoldier& soldier)
{
    mTimer -= dt;
    if (mTimer <= 0.f)
        soldier.setTransformState(new NormalState());
}