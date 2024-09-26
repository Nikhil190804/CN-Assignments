#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

void client_function(int client_num) {
    int sock;
    struct sockaddr_in server_addr;
    char buffer[2048];
    memset(buffer, 0, 2048);

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("Could not create socket");
        return;
    
    }

    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(3000);

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed\n");
        close(sock);
        return;
    }

    printf("Client %d connected to server.\n",client_num);

    const char *request = "GET_TOP_PROCESSES";
    if (send(sock, request, strlen(request), 0) < 0) {
        perror("Send failed\n");
        close(sock);
        return;
    }

    int recv_size = recv(sock, buffer, 2048-1, 0);
    if (recv_size < 0) {
        perror("Receive failed\n");
        close(sock);
        return;
    }

    buffer[recv_size] = '\0'; 

    printf("Client %d received data:\n%s\n", client_num, buffer);

    close(sock);
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

    for (int i = 0; i < n; i++) {
        client_function(i+1);
    }

    return 0;
}