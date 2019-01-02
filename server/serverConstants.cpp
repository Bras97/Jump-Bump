//
// Created by monika on 02/01/19.
//

#include "serverConstants.h"
#include <string.h>
#include <unistd.h>
#include <netdb.h> 
#include <iostream>

using namespace std;

Player::Player() {
    run = true;
    login.clear();
    buf.clear();
    command.clear();
    logged = false;
    playing = false;
    opponent = nullptr;
}

Player::~Player() {
    std::cout << "delete player " << fd << '\n';
    players_ptr->remove(this);
    pthread_mutex_unlock(mutex);
    if(opponent != nullptr) {
        char buffer[4];
        string message = "#-1&";
        strcpy(buffer, message.c_str());
        write(opponent->fd, buffer, message.size());
    }
}
