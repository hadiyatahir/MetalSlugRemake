#pragma once
#ifndef ENTITYMANAGER_H
#include "Entity.h"



class EntityManager {
    Entity** entities;      
    int entityCount;
    int entityCapacity;
public:
    EntityManager(int capacity);
    ~EntityManager();
    void addEntity(Entity* e);
    void removeEntity(int index);
    void updateAll(float dt, char** lvl, int cellSize, int w, int h);
    void drawAll(sf::RenderWindow& window);
};

#endif // !ENTITYMANAGER_H
