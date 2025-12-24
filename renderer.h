#ifndef RENDERER_H
#define RENDERER_H

#include "game_types.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

void render(sf::RenderWindow& window, const Bird& bird, 
            const std::vector<Pipe>& pipes, int score, 
            int highScore, GameState state, const sf::Font& font);

#endif

