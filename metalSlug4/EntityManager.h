//#pragma once
//#ifndef ENTITYMANAGER_H
//#include "Entity.h"
//
//
//
//class EntityManager {
//    Entity** entities;      
//    int entityCount;
//    int entityCapacity;
//public:
//    EntityManager(int capacity);
//    ~EntityManager();
//    void addEntity(Entity* e);
//    void removeEntity(int index);
//    void updateAll(float dt, char** lvl, int cellSize, int w, int h);
//    void drawAll(sf::RenderWindow& window);
//};
//
//#endif // !ENTITYMANAGER_H



// EntityManager.h
#pragma once
#include <SFML/Graphics.hpp>
#include "Soldier.h"

class Level; 

class EntityManager
{
private:
    PlayerSoldier* players;
    int playerCount;
    int activePlayerIndex;

    Level* currentLevel;  

public:
    EntityManager();
    ~EntityManager();

    // Called by PlayState::enter() after level load
    void init(Level* level);

    // Called by PlayState when level changes
    void setLevel(Level* level);

    void handleEvent(sf::Event& ev);
    void update(float dt);
    void render(sf::RenderWindow& window);

    bool allPlayersDead() const;
    void clear();
};
