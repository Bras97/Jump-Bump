//
// Created by monika on 01/12/18.
//
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>


#define SERVER_PORT 1234
#define QUEUE_SIZE 5

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


//struktura zawierająca dane, które zostaną przekazane do wątku
struct thread_data_t
{
//TODO
    char buf[100];
    int fd;
    int fd_player2;
};

//funkcja opisującą zachowanie wątku - musi przyjmować argument typu (void *) i zwracać (void *)
void *ThreadBehavior(void *t_data)
{

    pthread_detach(pthread_self());
    struct thread_data_t th_data = *(struct thread_data_t*)t_data;
    pthread_mutex_unlock(&mutex);
    //printf("%d, %d\n", (th_data).fd, (th_data).fd_player2);
    //dostęp do pól struktury: (*th_data).pole
    //TODO (przy zadaniu 1) klawiatura -> wysyłanie albo odbieranie -> wyświetlanie


    while(1) {
        read((th_data).fd, (th_data).buf, 100);
        printf("%d, %s\n", (th_data).fd, (th_data).buf);
        write((th_data).fd_player2, (th_data).buf, 100);
    }

    pthread_exit(NULL);
}

//funkcja obsługująca połączenie z nowym klientem
void handleConnection(int connection_socket_descriptor, int player2_socket_descriptor) {
    //printf("%d, %d\n", connection_socket_descriptor, player2_socket_descriptor);
    //wynik funkcji tworzącej wątek
    int create_result = 0;

    //uchwyt na wątek
    pthread_t thread1;

    //dane, które zostaną przekazane do wątku
    //TODO dynamiczne utworzenie instancji struktury thread_data_t o nazwie t_data (+ w odpowiednim miejscu zwolnienie pamięci)
    //TODO wypełnienie pól struktury

    pthread_mutex_lock(&mutex);
    struct thread_data_t t_data;
    t_data.fd = connection_socket_descriptor;
    t_data.fd_player2 = player2_socket_descriptor;

    create_result = pthread_create(&thread1, NULL, ThreadBehavior, (void *)&t_data);


    if (create_result){
        printf("Błąd przy próbie utworzenia wątku, kod błędu: %d\n", create_result);
        exit(-1);
    }


}

int main(int argc, char* argv[])
{
    int server_socket_descriptor;
    int player1_socket_descriptor, player2_socket_descriptor;
    int bind_result;
    int listen_result;
    char reuse_addr_val = 1;
    struct sockaddr_in server_address;


    //inicjalizacja gniazda serwera

    memset(&server_address, 0, sizeof(struct sockaddr));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(SERVER_PORT);

    server_socket_descriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket_descriptor < 0)
    {
        fprintf(stderr, "%s: Błąd przy próbie utworzenia gniazda..\n", argv[0]);
        exit(1);
    }
    setsockopt(server_socket_descriptor, SOL_SOCKET, SO_REUSEADDR, (char*)&reuse_addr_val, sizeof(reuse_addr_val));

    bind_result = bind(server_socket_descriptor, (struct sockaddr*)&server_address, sizeof(struct sockaddr));
    if (bind_result < 0)
    {
        fprintf(stderr, "%s: Błąd przy próbie dowiązania adresu IP i numeru portu do gniazda.\n", argv[0]);
        exit(1);
    }

    listen_result = listen(server_socket_descriptor, QUEUE_SIZE);
    if (listen_result < 0) {
        fprintf(stderr, "%s: Błąd przy próbie ustawienia wielkości kolejki.\n", argv[0]);
        exit(1);
    }



    while(1)
    {

        player1_socket_descriptor = accept(server_socket_descriptor, NULL, NULL);
        printf("%d\n", player1_socket_descriptor);
        if (player1_socket_descriptor < 0)
        {
            fprintf(stderr, "%s: Błąd przy próbie utworzenia gniazda dla połączenia.\n", argv[0]);
            exit(1);
        }

        player2_socket_descriptor = accept(server_socket_descriptor, NULL, NULL);
        printf("%d\n", player2_socket_descriptor);
        handleConnection(player1_socket_descriptor, player2_socket_descriptor);
        handleConnection(player2_socket_descriptor, player1_socket_descriptor);

    }

    close(server_socket_descriptor);
    return(0);
}

