//#pragma once
//#include "Entity.h"
//#include <SFML/Graphics.hpp>
//
//
//
//class EntityManager
//{
//    static const int MAX_ENTITIES = 200;
//
//    Entity* entities[MAX_ENTITIES];
//    int     count;   // how many slots are currently used
//
//public:
//    EntityManager();
//    ~EntityManager();
//
//    // Add a new entity (must be heap-allocated, we take ownership)
//    void addEntity(Entity* e);
//
//    // Run update on all active entities
//    void update(float dt, class Level* level);
//
//    // Draw all active entities
//    void draw(sf::RenderWindow& window, int camX, int camY);
//
//    // Remove inactive entities and compact the array
//    void cleanup();
//
//    // Access for collision checks between entities
//    int     getCount()               const { return count; }
//    Entity* getEntity(int i)         const { return entities[i]; }
//};
