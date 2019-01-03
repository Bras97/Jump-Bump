//
// Created by monika on 30/12/18.
//

#ifndef SERVER_SERVERPLAYER_H
#define SERVER_SERVERPLAYER_H

#include "serverConstants.h"

void getCommand(Player *player);
void *playerPlays (void *t_data);

Player* getOpponent(Player *player);
void getPosition(Player *player);
void funcLogin(Player *player);
void funcList(Player *player);
void funcSendUnavailable(Player *player);
void funcInvite(Player *player);
void funcIsInvited(Player *player);


#endif //SERVER_SERVERPLAYER_H
