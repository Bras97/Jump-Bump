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

#define BUNNY_HEIGHT 40
#define BUNNY_WIDTH 50

#define MAX_POINTS 5


class Player {
public:
    std::string login;
    bool run;
    bool logged;
    bool playing;
    std::string command;
    std::string buf;
    int fd;
    Player *opponent;
    std::list <Player *> *players_ptr;
    pthread_mutex_t *serverMutex;
    pthread_mutex_t playerMutex;
    int position[2];

    Player();
     ~Player();
};

class Game {
public:
    Player *player[2];
    int score[2];
    int positions[2][2];

    explicit Game(Player *players[2]);
     ~Game();
};

struct thread_data
{
    int fd;
    std::list <Player *> *players_ptr;
    pthread_mutex_t *mutex;
};

struct thread_data_game
{
    Player *player[2];
    pthread_mutex_t *serverMutex;
};

#endif //JUMP_BUMP_CONSTANTS_H


