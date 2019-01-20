//
// Created by monika on 14/01/19.
//

#ifndef SERVER_SERVERPLAYERFUNCTIONS_H
#define SERVER_SERVERPLAYERFUNCTIONS_H


#include "serverConstants.h"

/**
 *
 * @param player
 * @return pointer to the opponent
 */
Player* getOpponent(Player *player);

/**
 * Set player's position
 * @param player
 */
void getPosition(Player *player);

/**
 * Manages logging in. Checks if the login is available and sends back information about it.
 * If login is available, player logs in.
 * @param player
 */
void funcLogin(Player *player);

/**
 * Sends list of all players connected to server
 * @param player
 */
void funcList(Player *player);

/**
 * Sends message about being unvailable to play
 * @param player
 */
void funcSendUnavailable(Player *player);

/**
 * Sends invitation to another player and wait for the reply
 * @param player
 */
void funcInvite(Player *player);

/**
 * Sends a reply to the invitation
 * @param player
 */
void funcIsInvited(Player *player);

/**
 * Starts a game's thread. It's called by the player who send the invitation, when it was accepted
 * @param player
 */
void startGame(Player *player);

#endif //SERVER_SERVERPLAYERFUNCTIONS_H

