//
// Created by monika on 02/01/19.
//

#include <iostream>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include "serverGame.h"

using namespace std;

int respawn[8][2];

void getPoint(Game *game, int player_number) {
    char buffer[BUFFER];
    memset(buffer, 0, BUFFER);
    string temp;
    temp.clear();

    game->player[abs(player_number-1)]->buf.clear();
    game->player[abs(player_number-1)]->command.clear();
    newPosition(game->player[abs(player_number-1)]);
    game->positions[abs(player_number-1)][0] = game->player[abs(player_number-1)]->position[0];
    game->positions[abs(player_number-1)][1] = game->player[abs(player_number-1)]->position[1];


    // nowy wynik
    game->score[player_number]++;
    for(int i = 0; i < 2; i++) {
        temp.clear();
        temp = "#7;" + to_string(game->score[i]) + ';' + to_string(game->score[abs(i-1)]) + END;
        strcpy(buffer, temp.c_str());
        write(game->player[i]->fd, buffer, temp.size());
    }

//    cout << game->player[0]->login << " " << game->score[0] << " || " << game->score[1] << " " << game->player[1]->login << '\n';


}

void checkCollision(Game *game, int player_number){

    int *oldPlayerPos, *playerPos, *opponentPos;
    playerPos = game->player[player_number]->position;
    opponentPos = game->player[abs(player_number-1)]->position;
    oldPlayerPos = game->positions[player_number];

    // czy spada
    if (playerPos[1] > oldPlayerPos[1]) {
        // czy jest kolizja
        if ((playerPos[0] <= opponentPos[0] + BUNNY_WIDTH) &&
            (playerPos[0] + BUNNY_WIDTH >= opponentPos[0]) &&
            (playerPos[1] <= opponentPos[1] + BUNNY_HEIGHT) &&
            (playerPos[1] + BUNNY_HEIGHT >= opponentPos[1])) {

            if (playerPos[1] <= opponentPos[1] - 15 &&
                playerPos[0] + BUNNY_WIDTH >= opponentPos[0] + 10 &&
                playerPos[0] <= playerPos[0] + BUNNY_WIDTH - 10) {

                getPoint(game, player_number);
            }

        }
    }


}

void newPosition(Player *player) {
    char buffer[BUFFER];
    string s;

    int newPos = rand() % 8;
    player->position[0] = respawn[newPos][0];
    player->position[1] = respawn[newPos][1];
    s = "#8;" + to_string(respawn[newPos][0]) + ';' + to_string(respawn[newPos][1]) + END;
    strcpy(buffer, s.c_str());
    write(player->fd, buffer, s.size());

    memset(buffer, 0, BUFFER);
    s.clear();
    s = "#9;" + to_string(respawn[newPos][0]) + ';' + to_string(respawn[newPos][1]) + END;
    strcpy(buffer, s.c_str());
    write(player->opponent->fd, buffer, s.size());

//    cout << buffer << '\n';

}


void startGame(Game *game) {
    srand(time(NULL));
    respawn[0][0]=0; respawn[0][1]=20;
    respawn[1][0]=530; respawn[1][1]=390;
    respawn[2][0]=650; respawn[2][1]=410;
    respawn[3][0]=690; respawn[3][1]=30;
    respawn[4][0]=250; respawn[4][1]=10;
    respawn[5][0]=310; respawn[5][1]=220;
    respawn[6][0]=100; respawn[6][1]=420;
    respawn[7][0]=0; respawn[7][1]=250;

    newPosition(game->player[0]);
    newPosition(game->player[1]);
}

void *playGame(void *data) {

    auto *game = new Game(((struct thread_data_game*)(data))->player);
    pthread_mutex_t *gameMutex = ((struct thread_data_game*)(data))->gameMutex;
    pthread_detach(pthread_self());
    pthread_mutex_unlock(((struct thread_data_game*)(data))->gameMutex);


    cout << "starting game between " << game->player[0]->login << " and " << game->player[1]->login << '\n';



    char buffer[BUFFER];
    string s;


    pthread_mutex_lock(&game->player[0]->playerMutex);
    pthread_mutex_lock(&game->player[1]->playerMutex);

    pthread_mutex_lock(gameMutex);
    startGame(game);

    pthread_mutex_unlock(&game->player[1]->playerMutex);
    pthread_mutex_unlock(&game->player[0]->playerMutex);

    while (game->player[0] != nullptr && game->player[1] != nullptr && game->player[0]->playing && game->player[1]->playing) {
        memset(buffer, 0, BUFFER);
        s.clear();

        pthread_mutex_lock(&game->player[0]->playerMutex);
        pthread_mutex_lock(&game->player[1]->playerMutex);


        //czy byl wykonany ruch przez gracza1
        if ((game->positions[0][0] != game->player[0]->position[0]) ||
            (game->positions[0][1] != game->player[0]->position[1])) {

            checkCollision(game, 0);

            s = "#9;" + to_string(game->player[0]->position[0]) + ";" + to_string(game->player[0]->position[1]) + END;
            strcpy(buffer, s.c_str());
            write(game->player[1]->fd, buffer, s.size());
            game->positions[0][0] = game->player[0]->position[0];
            game->positions[0][1] = game->player[0]->position[1];

        }

        //czy byl wykonany ruch przez gracza2
        if ((game->positions[1][0] != game->player[1]->position[0]) ||
            (game->positions[1][1] != game->player[1]->position[1])){

            checkCollision(game, 1);

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


    pthread_mutex_t *serverMutex = game->player[0]->serverMutex;

    cout << "Game over \n";
    pthread_mutex_lock(serverMutex);
    pthread_mutex_lock(&game->player[0]->playerMutex);
    pthread_mutex_lock(&game->player[1]->playerMutex);
    delete game;
    pthread_mutex_unlock(serverMutex);
}
