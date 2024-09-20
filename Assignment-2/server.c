#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <stdlib.h>
#include <pthread.h>
#include <fcntl.h>
#include <ctype.h>
#include <dirent.h>

char * give_ip(struct in_addr client){
    return inet_ntoa(client);
}

int give_port(struct sockaddr_in client){
    return ntohs(client.sin_port);
}

int is_pid_dir(char *name) {
    // Check if all characters in the name are digits
    for (int i = 0; name[i] != '\0'; i++) {
        if(!isdigit(name[i])){
            return 0;
        }
    }
    return 1; 
}


void get_process_info(){
    struct dirent *entry;
    DIR *proc_dir = opendir("/proc");
    if(proc_dir==NULL){
        printf("Failed to open /proc directory\n");
        fflush(stdout);
    }
    else{
        // start reading the proc directory
        while((entry = readdir(proc_dir))!=NULL){
            int is_a_process_file = is_pid_dir(entry->d_name);
            if(is_a_process_file){
                printf("holas: %s\n",entry->d_name);
            }
            fflush(stdout);
        }
    }
}

void* process_client(void* client_data){
    int client = (int)client_data;
    printf("done %d\n",client);
    char buffer[1024] = {0};
    fflush(stdout);
    read(client, buffer, 1024);
    printf("msg : %s",buffer);
    fflush(stdout);
    if(true){
        //process the client request now
        get_process_info();
    }
    else{
        printf("Request not valid!!!\nClosing the connection");
        fflush(stdout);
        exit(1);
    }
}


int main(){
    int server = -1;
    struct sockaddr_in server_address;
    int addresslen = sizeof(server_address);

    // open a server socket now 
    server=socket(AF_INET,SOCK_STREAM,0);
    if(server <0){
        printf("Error has occured while creating server socket!!!\n");
        close(server);
        exit(1);
    }
    else{
        printf("Server Socket Initialized.....\n");
        fflush(stdout);
    }

    // fill the address struct with desired values
    int PORT = 3000;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);

    //bind the server now to the desired port
    int bind_result = bind(server,(struct sockaddr *)&server_address,addresslen);
    if(bind_result < 0){
        printf("Failed to bind the server on port %d\n",PORT);
        exit(1);
    }
        
    else{
        printf("Successfully binded the server on port %d\n",PORT);
        fflush(stdout);
    }

    // listen for the incoming requests
    int pending_requests = 100;
    int listen_result = listen(server,pending_requests);
    if(listen_result < 0){
        printf("Server Failed to listen requests!!!\n");
    }
    else{
        printf("Server now listening on port %d \n",PORT);
        fflush(stdout);
    }

    // start accepting new requests in infinte loop
    while(true){
        int new_client = -1;
        struct sockaddr_in new_client_address;
        int server_len = sizeof(new_client_address);
        new_client = accept(server,(struct sockaddr *)&new_client_address,(socklen_t*)&server_len);
        if(new_client < 0){
            printf("Failed to accept the new connection!!!\n");
            close(new_client);
        }
        else{
            printf("New client connection accepted...\n");
            char * client_ip = give_ip(new_client_address.sin_addr);
            int client_port = give_port(new_client_address);
            printf("Client IP: %s\n",client_ip);
            printf("Client Port: %d\n",client_port);
            fflush(stdout);
        }
        
        // now create a thread to process the new client
        pthread_t client_thread;
        int thread = pthread_create(&client_thread,NULL,process_client,(void*)new_client);

    }
    

    return 0;
}
