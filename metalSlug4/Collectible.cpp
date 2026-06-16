#include "Collectible.h"

void Prisoner::setup(float x, float y)
{
    // ---- Prisoner ----
    mTexture.loadFromFile("Sprites/prisoner.png");
    mSprite.setTexture(mTexture);
    mSprite.setPosition(x, y);
    mSprite.setScale(2.f, 2.f);

    sf::IntRect trappedFrames[10] = {
        sf::IntRect(13,12,48,43),
        sf::IntRect(70,12,47,43),
        sf::IntRect(124,12,49,43),
        sf::IntRect(179,15,54,42),
        sf::IntRect(244,12,55,43),
        sf::IntRect(244,12,55,43),
        sf::IntRect(179,15,54,42),
        sf::IntRect(124,12,49,43),
        sf::IntRect(70,12,47,43),
        sf::IntRect(13,12,48,43)
    };
    mAnims[PRIS_TRAPPED].setup("trapped", trappedFrames, 8, 10.f, true);

    sf::IntRect freedFrames[6] = {
        sf::IntRect(244,12,55,43),
        sf::IntRect(314,14,55,42),
        sf::IntRect(384,12,58,45),
        sf::IntRect(446,11,61,48),
        sf::IntRect(518,5,64,50),
        sf::IntRect(587,2,55,54)
    };
    mAnims[PRIS_FREED].setup("freed", freedFrames, 6, 6.f, false);

    setAnim(PRIS_TRAPPED);

    // ---- Crate ----
    mCrateTexture.loadFromFile("Sprites/crate.png");
    mCrateSprite.setTexture(mCrateTexture);
    mCrateSprite.setScale(2.3f, 2.3f);

    // Replace these IntRects with your actual crate sprite sheet frames
    sf::IntRect crateFrames[6] = {
        sf::IntRect(1,3,30,28),
        sf::IntRect(31,3,32,28),
        sf::IntRect(64,1,30,30),
        sf::IntRect(94,0,32,31),
        sf::IntRect(128,1,30,30),
        sf::IntRect(160,2,31,29)
    };
    mCrateAnims[CRATE_IDLE].setup("crate_idle", crateFrames, 6, 8.f, true);

    mCrateAnims[CRATE_IDLE].reset();
    mCrateSprite.setTextureRect(
        mCrateAnims[CRATE_IDLE].getCurrentFrame()
    );


    mCurAnim = PRIS_TRAPPED;
    mAnims[PRIS_TRAPPED].reset();
    mAnims[PRIS_TRAPPED].play();
}

void Prisoner::setAnim(PrisonerAnim a)
{
    if (mCurAnim == a && !mAnims[a].isFinished()) return;
    mCurAnim = a;
    mAnims[a].reset();
    mAnims[a].play();

}

void Prisoner::tryFree(sf::FloatRect playerBounds)
{
    if (mState != State::TRAPPED) return;

    sf::FloatRect trigger(
        mSprite.getGlobalBounds().left - mProximity,
        mSprite.getGlobalBounds().top - mProximity,
        mSprite.getGlobalBounds().width + mProximity * 2.f,
        mSprite.getGlobalBounds().height + mProximity * 2.f
    );

    if (trigger.intersects(playerBounds))
    {
        mState = State::FREEING;
        setAnim(PRIS_FREED);
    }
}

void Prisoner::update(float dt)
{

    // Update crate animation while it sits waiting
    if (mCrateActive)
    {
        mCrateAnims[CRATE_IDLE].update(dt);
        mCrateSprite.setTextureRect(mCrateAnims[CRATE_IDLE].getCurrentFrame());
    }

    if (mState == State::GONE) return;

    // Update prisoner animation
    mAnims[mCurAnim].update(dt);
    mSprite.setTextureRect(mAnims[mCurAnim].getCurrentFrame());

    // Freed animation finished — place crate, remove prisoner
    if (mState == State::FREEING && mAnims[PRIS_FREED].isFinished())
    {
        mCrateSprite.setPosition(mSprite.getPosition());
        mCrateAnims[CRATE_IDLE].reset();
        mCrateAnims[CRATE_IDLE].play();
        mCrateActive = true;
        mState = State::GONE;
    }

   
}

void Prisoner::draw(sf::RenderWindow& window, int camX, int camY)
{
    if (mState != State::GONE)
    {
        sf::Sprite screen = mSprite;
        screen.setPosition(
            mSprite.getPosition().x - camX,
            mSprite.getPosition().y - camY
        );
        window.draw(screen);
    }

    if (mCrateActive)
    {
        sf::Sprite screenCrate = mCrateSprite;
        screenCrate.setPosition(
            mCrateSprite.getPosition().x - camX,
            mCrateSprite.getPosition().y - camY
        );
        window.draw(screenCrate);
    }
}