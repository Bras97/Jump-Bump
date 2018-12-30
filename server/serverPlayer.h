//
// Created by monika on 30/12/18.
//
#include <string>

#ifndef SERVER_SERVERPLAYER_H
#define SERVER_SERVERPLAYER_H

void getCommand(std::string *buf, std::string *command);
void *playerPlays (void *t_data);

#endif //SERVER_SERVERPLAYER_H
