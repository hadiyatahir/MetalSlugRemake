#pragma once

class ScoreManager
{
public:
    // Singleton accessor — one global instance, created on first call
    static ScoreManager& instance()
    {
        static ScoreManager inst;  // constructed once, lives for the program's lifetime
        return inst;
    }

    // ── Core methods ─────────────────────────────────────────────
    void addScore(int points);
    void resetScore();
    int  getScore()     const;
    int  getHighScore() const;

    // ── Bonus kill feats ─────────────────────────────────────────
    // Each one just calls addScore() with the right value.
    // Inline here so no .cpp entry needed for these simple ones.
    void awardMeleeKill() { addScore(50); }   // knife/melee kill
    void awardAerialKill() { addScore(100); }   // kill while airborne
    void awardMultiKill() { addScore(200); }   // exactly 2 kills with 1 explosive
    void awardMassacre(int killCount);            // 3+ kills — needs formula, defined in .cpp

    // ── Level clear bonuses ──────────────────────────────────────
    void awardSurvivalClear() { addScore(1000); }
    void awardCampaignClear() { addScore(3000); }
    void awardFlawlessVictory() { addScore(5000); } // no damage taken the whole level

    // ── Per-enemy score values ───────────────────────────────────
    // Static so enemy subclasses can call them WITHOUT needing an instance.
    // Keeps the values in one place — change here, changes everywhere.
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
    // Private constructor — prevents anyone from making a second instance
    ScoreManager() : mScore(0), mHighScore(0) {}
    int mScore;
    int mHighScore;
};