#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <stdlib.h>
#include <fcntl.h>
#include <ctype.h>
#include <dirent.h>

struct process_data
{
    char *Name_of_process;
    int process_id;
    unsigned long user_time;
    unsigned long kernel_time;
    unsigned long total_time;
};

char *give_ip(struct in_addr client)
{
    return inet_ntoa(client);
}

int give_port(struct sockaddr_in client)
{
    return ntohs(client.sin_port);
}

int is_pid_dir(char *name)
{
    // Check if all characters in the name are digits
    for (int i = 0; name[i] != '\0'; i++)
    {
        if (!isdigit(name[i]))
        {
            return 0;
        }
    }
    return 1;
}

char *get_process_info()
{
    // give memory to store the final 2 process data
    unsigned long first = 0;
    unsigned long second = 0;

    struct process_data *arr = (struct process_data *)malloc(2 * sizeof(struct process_data));
    arr[0].kernel_time = 0;
    arr[0].Name_of_process = NULL;
    arr[0].process_id = 0;
    arr[0].total_time = 0;
    arr[0].user_time = 0;

    arr[1].kernel_time = 0;
    arr[1].Name_of_process = NULL;
    arr[1].process_id = 0;
    arr[1].total_time = 0;
    arr[1].user_time = 0;

    // read the /proc directory
    struct dirent *entry;
    DIR *proc_dir = opendir("/proc");
    if (proc_dir == NULL)
    {
        printf("Failed to open /proc directory\n");
        fflush(stdout);
        return NULL;
    }

    // reading the file names of proc
    while ((entry = readdir(proc_dir)) != NULL)
    {
        int is_a_process_file = is_pid_dir(entry->d_name);
        if (is_a_process_file)
        {

            char name[300];
            unsigned long user, kernel, total;
            int pid;

            char file_name[256];
            sprintf(file_name, "/proc/%s/stat", entry->d_name);
            FILE *new_process_file = fopen(file_name, "r");
            if (new_process_file == NULL)
            {
                printf("Error while opening file: /proc/%s/stat", entry->d_name);
                fflush(stdout);
                continue;
            }

            char file_content[1024];
            char *result = fgets(file_content, sizeof(file_content), new_process_file);
            if (result == NULL)
            {
                printf("Error while reading the file: /proc/%s/stat", entry->d_name);
                fflush(stdout);
                fclose(new_process_file);
                continue;
            }
            fclose(new_process_file);

            // now parse the contents of this file
            sscanf(file_content, "%d (%[^)]) %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %lu %lu",
                   &pid, name, &user, &kernel);

            total = user + kernel;

            if (total >= first)
            {
                // update second maximum to first maximum
                arr[1] = arr[0];

                // update first maximum
                arr[0].kernel_time = kernel;
                arr[0].Name_of_process = strdup(name);
                arr[0].process_id = pid;
                arr[0].user_time = user;
                arr[0].total_time = total;

                // update temp variables
                second = first;
                first = total;
            }
            else if (total >= second)
            {
                arr[1].kernel_time = kernel;
                arr[1].Name_of_process = strdup(name);
                arr[1].process_id = pid;
                arr[1].user_time = user;
                arr[1].total_time = total;

                second = total;
            }
        }
        fflush(stdout);
    }
    closedir(proc_dir);

    // Print the top two processes with maximum total time
    printf("\n\nProcess PID: %d, Name: %s, Total Time: %lu\n", arr[0].process_id, arr[0].Name_of_process, arr[0].total_time);
    printf("Process PID: %d, Name: %s, Total Time: %lu\n", arr[1].process_id, arr[1].Name_of_process, arr[1].total_time);
    fflush(stdout);

    // format the output to send to client
    char final_result[2048];
    sprintf(final_result, "\n1st Process:-----\nProcess PID: %d, Name: %s, User Time: %lu clock ticks, Kernel Time: %lu clock ticks, Total Time: %lu clock ticks\n2nd Process:-----\nProcess PID: %d, Name: %s, User Time: %lu clock ticks, Kernel Time: %lu clock ticks, Total Time: %lu clock ticks\n", arr[0].process_id, arr[0].Name_of_process, arr[0].user_time, arr[0].kernel_time, arr[0].total_time, arr[1].process_id, arr[1].Name_of_process, arr[1].user_time, arr[1].kernel_time, arr[1].total_time);

    // free the memory of arr
    free(arr);

    char *ptr_to_result = final_result;
    return ptr_to_result;
}

void *process_client(void *client_data)
{
    int client = (int)client_data;
    char buffer[1024] = {0};
    fflush(stdout);
    read(client, buffer, 1024);
    printf("msg from client: %d is %s\n", client,buffer);
    char response[2048] = {0};
    fflush(stdout);
    if (strcmp(buffer,"GET_TOP_PROCESSES")==0)
    {
        // process the client request now
        char *answer = get_process_info();
        if (answer == NULL)
        {
            printf("\nFailed to read proc directory, Server side error!");
            fflush(stdout);
            strcpy(response, "\nFailed to read proc directory, Server side error!");
        }
        else
        {
            strcpy(response, answer);
        }

        // send the data fetched/errors
        send(client, response, sizeof(response), 0);

        // close the connection
        close(client);
    }
    else
    {
        printf("\nRequest not valid!!!\nClosing the connection");
        char *answer = "\nRequest not valid!!!\n";
        send(client, answer, strlen(answer), 0);
        fflush(stdout);
        close(client);
    }
}

int main()
{
    int server = -1;
    struct sockaddr_in server_address;
    int addresslen = sizeof(server_address);

    // open a server socket now
    server = socket(AF_INET, SOCK_STREAM, 0);
    if (server < 0)
    {
        printf("Error has occured while creating server socket!!!\n");
        close(server);
        exit(1);
    }
    else
    {
        printf("Server Socket Initialized.....\n");
        fflush(stdout);
    }

    // fill the address struct with desired values
    int PORT = 3000;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);

    // bind the server now to the desired port
    int bind_result = bind(server, (struct sockaddr *)&server_address, addresslen);
    if (bind_result < 0)
    {
        printf("Failed to bind the server on port %d\n", PORT);
        exit(1);
    }

    else
    {
        printf("Successfully binded the server on port %d\n", PORT);
        fflush(stdout);
    }

    // listen for the incoming requests
    int pending_requests = 100;
    int listen_result = listen(server, pending_requests);
    if (listen_result < 0)
    {
        printf("Server Failed to listen requests!!!\n");
        fflush(stdout);
        exit(1);
    }
    else
    {   
        char *server_ip = give_ip(server_address.sin_addr);
        printf("Server now listening on port %d with IP as: %s\n", PORT,server_ip);
        fflush(stdout);
    }

    // start accepting new requests in infinte loop
    while (true)
    {
        int new_client = -1;
        struct sockaddr_in new_client_address;
        int server_len = sizeof(new_client_address);
        new_client = accept(server, (struct sockaddr *)&new_client_address, (socklen_t *)&server_len);
        if (new_client < 0)
        {
            printf("Failed to accept the new connection!!!\n");
            close(new_client);
        }
        else
        {
            printf("New client connection accepted...\n");
            char *client_ip = give_ip(new_client_address.sin_addr);
            int client_port = give_port(new_client_address);
            printf("Client IP: %s\n", client_ip);
            printf("Client Port: %d\n", client_port);
            fflush(stdout);
        }

        // now create a thread to process the new client
        pthread_t client_thread;
        int thread = pthread_create(&client_thread, NULL, process_client, (void *)new_client);
        if(thread==0){
            pthread_detach(client_thread);
        }
        else{
            printf("\nThread failed!!");
            fflush(stdout);
        }
        
    }

    return 0;
}