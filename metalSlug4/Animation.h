#pragma once
#include <SFML/Graphics.hpp>
using namespace sf;



class Animation
{
public:
    char name[32]; // animation ka naam
   
    Animation();
   
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
