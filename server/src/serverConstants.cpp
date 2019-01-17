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
    playerMutex = PTHREAD_MUTEX_INITIALIZER;
}

Player::~Player() {
    std::cout << ">>> delete player " << fd <<  " " << login <<'\n';
    players_ptr->remove(this);
    pthread_mutex_destroy(&playerMutex);
    close(fd);
}

Game::Game(Player *players[2]) {
    player[0] = players[0];
    player[1] = players[1];
    score[0] = 0;
    score[1] = 0;
    positions[0][0] = 0;
    positions[0][1] = 0;
    positions[1][0] = 0;
    positions[1][1] = 0;
}

Game::~Game() {
    player[0]->playing = false;
    player[1]->playing = false;

    pthread_mutex_unlock(&player[0]->playerMutex);
    pthread_mutex_unlock(&player[1]->playerMutex);

}
