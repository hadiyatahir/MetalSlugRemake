#include "Animation.h"
using namespace sf;

void strcopy(char* dest, const char* src, int maxSize)
{
    int i = 0;

    while (src[i] != '\0' && i < maxSize - 1)
    {
        dest[i] = src[i];
        i++;
    }

    dest[i] = '\0'; // always null-terminate
}

Animation::Animation() : mframeCount(0), mframeRate(12.f), mcurrentFrameIndex(0), mtimeAccumulator(0.f), misLooping(true), misPlaying(false), mfinished(false)
{
    name[0] = '\0';
}
void Animation::setup(const char* animName, IntRect* frames, int frameCount, float frameRate, bool looping)
{
    //yaar iska banado function strcopy manually 

    strcopy(name, animName, 31);
    name[31] = '\0';

    mframeCount = (frameCount > 32) ? 32 : frameCount;
    for (int i = 0; i < mframeCount; i++)
        mframes[i] = frames[i];

    mframeRate = frameRate;
    misLooping = looping;
    reset();
}
void Animation::play()
{
    misPlaying = true;
    mfinished = false;
}
void Animation::stop() {
    misPlaying = false;
}

void Animation::reset()
{
    mcurrentFrameIndex = 0;
    mtimeAccumulator = 0.f;
    mfinished = false;
}
void Animation::update(float dt)
{
    if (!misPlaying || mfinished || mframeCount == 0)
        return;

    mtimeAccumulator += dt;
    float frameDur = 1.f / mframeRate;
    while (mtimeAccumulator >= frameDur)
    {
        mtimeAccumulator -= frameDur;
        mcurrentFrameIndex++;
        if (mcurrentFrameIndex >= mframeCount)
        {
            if (misLooping)
                mcurrentFrameIndex = 0;
            else
            {
                mcurrentFrameIndex = mframeCount - 1;
                mfinished = true;
                misPlaying = false;
            }
        }
    }
}
IntRect Animation::getCurrentFrame() const
{
    if (mframeCount == 0)
        return IntRect(0, 0, 0, 0);

    return mframes[mcurrentFrameIndex];
}
bool Animation::isPlayingAnim() const {
    return misPlaying;
}
bool Animation::isFinished() const {
    return mfinished;
}
bool Animation::isLooping() const {
    return misLooping;
}