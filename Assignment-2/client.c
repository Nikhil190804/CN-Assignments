#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

typedef struct {
    int thread_num;
} client_args;

void *client_function(void *args) {
    client_args *cargs = (client_args *)args;
    int sock;
    struct sockaddr_in server_addr;
    char buffer[2048];
    memset(buffer, 0, 2048);

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("Could not create socket");
        pthread_exit(NULL);
    }

    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(3000);

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close(sock);
        pthread_exit(NULL);
    }

    printf("Client %d connected to server.\n", cargs->thread_num);

    const char *request = "GET_TOP_PROCESSES";
    if (send(sock, request, strlen(request), 0) < 0) {
        perror("Send failed");
        close(sock);
        pthread_exit(NULL);
    }

    int recv_size = recv(sock, buffer, 2048-1, 0);
    if (recv_size < 0) {
        perror("Receive failed");
        close(sock);
        pthread_exit(NULL);
    }

    buffer[recv_size] = '\0'; 

    printf("Client %d received data:\n%s\n", cargs->thread_num, buffer);

    close(sock);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <number_of_clients>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int n = atoi(argv[1]);
    if (n <= 0) {
        printf("Number of clients must be positive.\n");
        exit(EXIT_FAILURE);
    }

    pthread_t threads[n];
    client_args args[n];

    for (int i = 0; i < n; i++) {
        args[i].thread_num = i + 1;
        if (pthread_create(&threads[i], NULL, client_function, (void *)&args[i]) != 0) {
            perror("Could not create thread");
        }
    }

    for (int i = 0; i < n; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}