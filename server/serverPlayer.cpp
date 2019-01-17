//
// Created by monika on 30/12/18.
//

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
#include "serverPlayerFunctions.h"

using namespace std;

void getCommand(Player *player) {
    string newPart = player->buf;
    (player->buf).clear();

    if (newPart.empty()) {
        player->run = false;
        player->playing = false;
        return;
    }

    // gdy juz byl poczatek
    if(!(player->command).empty()) {
        if ((player->command).at(0) == START) {
//            cout << "start\n";
            (player->command).append(newPart.substr(0, newPart.find(END) + 1));
            newPart = newPart.substr(newPart.find(END) + 1,
                                     newPart.size() - (newPart.find(END) + 2));
            (player->buf).append(newPart);
        }
    }

    else {
//        cout << "new\n";
        int startPoint = newPart.find(START);
        int endPoint = newPart.find(END);
//        cout << startPoint << " " << endPoint << " " << newPart.size() <<'\n';

        if(startPoint == -1);

        else if(endPoint == -1) {
            (player->buf).append(newPart.c_str());
        }

        else {
            (player->command).append(newPart.substr(startPoint, endPoint - startPoint + 1));
            if(endPoint < newPart.size() - 1) {
                newPart = newPart.substr(endPoint + 1, newPart.size() - (endPoint + 1));
                (player->buf).append(newPart);
            }

            int firstDelimiter = (player->command).find(DELIMITER);
            if (firstDelimiter != -1) {
                string commandNumber = (player->command).substr(1, firstDelimiter - 1);
                if (commandNumber == "-1") {
                    pthread_mutex_lock(&player->playerMutex);
                    player->run = false;
                    player->playing = false;
                    pthread_mutex_unlock(&player->playerMutex);
                    return;
                }
            }
        }
    }

    if(!player->buf.empty()){
        int pos = player->buf.find('\r');
        if (pos <= player->buf.size()) {
            player->buf.erase();
        }
    }

//    cout << player->login << " ";
//    cout  << "c: " << player->command << " || buf: " << player->buf << " " <<  '\n';
    

}


//funkcja obslugujaca gracza
void *playerPlays (void *t_data)
{
    pthread_detach(pthread_self());

    auto *player = new Player();
    player->fd = ((struct thread_data*)(t_data))->fd;
    player->players_ptr = ((struct thread_data*)(t_data))->players_ptr;
    player->players_ptr->push_back(player);
    player->serverMutex = ((struct thread_data*)(t_data))->mutex;

    std::cout << ">>> create player " << player->fd << '\n';

    pthread_mutex_unlock(player->serverMutex);

    char buffer[BUFFER];

    // logowanie
    while(player-> run && !player->logged) {
        memset(buffer, 0, BUFFER);
        read(player->fd, buffer, BUFFER);
        player->buf.append(string(buffer));
        getCommand(player);

        if(!player->command.empty()) {
            if(player->command.at(0) == START && player->command.at(player->command.size()-1) == END) {

                if (player->command.at(1) == '0') {
                    funcLogin(player);
                }

                player->command.clear();
            }
        }
    }

    while(player -> run) {
        // zanim zacznie grac
        player->opponent = nullptr;
        while (player->run && !player->playing) {
            memset(buffer, 0, BUFFER);
            read(player->fd, buffer, BUFFER);
            player->buf.append(string(buffer));
            getCommand(player);

            if (!player->command.empty()) {
                if (player->command.at(0) == START && player->command.at(player->command.size() - 1) == END) {

                    // wyslij liste graczy
                    if (player->command.at(1) == '1') {
                        funcList(player);
                    }

                        // gracz chce zaprosic gracza2 do gry
                    else if (player->command.at(1) == '2') {
                        funcInvite(player);

                    }

                        // gracz jest zapraszany do gry
                    else if (player->command.at(1) == '3') {
                        funcIsInvited(player);
                    }

                    player->command.clear();
                }
            }
        }

        // przesylanie pozycji
        while (player->run && player->playing) {

            memset(buffer, 0, BUFFER);
            read(player->fd, buffer, BUFFER);
            player->buf.append(string(buffer));
            getCommand(player);

            if (!player->command.empty()) {
                if (player->command.at(0) == START && player->command.at(player->command.size() - 1) == END) {

                    if (player->command.at(1) == '9') {
                        getPosition(player);
                    }

                    player->command.clear();

                }
            }

        }

    }


    pthread_mutex_t *serverMutex = player->serverMutex;

    // gra zostala zakonczona
    pthread_mutex_lock(serverMutex);
    pthread_mutex_lock(&player->playerMutex);
    if(player->opponent != nullptr) {
        char buffer[4];
        string message = "#-1;&";
        strcpy(buffer, message.c_str());
        write(player->opponent->fd, buffer, message.size());
        pthread_mutex_lock(&player->opponent->playerMutex);
        player->opponent->playing = false;
        player->opponent->opponent = nullptr;
        pthread_mutex_unlock(&player->opponent->playerMutex);
    }

    delete player;

    pthread_mutex_unlock(&player->playerMutex);
    pthread_mutex_unlock(serverMutex);

}

