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

    while(!player->logged) {
        memset(buffer, 0, BUFFER);
        read(player->fd, buffer, BUFFER);
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
                    break;
                }
            }
        }
    }



    while(player->logged) {

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