//#include "PlayState.h"




//void PlayState::update(float dt)
//{
//    for (int i = 0; i < 4; i++)
//    {
//        players[i].update(dt, lvl, cell_size, width, height);
//    }
//}
//
//
//
//void PlayState::draw(sf::RenderWindow& window)
//{
//    display_level(window, height, width, lvl, wallSprite1, cell_size);
//
//    for (int i = 0; i < 4; i++)
//    {
//        window.draw(players[i].getSprite());
//    }
//}


// PlayState.cpp
#include "PlayState.h"

PlayState::PlayState()
    : score(0), levelComplete(false)
{
    
}

PlayState::~PlayState()
{
    //levelmanger and netity manager have their own destrcutors
}

void PlayState::enter()
{
    levelManager.loadLevel(0);

    Level* currentLevel = levelManager.getCurrentLevel();

    entityManager.init(currentLevel);

    score = 0;
    levelComplete = false;
}

void PlayState::exit()
{
    entityManager.clear();
    //level amanger deleetes in its own destructor
}

void PlayState::handleEvent(sf::Event& ev)
{
    entityManager.handleEvent(ev);
}

void PlayState::update(float dt)
{

    levelManager.update(dt);
    entityManager.update(dt);

    if (levelComplete)
    {
        if (!levelManager.isLastLevel())
        {
            levelManager.nextLevel();
            entityManager.setLevel(levelManager.getCurrentLevel());
            entityManager.clear();
            entityManager.init(levelManager.getCurrentLevel());
            levelComplete = false;
        }
    }
}

void PlayState::render(sf::RenderWindow& window)
{
    levelManager.render(window);   // draws tile grid
    entityManager.render(window);  // draws players 
}