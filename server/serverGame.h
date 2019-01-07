//
// Created by monika on 02/01/19.
//

#ifndef SERVER_SERVERGAME_H
#define SERVER_SERVERGAME_H

#include "serverConstants.h"

void checkCollision(Game *game);
void getPoint(Game *game, int player_number);
void newPosition(Player *player);
void startGame(Game *game);
void *playGame(void *t_data);

#endif //SERVER_SERVERGAME_H
