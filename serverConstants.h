//
// Created by monika on 29/12/18.
//

#include <list>
#include <string>

#ifndef JUMP_BUMP_CONSTANTS_H
#define JUMP_BUMP_CONSTANTS_H

#define SERVER_PORT 1234
#define QUEUE_SIZE 5

#define DELIMITER ';'
#define START '#'
#define END '&'
#define BUFFER 20

pthread_mutex_t mutex_server = PTHREAD_MUTEX_INITIALIZER;

class Player {
public:
    std::string login;
    bool logged;
    std::string command;
    std::string buf;
    int fd;
    std::list <Player *> *players_ptr;

    Player() {
        login.clear();
        buf.clear();
        command.clear();
        logged = false;
    }
    virtual ~Player() {}
};

class Game {
public:
    int *fd_player1;
    int *fd_player2;
    int score_player1;
    int score_player2;
};

struct thread_data
{
    int fd;
    std::list <Player *> *players_ptr;
};

#endif //JUMP_BUMP_CONSTANTS_H


