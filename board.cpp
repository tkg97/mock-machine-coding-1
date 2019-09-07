#include <fstream>
#include <iostream>
#include <random>

#include "Board.h"

std::default_random_engine generator;
std::uniform_int_distribution<int> distribution(1, 6);

Board::Board(std::string filePath)
{
    std::ifstream file;
    file.open(filePath);
    if (file.is_open())
    {
        int numSnakes, numLadders, numPlayers;
        file >> this->size;
        file >> numSnakes;
        for (int i = 0; i < numSnakes; i++)
        {
            int low, high;
            file >> high >> low;
            Snake_Ladder *s = new Snake(low, high);
            components.push_back(s);
        }
        file >> numLadders;
        for (int i = 0; i < numLadders; i++)
        {
            int low, high;
            file >> low >> high;
            Snake_Ladder *s = new Ladder(low, high);
            components.push_back(s);
        }
        file >> numPlayers;
        for (int i = 0; i < numPlayers; i++)
        {
            std::string name;
            file >> name;
            GamePlayer *p = new GamePlayer(name);
            players.push_back(p);
        }
    }
}

void Board::startGame()
{
    int activePlayers = players.size();
    while (activePlayers > 1)
    {
        for (int i = 0; i < players.size(); i++)
        {
            if (!players[i]->getActiveStatus())
                continue;

            int k = players[i]->rollDice();
            int pos_init = players[i]->getCurrPosition();
            for (Snake_Ladder *comp : components)
            {
                int modified_pos = comp->modifyPosition(pos_init + k);
                if (modified_pos > this->size)
                {
                    // it means, we are landing outside the board, and this can't be due to any component
                    // it can only be due to the number thrown on the dice
                    // Thus we don't need to do any update in the current postion of the player
                    break;
                }
                players[i]->updatePosition(modified_pos);
                if (modified_pos != (pos_init + k))
                {
                    break;
                }
            }
            int pos_final = players[i]->getCurrPosition();

            std::cout << players[i]->getName() << " rolled a " << k << " and moved from " << pos_init << " to " << pos_final << std::endl;

            if (pos_final == this->size)
            {
                std::cout << "-------------------------------------------------------" << std::endl;
                std::cout << players[i]->getName() << " wins among the currently active players" << std::endl;
                std::cout << "-------------------------------------------------------" << std::endl;
                players[i]->setActiveStatus(false);
                activePlayers--;
                if (activePlayers <= 1)
                    break;
            }
        }
    }
}

void Board::restartGame()
{
    for (GamePlayer *p : players)
    {
        p->updatePosition(0);
        p->setActiveStatus(true);
    }
}

int GamePlayer::rollDice()
{
    return distribution(generator);
}