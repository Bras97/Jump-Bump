//
// Created by monika on 29/12/18.
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

//funkcja obslugujaca gracza
void *playerPlays (void *t_data)
{
    pthread_detach(pthread_self());
    cout << "player ";


    Player *player = new Player();
    player->fd = ((struct thread_data*)(t_data))->fd;
    player->players_ptr = ((struct thread_data*)(t_data))->players_ptr;
    player->players_ptr->push_back(player);


    cout << player->fd << '\n';

    pthread_mutex_unlock(&mutex_server);

    char buffer[BUFFER];

    // logowanie
    while(!player->logged) {
        memset(buffer, 0, BUFFER);
        read(player->fd, buffer, BUFFER);
		cout << string(buffer) << endl;
        player->buf.append(string(buffer));
        getCommand(&player->buf, &player->command);

        if(player->command.at(0) == START && player->command.at(player->command.size()-1) == END) {

            if(player->command.at(1) == '0') {
                string newLogin = player->command.substr(
                        player->command.find(DELIMITER)+1,
                        player->command.find(END) - player->command.find(DELIMITER) - 1);
                cout << newLogin << '\n';

                bool err = false;

                for (auto &p : *player->players_ptr) {
                    if(p->login == newLogin) {
                        err = true;
                    }
                }

                memset(buffer, 0, BUFFER);

                // gdy login juz istnieje
                if(err) {
                    string message = "#0;0&";
                    strcpy(buffer, message.c_str());
                    write(player->fd, buffer, message.size());
                }

                else {
                    player->login.append(newLogin);
                    player->logged = true;
                    string message = "#0;1&";
                    strcpy(buffer, message.c_str());
                    write(player->fd, buffer, message.size());
                }
            }
        }
    }


    // zanim zacznie grac
    while(!player->playing) {
        memset(buffer, 0, BUFFER);
        read(player->fd, buffer, BUFFER);
        player->buf.append(string(buffer));
        getCommand(&player->buf, &player->command);

        if (player->command.at(0) == START && player->command.at(player->command.size() - 1) == END) {
            memset(buffer, 0, BUFFER);

            // wyslij liste graczy
            if (player->command.at(1) == '1') {
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

                // gracz chce zaprosic gracza2 do gry
            else if (player->command.at(1) == '2') {

                int opponentFd = -1;

                string opponentLogin = player->command.substr(player->command.find(DELIMITER) + 1,
                                                              player->command.size() - player->command.find(DELIMITER));

                for (auto &p : *player->players_ptr) {
                    if (p->login == opponentLogin) {
                        if (p->playing == false) {
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
                    int reply = -1;
                    while (reply == -1) {
                        memset(buffer, 0, BUFFER);
                        read(player->fd, buffer, BUFFER);
                        player->buf.append(string(buffer));
                        getCommand(&player->buf, &player->command);

                        if (player->command.at(0) == START && player->command.at(player->command.size() - 1) == END) {

                            if(player->command.at(1) == '3') {

                            }

                            else if (player->command.at(1) != '4') {
                                player->command.clear();
                                break;
                            }

                            else reply = player->command.at(3);
                        }

                    }

                    memset(buffer, 0, BUFFER);
                    //zgodzil sie na gre
                    if (reply == 1) {
                        string message = "#4;1&";
                        strcpy(buffer, message.c_str());
                        write(opponentFd, buffer, message.size());

                        player->playing = true;
                        player->opponentFd = opponentFd;

                    } else {
                        string message = "#4;0&";
                        strcpy(buffer, message.c_str());
                        write(opponentFd, buffer, message.size());
                    }
                }
            }

                // gracz jest zapraszany do gry
            else if (player->command.at(1) == '3') {

                int firstDelimiter = player->command.find(DELIMITER);
                int secondDelimiter = player->command.find(DELIMITER, firstDelimiter + 1);

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
                    string message = "#3;1&";
                    strcpy(buffer, message.c_str());
                    write(opponentFd, buffer, message.size());

                    player->opponentFd = opponentFd;
                    player->playing = true;

                    // start watku gry (do kolizji)
                    //startGame(&player, &opponent);

                } else {
                    string message = "#3;0&";
                    strcpy(buffer, message.c_str());
                    write(player->fd, buffer, message.size());
                    memset(buffer, 0, BUFFER);
                }


            }

            player->command.clear();
        }
    }

    // przesylanie pozycji
    while(player->playing) {

        memset(buffer, 0, BUFFER);
        read(player->fd, buffer, BUFFER);
        player->buf.append(string(buffer));
        getCommand(&player->buf, &player->command);

        if (player->command.at(0) == START && player->command.at(player->command.size() - 1) == END) {
            memset(buffer, 0, BUFFER);

            if(player->command.at(1) == '9') {
                write(player->opponentFd, buffer, BUFFER);
            }

        }

    }



    delete player;
}


int main(int argc, char* argv[])
{
    int server_fd;

    auto *players = new list<Player*>;
    auto *games = new list<Game*>;

    int bind_result;
    int listen_result;
    char reuse_addr_val = 1;
    struct sockaddr_in server_address;


    //inicjalizacja gniazda serwera

    memset(&server_address, 0, sizeof(struct sockaddr));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(SERVER_PORT);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
    {
        fprintf(stderr, "%s: Błąd przy próbie utworzenia gniazda..\n", argv[0]);
        exit(1);
    }
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (char*)&reuse_addr_val, sizeof(reuse_addr_val));

    bind_result = bind(server_fd, (struct sockaddr*)&server_address, sizeof(struct sockaddr));
    if (bind_result < 0)
    {
        fprintf(stderr, "%s: Błąd przy próbie dowiązania adresu IP i numeru portu do gniazda.\n", argv[0]);
        exit(1);
    }

    listen_result = listen(server_fd, QUEUE_SIZE);
    if (listen_result < 0) {
        fprintf(stderr, "%s: Błąd przy próbie ustawienia wielkości kolejki.\n", argv[0]);
        exit(1);
    }



    while(1)
    {

        int player_fd = accept(server_fd, NULL, NULL);
        printf("%d\n", player_fd);
        if (player_fd < 0)
        {
            fprintf(stderr, "%s: Błąd przy próbie utworzenia gniazda dla połączenia.\n", argv[0]);
            exit(1);
        }

        pthread_mutex_lock(&mutex_server);

        struct thread_data * t_data = new thread_data;
        t_data->fd = player_fd;
        t_data->players_ptr = players;
        pthread_t thread1;

        int create_result = pthread_create(&thread1, NULL, playerPlays, (void *)t_data);

        if (create_result){
            printf("Błąd przy próbie utworzenia wątku, kod błędu: %d\n", create_result);
            exit(-1);
        }

        pthread_mutex_lock(&mutex_server);
        pthread_mutex_unlock(&mutex_server);

        delete t_data;

    }


}