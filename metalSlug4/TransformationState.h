#pragma once

class PlayerSoldier;  // forward declare

class TransformationState
{
public:
    virtual ~TransformationState() = default;

    virtual void enter(PlayerSoldier& soldier) = 0;
    virtual void exit(PlayerSoldier& soldier) = 0;
    virtual void update(float dt, PlayerSoldier& soldier) = 0;

    virtual float getSpeedMultiplier() const { return 1.f; }
    virtual bool  canShoot()          const { return true; }
    virtual bool  canCycleWeapon()    const { return true; }
    virtual bool  hasMelee()          const { return false; }
};

// Normal
class NormalState : public TransformationState
{
public:
    void enter(PlayerSoldier& soldier)  override;
    void exit(PlayerSoldier& soldier)   override {}
    void update(float dt, PlayerSoldier& soldier) override {}
};

//zombie or undead
class UndeadState : public TransformationState
{
public:
    void enter(PlayerSoldier& soldier)  override;
    void exit(PlayerSoldier& soldier)   override;
    void update(float dt, PlayerSoldier& soldier) override;

    float getSpeedMultiplier() const override { return 0.5f; }
    bool  canShoot()           const override { return true; }   // pistol only
    bool  canCycleWeapon()     const override { return false; }  // locked to pistol
    bool  hasMelee()           const override { return false; }
    //bool hasMelee()      override { return false; }

private:
    float mTimer = 10.f;
};

//mummy
class MummyState : public TransformationState
{
public:
    void enter(PlayerSoldier& soldier)  override;
    void exit(PlayerSoldier& soldier)   override;
    void update(float dt, PlayerSoldier& soldier) override;

    float getSpeedMultiplier() const override { return 0.5f; }
    bool  canShoot()           const override { return false; }  // no guns
    bool  canCycleWeapon()     const override { return false; }
    bool  hasMelee()           const override { return true; }   // melee only

private:
    float mTimer = 10.f;
};