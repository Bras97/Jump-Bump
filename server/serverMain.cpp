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
#include "serverPlayer.h"


int main(int argc, char* argv[]) {
    int server_fd;

    auto *players = new std::list<Player*>;
    auto *games = new std::list<Game*>;

    int bind_result;
    int listen_result;
    char reuse_addr_val = 1;
    struct sockaddr_in server_address;

    pthread_mutex_t mutex_server = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t *mutex = &mutex_server;



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


    std::cout << "Server\n";
    while(1)
    {

        int player_fd = accept(server_fd, NULL, NULL);
//        printf("%d\n", player_fd);
        if (player_fd < 0)
        {
            fprintf(stderr, "%s: Błąd przy próbie utworzenia gniazda dla połączenia.\n", argv[0]);
            exit(1);
        }

        pthread_mutex_lock(&mutex_server);

        struct thread_data * t_data = new thread_data;
        t_data->fd = player_fd;
        t_data->players_ptr = players;
        t_data->mutex = mutex;
        pthread_t thread1;

        int create_result = pthread_create(&thread1, NULL, playerPlays, (void *)t_data);

        if (create_result){
            printf("Błąd przy próbie utworzenia wątku, kod błędu: %d\n", create_result);
            exit(-1);
        }

        pthread_mutex_lock(&mutex_server);
        delete t_data;
        pthread_mutex_unlock(&mutex_server);

    }


}