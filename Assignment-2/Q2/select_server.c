#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <dirent.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <sys/select.h> 

#define MAX_CLIENTS 100
#define BUFFER_SIZE 2048

struct process_data {
    char *process_name;
    int pid;
    unsigned long user_time;
    unsigned long kernel_time;
    unsigned long total_time;
};

char *get_process_info() {
    unsigned long first = 0, second = 0;
    struct process_data top2[2] = { {NULL, 0, 0, 0, 0}, {NULL, 0, 0, 0, 0} };

    DIR *proc_dir = opendir("/proc");
    if (proc_dir == NULL) {
        perror("Failed to open /proc directory");
        return NULL;
    }

    struct dirent *entry;
    while ((entry = readdir(proc_dir)) != NULL) {
        if (isdigit(entry->d_name[0])) { 
            char path[512];  
            snprintf(path, sizeof(path), "/proc/%s/stat", entry->d_name);

            FILE *stat_file = fopen(path, "r");
            if (stat_file == NULL) {
                continue;  
            }

            char name[300];
            unsigned long user_time, kernel_time, total_time;
            int pid;

            char stat_data[BUFFER_SIZE];
            fgets(stat_data, sizeof(stat_data), stat_file);
            fclose(stat_file);

            sscanf(stat_data, "%d (%[^)]) %*c %*d %*d %*d %*d %*u %*u %*u %*u %*u %lu %lu",
                   &pid, name, &user_time, &kernel_time);
            total_time = user_time + kernel_time;

            if (total_time > first) {
                top2[1] = top2[0];  
                top2[0] = (struct process_data){strdup(name), pid, user_time, kernel_time, total_time};
                second = first;
                first = total_time;
            } else if (total_time > second) {
                top2[1] = (struct process_data){strdup(name), pid, user_time, kernel_time, total_time};
                second = total_time;
            }
        }
    }
    closedir(proc_dir);

    char *result = (char *)malloc(BUFFER_SIZE * sizeof(char));
    snprintf(result, BUFFER_SIZE, 
             "1st Process: PID: %d, Name: %s, User Time: %lu, Kernel Time: %lu, Total Time: %lu\n"
             "2nd Process: PID: %d, Name: %s, User Time: %lu, Kernel Time: %lu, Total Time: %lu\n",
             top2[0].pid, top2[0].process_name, top2[0].user_time, top2[0].kernel_time, top2[0].total_time,
             top2[1].pid, top2[1].process_name, top2[1].user_time, top2[1].kernel_time, top2[1].total_time);

    free(top2[0].process_name);
    free(top2[1].process_name);
    
    return result;
}

void handle_client(int client_socket) {
    char buffer[BUFFER_SIZE] = {0};
    read(client_socket, buffer, sizeof(buffer));
    printf("Received message: %s\n", buffer);
    
    char *response = get_process_info();
    if (response == NULL) {
        const char *error_msg = "Failed to read proc directory\n";
        send(client_socket, error_msg, strlen(error_msg), 0);
    } else {
        send(client_socket, response, strlen(response), 0);
        free(response);
    }
    
    close(client_socket);
}

int main() {
    int server_socket;
    struct sockaddr_in server_addr;
    fd_set read_fds, active_fds;
    int max_sd, new_socket;
    int port = 3000;

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, MAX_CLIENTS) < 0) {
        perror("Listen failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", port);

    FD_ZERO(&active_fds);
    FD_SET(server_socket, &active_fds);
    max_sd = server_socket;

    while (1) {
        read_fds = active_fds; 

        if (select(max_sd + 1, &read_fds, NULL, NULL, NULL) < 0) {
            perror("Select error");
            break;
        }

        if (FD_ISSET(server_socket, &read_fds)) {
            socklen_t addr_len = sizeof(server_addr);
            new_socket = accept(server_socket, (struct sockaddr *)&server_addr, &addr_len);
            if (new_socket < 0) {
                perror("Accept error");
                continue;
            }
            printf("New client connected\n");

            FD_SET(new_socket, &active_fds);
            if (new_socket > max_sd) {
                max_sd = new_socket; 
            }
        }

        for (int i = 0; i <= max_sd; i++) {
            if (FD_ISSET(i, &read_fds)) {
                if (i != server_socket) {
                    handle_client(i);
                    FD_CLR(i, &active_fds); 
                }
            }
        }
    }

    close(server_socket);
    return 0;
}