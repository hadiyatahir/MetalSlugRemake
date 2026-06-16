#include "ScoreManager.h"

void ScoreManager::addScore(int points)
{
    mScore += points;
    if (mScore > mHighScore)
        mHighScore = mScore;
}

void ScoreManager::resetScore()
{
    mScore = 0;
}

int ScoreManager::getScore()     const { return mScore; }
int ScoreManager::getHighScore() const { return mHighScore; }


void ScoreManager::awardMassacre(int killCount)
{
    if (killCount < 3) return;
    int bonus = 300 + (killCount - 3) * 50;
    addScore(bonus);
}