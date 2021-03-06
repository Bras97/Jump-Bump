//
// Created by monika on 14/01/19.
//

#include "serverPlayerFunctions.h"
#include "serverGame.h"
#include "serverPlayer.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <string>
#include <list>
#include <iostream>

using namespace std;

Player* getOpponent(Player *player){
    int firstDelimiter = player->command.find(DELIMITER);

    string opponentLogin = player->command.substr(firstDelimiter + 1,
                                                  player->command.size() - firstDelimiter - 2);

    for (auto &p : *player->players_ptr) {
        if (p->login == opponentLogin) {
            return p;
        }
    }

    return nullptr;
}

void getPosition(Player *player) {
    pthread_mutex_lock(&player->playerMutex);
    int firstDelimiter = player->command.find(DELIMITER);
    int secondDelimiter = player->command.find(DELIMITER, firstDelimiter+1);
    string pos1 = player->command.substr(firstDelimiter + 1,
                                         secondDelimiter - firstDelimiter - 1);
    string pos2 = player->command.substr(secondDelimiter + 1,
                                         player->command.size() - secondDelimiter - 2);
//    cout << pos1 << " " << pos2 << '\n';
    if(!pos1.empty()) player->position[0] = stoi(pos1);
    if(!pos2.empty()) player->position[1] = stoi(pos2);
    pthread_mutex_unlock(&player->playerMutex);
}

void startGame(Player *player) {
    pthread_mutex_lock(player->serverMutex);
    struct thread_data_game * game_data = new thread_data_game;
    game_data->player[0] = player;
    game_data->player[1] = player->opponent;
    game_data->serverMutex = player->serverMutex;
    pthread_t thread1;
    int create_result = pthread_create(&thread1, NULL, playGame, (void *)game_data);

    if (create_result){
        printf("Błąd przy próbie utworzenia wątku, kod błędu: %d\n", create_result);
        exit(-1);
    }

    pthread_mutex_lock(player->serverMutex);
    delete game_data;
    pthread_mutex_unlock(player->serverMutex);
}

void funcLogin(Player *player) {
    string newLogin = player->command.substr(
            player->command.find(DELIMITER) + 1,
            player->command.find(END) - player->command.find(DELIMITER) - 1);

    bool err = false;

    for (auto &p : *player->players_ptr) {
        if (p->login == newLogin) {
            err = true;
        }
    }

    // when login is already used
    if (err) {
        cout << "[Login is not available] fd: " << player->fd << '\n';
        string message = "#0;0&";
        write(player->fd, message.c_str(), message.size());
    } else {
        cout << "[Logged] fd: " << player->fd << ", login: " << newLogin << '\n';
        pthread_mutex_lock(&player->playerMutex);
        player->login.clear();
        player->login.append(newLogin);
        player->logged = true;
        pthread_mutex_unlock(&player->playerMutex);
        string message = "#0;1&";
        write(player->fd, message.c_str(), message.size());
    }
}

void funcList(Player *player) {
    string message = "#1;";
    for (auto &p : *player->players_ptr) {
        if ((!p->login.empty()) && p->fd != player->fd) {
            message = message + p->login + ":";
        }
    }
    message+=END;
    write(player->fd, message.c_str(), message.size());
}

void funcSendUnavailable(Player *player) {
    string message = "#2;0&";
    write(player->fd, message.c_str(), message.size());
}

void funcInvite(Player *player) {
    Player *opponent = getOpponent(player);

    if (opponent == nullptr) {
        funcSendUnavailable(player);
    }

    else if (opponent->playing) {
        funcSendUnavailable(player);
    }

        // send the invite
    else {
        char buffer[BUFFER];
        string message = "#2;1&";
        write(player->fd, message.c_str(), message.size());

        message.clear();
        message = "#3;" + player->login + END;
        write(opponent->fd, message.c_str(), message.size());

        player->command.clear();

        // wait for the reply
        char reply = '.';
        while (reply == '.') {
            memset(buffer, 0, BUFFER);
            read(player->fd, buffer, BUFFER);
            player->buf.append(string(buffer));
            getCommand(player);

            if(!player->command.empty()) {
                if (player->command.at(0) == START && player->command.at(player->command.size() - 1) == END) {

                    // if he is invited during his wait, send inviter message about being unavailable
                    if (player->command.at(1) == '3') {
                        Player *newPlayer = getOpponent(player);
                        if(newPlayer != nullptr) {
                            funcSendUnavailable(newPlayer);
                        }
                    }

                    // get reply
                    else if (player->command.at(1) == '4') {
                        reply = player->command.at(3);
                    }
                    player->command.clear();
                }
            }
        }

        // check if it was accepted
        if (reply == '1') {
            pthread_mutex_lock(&player->playerMutex);
            player->opponent= opponent;
            player->playing = true;
            pthread_mutex_unlock(&player->playerMutex);
            // start the game
            startGame(player);
        }
    }
}

void funcIsInvited(Player *player) {
    int firstDelimiter = player->command.find(DELIMITER);
    int secondDelimiter = player->command.find(DELIMITER, firstDelimiter + 1);

    string opponentLogin = player->command.substr(firstDelimiter + 1,
                                                  secondDelimiter - firstDelimiter - 1);
    Player *newOpponent = nullptr;

    for (auto &p : *player->players_ptr) {
        if (p->login == opponentLogin) {
            newOpponent = p;
        }
    }

    char reply = player->command.at(secondDelimiter + 1);
    if(newOpponent != nullptr) {
        // if player accepted invitation
        if (reply == '1') {
            pthread_mutex_lock(&player->playerMutex);
            player->opponent = newOpponent;
            player->playing = true;
            pthread_mutex_unlock(&player->playerMutex);
            string message = "#4;1&";
            write(newOpponent->fd, message.c_str(), message.size());

        } else {
            string message = "#4;0&";
            write(newOpponent->fd, message.c_str(), message.size());

        }
    }
}
