#pragma once

class ScoreManager
{
public:
    static ScoreManager& instance()
    {
        static ScoreManager inst;
        return inst;
    }

   
    void addScore(int points);
    void resetScore();
    int  getScore()     const;
    int  getHighScore() const;

    
    void awardMeleeKill() { addScore(50); }  
    void awardAerialKill() { addScore(100); }
    void awardMultiKill() { addScore(200); }  
    void awardMassacre(int killCount);            

   
    void awardSurvivalClear() { addScore(1000); }
    void awardCampaignClear() { addScore(3000); }
    void awardFlawlessVictory() { addScore(5000); } 

  
    static int scoreForRebelSoldier() { return 50; }
    static int scoreForShieldedSoldier() { return 75; }
    static int scoreForBazookaSoldier() { return 100; }
    static int scoreForGrenadeSoldier() { return 100; }
    static int scoreForParatrooper(int baseInfantryScore) { return baseInfantryScore + 25; }
    static int scoreForMummy() { return 150; }
    static int scoreForZombie() { return 100; }
    static int scoreForMartian() { return 200; }
    static int scoreForNormalBoss() { return 500; }
    static int scoreForUltimateBoss() { return 1500; }

private:
   
    ScoreManager() : mScore(0), mHighScore(0) {}
    int mScore;
    int mHighScore;
};