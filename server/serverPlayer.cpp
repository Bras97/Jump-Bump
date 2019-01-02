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
#include "serverConstants.h"

using namespace std;

void getCommand(string *buf, string *command) {

    string newPart = *buf;
    (*buf).clear();

    // gdy juz byl poczatek
    if(!(*command).empty()) {
        if ((*command).at(0) == START) {
//            cout << "start\n";
            (*command).append(newPart.substr(0, newPart.find(END) + 1));
            newPart = newPart.substr(newPart.find(END) + 1,
                                     newPart.size() - (newPart.find(END) + 2));
            (*buf).append(newPart);
        }
    }

    else {
//        cout << "new\n";
        (*command).clear();
        int startPoint = newPart.find(START);
        int endPoint = newPart.find(END);
//        cout << startPoint << " " << endPoint << " " << newPart.size() <<'\n';

        if(startPoint == -1) {
        }

        else if(endPoint == -1) {
            (*command).append(newPart);
        }

        else {
            (*command).append(newPart.substr(startPoint, endPoint + 1 - startPoint));
            newPart = newPart.substr(endPoint+1, newPart.size()-(endPoint+1));
            (*buf).append(newPart);
        }
//        cout << *command << " " << *buf << '\n';
    }


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
        string message = "#0;0&";
        strcpy(buffer, message.c_str());
        write(player->fd, buffer, message.size());
    } else {
        cout << newLogin << '\n';
        player->login.append(newLogin);
        player->logged = true;
        string message = "#0;1&";
        strcpy(buffer, message.c_str());
        write(player->fd, buffer, message.size());
    }
}

void funcList(Player *player) {
    char buffer[BUFFER];

    string s = "#1";
    strcpy(buffer, s.c_str());
    write(player->fd, buffer, 2);
    memset(buffer, 0, BUFFER);
    for (auto &p : *player->players_ptr) {
        if ((!p->login.empty()) && p->fd != player->fd) {
            string message = ';' + p->login;
            strcpy(buffer, message.c_str());
            write(player->fd, buffer, message.size());
            memset(buffer, 0, BUFFER);
        }
    }
    s = "&";
    strcpy(buffer, s.c_str());
    write(player->fd, buffer, 1);
}

void funcInvite(Player *player) {
    char buffer[BUFFER];

    int opponentFd = -1;

    string opponentLogin = player->command.substr(player->command.find(DELIMITER) + 1,
                                                  player->command.size() - player->command.find(DELIMITER) - 2);
    if(opponentLogin == player->login) {
        string message = "#2;0&";
        strcpy(buffer, message.c_str());
        write(player->fd, buffer, message.size());
        return;
    }

    for (auto &p : *player->players_ptr) {
        if (p->login == opponentLogin) {
            if (!p->playing) {
                opponentFd = p->fd;
            }
            break;
        }
    }

    if (opponentFd == -1) {
        string message = "#2;0&";
        strcpy(buffer, message.c_str());
        write(player->fd, buffer, message.size());
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
        write(opponentFd, buffer, message.size());

        player->command.clear();

        // czeka na odpowiedz
        char reply = '.';
        while (reply == '.') {
            memset(buffer, 0, BUFFER);
            read(player->fd, buffer, BUFFER);
            player->buf.append(string(buffer));
            getCommand(&player->buf, &player->command);

            if(!player->command.empty()) {
                if (player->command.at(0) == START && player->command.at(player->command.size() - 1) == END) {

                    // w miedzyczasie ktos go zaprasza
                    if (player->command.at(1) == '3') {

                        //wyslij ze niedostepny

                    } else if (player->command.at(1) == '4') {
                        reply = player->command.at(3);
                    }

                    player->command.clear();
                }
            }

        }

        //zgodzil sie na gre
        if (reply == '1') {

            player->playing = true;
            player->opponentFd = opponentFd;

        }
    }
}

void funcIsInvited(Player *player) {
    char buffer[BUFFER];

    size_t firstDelimiter = player->command.find(DELIMITER);
    size_t secondDelimiter = player->command.find(DELIMITER, firstDelimiter + 1);

    string opponentLogin = player->command.substr(firstDelimiter + 1,
                                                  secondDelimiter - firstDelimiter - 1);
    int opponentFd = -1;
    Player *opponent = nullptr;

    for (auto &p : *player->players_ptr) {
        if (p->login == opponentLogin) {
            opponentFd = p->fd;
            opponent = p;
        }
    }

    char reply = player->command.at(secondDelimiter + 1);
    // zgadza sie na gre
    if (reply == '1') {
        string message = "#4;1&";
        strcpy(buffer, message.c_str());
        write(opponentFd, buffer, message.size());

        player->opponentFd = opponentFd;
        player->playing = true;

        // start watku gry (do kolizji)
        //startGame(&player, &opponent);

    } else {
        string message = "#4;0&";
        strcpy(buffer, message.c_str());
        write(opponentFd, buffer, message.size());
        memset(buffer, 0, BUFFER);
    }
}

//funkcja obslugujaca gracza
void *playerPlays (void *t_data)
{
    pthread_detach(pthread_self());
    cout << "player ";


    auto *player = new Player();
    player->fd = ((struct thread_data*)(t_data))->fd;
    player->players_ptr = ((struct thread_data*)(t_data))->players_ptr;
    player->players_ptr->push_back(player);
    player->mutex = ((struct thread_data*)(t_data))->mutex;


    cout << player->fd << '\n';

    pthread_mutex_unlock(player->mutex);

    char buffer[BUFFER];

    // logowanie
    while(!player->logged) {
        memset(buffer, 0, BUFFER);
        read(player->fd, buffer, BUFFER);
        player->buf.append(string(buffer));
        getCommand(&player->buf, &player->command);

        if(!player->command.empty()) {
            if(player->command.at(0) == START && player->command.at(player->command.size()-1) == END) {

                if (player->command.at(1) == '0') {
                    funcLogin(player);
                }

                player->command.clear();
            }
        }
    }


    // zanim zacznie grac
    while(!player->playing) {
        memset(buffer, 0, BUFFER);
        read(player->fd, buffer, BUFFER);
        player->buf.append(string(buffer));
        getCommand(&player->buf, &player->command);

        if(!player->command.empty()) {
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
    while(player->playing) {

        char buffer[BUFFER];

        memset(buffer, 0, BUFFER);
        read(player->fd, buffer, BUFFER);
        player->buf.append(string(buffer));
        getCommand(&player->buf, &player->command);

        if (player->command.at(0) == START && player->command.at(player->command.size() - 1) == END) {

            if(player->command.at(1) == '9') {
                write(player->opponentFd, buffer, BUFFER);
            }

            player->command.clear();

        }

    }



    delete player;
}
