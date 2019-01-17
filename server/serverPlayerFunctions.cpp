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

    char buffer[BUFFER];
    string newLogin = player->command.substr(
            player->command.find(DELIMITER) + 1,
            player->command.find(END) - player->command.find(DELIMITER) - 1);

    bool err = false;

    for (auto &p : *player->players_ptr) {
        if (p->login == newLogin) {
            err = true;
        }
    }

    memset(buffer, 0, BUFFER);

    // gdy login juz istnieje
    if (err) {
        cout << "[Loggin is not available] fd: " << player->fd << '\n';
        string message = "#0;0&";
        strcpy(buffer, message.c_str());
        write(player->fd, buffer, message.size());
    } else {
        cout << "[Logged] fd: " << player->fd << ", login: " << newLogin << '\n';
        pthread_mutex_lock(&player->playerMutex);
        player->login.clear();
        player->login.append(newLogin);
        player->logged = true;
        pthread_mutex_unlock(&player->playerMutex);
        string message = "#0;1&";
        strcpy(buffer, message.c_str());
        write(player->fd, buffer, message.size());
    }
}

void funcList(Player *player) {
    char buffer[BUFFER];

    string s = "#1;";
    strcpy(buffer, s.c_str());
    write(player->fd, buffer, 3);
    memset(buffer, 0, BUFFER);
    for (auto &p : *player->players_ptr) {
        if ((!p->login.empty()) && p->fd != player->fd) {
            string message = p->login + ':';
            strcpy(buffer, message.c_str());
            write(player->fd, buffer, message.size());
            memset(buffer, 0, BUFFER);
        }
    }
    s = "&";
    strcpy(buffer, s.c_str());
    write(player->fd, buffer, 1);
}

void funcSendUnavailable(Player *player) {
    char buffer[5];
    string message = "#2;0&";
    strcpy(buffer, message.c_str());
    write(player->fd, buffer, message.size());
}

void funcInvite(Player *player) {
    char buffer[BUFFER];

    Player *opponent = getOpponent(player);

    if (opponent == nullptr) {
        funcSendUnavailable(player);
    }

    else if (opponent->playing) {
        funcSendUnavailable(player);
    }

        // wysyla zaproszenie
    else {
        string message = "#2;1&";
        strcpy(buffer, message.c_str());
        write(player->fd, buffer, message.size());
        memset(buffer, 0, BUFFER);

        message.clear();
        message.append("#3;");
        message.append(player->login);
        message.append("&");
        strcpy(buffer, message.c_str());
        write(opponent->fd, buffer, message.size());

        player->command.clear();

        // czeka na odpowiedz
        char reply = '.';
        while (reply == '.') {
            memset(buffer, 0, BUFFER);
            read(player->fd, buffer, BUFFER);
            player->buf.append(string(buffer));
            getCommand(player);

            if(!player->command.empty()) {
                if (player->command.at(0) == START && player->command.at(player->command.size() - 1) == END) {

                    // w miedzyczasie ktos go zaprasza
                    if (player->command.at(1) == '3') {
                        Player *newPlayer = getOpponent(player);
                        if(newPlayer != nullptr) {
                            funcSendUnavailable(newPlayer);
                        }
                    }

                    else if (player->command.at(1) == '4') {
                        reply = player->command.at(3);
                    }

                    player->command.clear();
                }
            }

        }

        //zgodzil sie na gre
        if (reply == '1') {
            pthread_mutex_lock(&player->playerMutex);
            player->opponent= opponent;
            player->playing = true;
            pthread_mutex_unlock(&player->playerMutex);
            // start watku gry (do kolizji)
            startGame(player);

        }
    }
}

void funcIsInvited(Player *player) {
    char buffer[BUFFER];

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
    // zgadza sie na gre
    if(newOpponent != nullptr) {
        if (reply == '1') {
            pthread_mutex_lock(&player->playerMutex);
            player->opponent = newOpponent;
            player->playing = true;
            pthread_mutex_unlock(&player->playerMutex);
            string message = "#4;1&";
            strcpy(buffer, message.c_str());
            write(newOpponent->fd, buffer, message.size());

        } else {
            string message = "#4;0&";
            strcpy(buffer, message.c_str());
            write(newOpponent->fd, buffer, message.size());
            memset(buffer, 0, BUFFER);
        }
    }
}
