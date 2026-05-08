#pragma once
#include <SFML/Graphics.hpp>
using namespace sf;
class Animation
{
public:
    char name[32]; // animation ka naam
    // + play() + stop() + reset() + update(dt) + getCurrentFrame()
    Animation();
    // Call once to configure the animation.
    // frames[] = array of sf::IntRect (pixel regions on the spritesheet)
    // frameCount = number of entries in that array (max 32)
    // frameRate = frames per second
    // looping = whether it loops
    void setup(const char* animName, IntRect* frames, int frameCount, float frameRate, bool looping);
    void play();
    void stop();
    void reset();
    void update(float dt);
    IntRect getCurrentFrame() const;
    bool isPlayingAnim() const;
    bool isFinished() const;  // true if nonlooping reached last frame

    bool isLooping() const;

private:
    IntRect mframes[32];
    int  mframeCount;
    float mframeRate;
    int mcurrentFrameIndex;
    float mtimeAccumulator;
    bool misLooping;
    bool misPlaying;
    bool mfinished;
};
