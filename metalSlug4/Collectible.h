#pragma once
#include <SFML/Graphics.hpp>
#include "Animation.h"

enum PrisonerAnim { PRIS_TRAPPED = 0, PRIS_FREED, PRIS_ANIM_COUNT };
enum CrateAnim { CRATE_IDLE = 0, CRATE_ANIM_COUNT };

class Prisoner
{
public:
    enum class State { TRAPPED, FREEING, GONE };

    void setup(float x, float y);
    void update(float dt);
    void draw(sf::RenderWindow& window, int camX, int camY);
    void tryFree(sf::FloatRect playerBounds);

    State         getState()      const { return mState; }
    bool          isCrateActive() const { return mCrateActive; }
    sf::FloatRect getCrateBounds() { return mCrateSprite.getGlobalBounds(); }
    void          collectCrate() { mCrateActive = false; }

private:
    void setAnim(PrisonerAnim a);

    // Prisoner
    sf::Texture  mTexture;
    sf::Sprite   mSprite;
    Animation    mAnims[PRIS_ANIM_COUNT];
    PrisonerAnim mCurAnim = PRIS_TRAPPED;
    State        mState = State::TRAPPED;

    // Crate
    sf::Texture  mCrateTexture;
    sf::Sprite   mCrateSprite;
    Animation    mCrateAnims[CRATE_ANIM_COUNT];
    bool         mCrateActive = false;

    float        mProximity = 80.f;
};