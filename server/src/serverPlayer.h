//
// Created by monika on 30/12/18.
//

#ifndef SERVER_SERVERPLAYER_H
#define SERVER_SERVERPLAYER_H

#include "serverConstants.h"

/**
 * Manages received messages
 * @param player
 */
void getCommand(Player *player);

/**
 * Manages the player
 * @param t_data
 * @return
 */
void *playerPlays (void *t_data);




#endif //SERVER_SERVERPLAYER_H
