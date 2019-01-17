//
// Created by monika on 14/01/19.
//

#ifndef SERVER_SERVERPLAYERFUNCTIONS_H
#define SERVER_SERVERPLAYERFUNCTIONS_H


#include "serverConstants.h"

Player* getOpponent(Player *player);
void getPosition(Player *player);
void funcLogin(Player *player);
void funcList(Player *player);
void funcSendUnavailable(Player *player);
void funcInvite(Player *player);
void funcIsInvited(Player *player);
void startGame(Player *player);

#endif //SERVER_SERVERPLAYERFUNCTIONS_H

