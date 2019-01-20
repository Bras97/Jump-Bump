//
// Created by monika on 02/01/19.
//

#ifndef SERVER_SERVERGAME_H
#define SERVER_SERVERGAME_H

#include "serverConstants.h"

/**
 * Checks if collision happened
 * @param game
 * @param player_number number of the player who potentially killed the other one
 */
void checkCollision(Game *game, int player_number);

/**
 * Adds point to winning player, sends messages about scores and sets respawn position for killed player
 * @param game
 * @param player_number
 */
void getPoint(Game *game, int player_number);

/**
 * Sets player's new position
 * @param player
 */
void newPosition(Player *player);

/**
 * Prepares respawn points and sets players on starting positions
 * @param game
 */
void startGame(Game *game);

/**
 * Manages the game
 * @param data
 * @return
 */
void *playGame(void *t_data);

#endif //SERVER_SERVERGAME_H
