//
// Created by monika on 02/01/19.
//

#include <iostream>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include "serverGame.h"

using namespace std;

void *playGame(void *data) {

    auto *game = new Game(((struct thread_data_game*)(data))->player);
    pthread_detach(pthread_self());
    pthread_mutex_unlock(((struct thread_data_game*)(data))->gameMutex);

    cout << "starting game between " << game->player[0]->login << " and " << game->player[1]->login << '\n';
    cout << game->player[0]->playing << " " << game->player[1]->playing << endl;

    char buffer[BUFFER];
    while(game->player[0]->playing && game->player[1]->playing) {
        memset(buffer, 0, BUFFER);

        pthread_mutex_lock(&game->player[0]->playerMutex);
        pthread_mutex_lock(&game->player[1]->playerMutex);
        string s;

        if ((game->positions[0][0] != game->player[0]->position[0]) || (game->positions[0][1] != game->player[0]->position[1])) {
            s = "#9;" + to_string(game->player[0]->position[0]) + ";" + to_string(game->player[0]->position[1]) + END;
            strcpy(buffer, s.c_str());
            write(game->player[1]->fd, buffer, s.size());
            game->positions[0][0] = game->player[0]->position[0];
            game->positions[0][1] = game->player[0]->position[1];
        }

        if ((game->positions[1][0] != game->player[1]->position[0]) || (game->positions[1][1] != game->player[1]->position[1])) {
            s.clear();
            memset(buffer, 0, BUFFER);
            s = "#9;" + to_string(game->player[1]->position[0]) + ";" + to_string(game->player[1]->position[1]) + END;
            strcpy(buffer, s.c_str());
            write(game->player[0]->fd, buffer, s.size());
            game->positions[1][0] = game->player[1]->position[0];
            game->positions[1][1] = game->player[1]->position[1];
        }


        pthread_mutex_unlock(&game->player[0]->playerMutex);
        pthread_mutex_unlock(&game->player[1]->playerMutex);

    }
}
