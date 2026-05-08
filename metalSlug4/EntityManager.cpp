//#include "EntityManager.h"
//
//EntityManager::EntityManager() : count(0)
//{
//    for (int i = 0; i < MAX_ENTITIES; i++)
//        entities[i] = nullptr;
//}
//
//EntityManager::~EntityManager()
//{
//    // We own every entity pointer — delete them all
//    for (int i = 0; i < count; i++)
//    {
//        delete entities[i];
//        entities[i] = nullptr;
//    }
//    count = 0;
//}
//
//// ============================================================
//// addEntity
//// ============================================================
//// Store the pointer at the next available slot.
//// We take ownership — the caller must NOT delete the entity.
//// ============================================================
//void EntityManager::addEntity(Entity* e)
//{
//    if (count >= MAX_ENTITIES) return; // no room — drop it
//    entities[count] = e;
//    count++;
//}
//
//// ============================================================
//// update
//// ============================================================
//// Call update() on every active entity.
//// Passing Level* lets entities do their own collision checks
//// (e.g. player checks if the tile below is solid = gravity).
//// After updating all, call cleanup() to remove dead ones.
//// ============================================================
//void EntityManager::update(float dt, Level* level)
//{
//    for (int i = 0; i < count; i++)
//    {
//        if (entities[i] != nullptr && entities[i]->isActive())
//            entities[i]->update(dt, level);
//    }
//    cleanup();
//}
//
//// ============================================================
//// draw
//// ============================================================
//// Draw every active entity.
//// camX/camY come from LevelManager — entities subtract them
//// to convert world position -> screen position.
//// ============================================================
//void EntityManager::draw(sf::RenderWindow& window, int camX, int camY)
//{
//    for (int i = 0; i < count; i++)
//    {
//        if (entities[i] != nullptr && entities[i]->isActive())
//            entities[i]->draw(window, camX, camY);
//    }
//}
//
//// ============================================================
//// cleanup — compact the array
//// ============================================================
//// This removes "holes" left by deactivated entities.
//// We do a two-pointer compaction:
////   - 'write' starts at 0
////   - 'read' scans every slot
////   - if entities[read] is active: copy to entities[write], write++
////   - if entities[read] is inactive: delete it, don't copy
////
//// After the loop: slots 0..write-1 are all active.
//// Slots write..count-1 are garbage — zero them out.
//// Set count = write.
////
//// WHY this approach?
////   We need O(n) time and no extra memory.
////   The order of entities doesn't matter for a game loop,
////   so compacting (which changes order) is fine.
//// ============================================================
//void EntityManager::cleanup()
//{
//    int write = 0;
//
//    for (int read = 0; read < count; read++)
//    {
//        if (entities[read] == nullptr) continue;
//
//        if (entities[read]->isActive())
//        {
//            // Keep this entity — move it to the write position
//            entities[write] = entities[read];
//            write++;
//        }
//        else
//        {
//            // Dead entity — delete it and free the memory
//            delete entities[read];
//            entities[read] = nullptr;
//        }
//    }
//
//    // Zero out the now-unused tail of the array
//    for (int i = write; i < count; i++)
//        entities[i] = nullptr;
//
//    count = write;
//}
