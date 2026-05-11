#pragma once
#include "GameState.h"
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

class LoadingState : public GameState
{

        float timer = 0.f;
        float duration = 2.0f; //timing for loading screen

        sf::Font font;
        sf::Text text;

        sf::Texture loadTex;
        sf::Sprite lbg;

        int nextBiome;
        int screenW = 1600;
        int screenH = 900;

    public:

        LoadingState(int biome, int w = 1600, int h = 900)
        {
            nextBiome = biome;
            screenW = w;
            screenH = h;
            nextState = nullptr;
        }

        void enter() override
        {
            font.loadFromFile("Fonts/metal-slug.ttf");

            text.setFont(font);
            text.setString("Loading...");
            text.setCharacterSize(40);
            text.setFillColor(sf::Color::White);
            text.setPosition(700.f, 450.f);

            loadTex.loadFromFile("Sprites/Menu/loading.png");
            lbg.setTexture(loadTex);
        }

        void update(float dt) override
        {
            timer += dt;

            
            if (timer >= duration && !isDone)
            {
                isDone = true;

                if (nextBiome <= 3)   
                {
                    nextState = new PlayState(screenW, screenH, nextBiome);
                }
                else
                {
                    nextState = nullptr; //to stop the infinti loop at aquatic biome   which infact is not working owrk on it later
                }
            }


        }

        void draw(sf::RenderWindow& window) override
        {
            window.clear(sf::Color::Black);

            window.draw(lbg);
            window.draw(text);
        }

        void handleInput(sf::RenderWindow&) override {}
        void exit() override {}















};