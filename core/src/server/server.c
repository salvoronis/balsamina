#include <core.h>
#include <stdio.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>

#define NUM_CLIENTS 10

struct sockaddr_in address;
int addrlen;

void *clientHandler(void *vargp){
    int * new_client = (int*)vargp;
    char buffer[1024];
    char * hello = "OK";
    while (recv(*new_client, buffer, sizeof(buffer), 0) > 0) {
        char * cmd = (char *) calloc(1, 6);
        strncpy(cmd, buffer, 5);
        if (strcmp(cmd, "exit:") == 0) {
            puts("close");
            send(*new_client, "closed", strlen("closed"), 0);
            free(cmd);
            return NULL;
        } else if (strcmp(cmd, "mess:") == 0) {
            printf("%d: %s\n", *new_client, buffer);
            send(*new_client, hello, strlen(hello), 0);
        }
        free(cmd);
    }
}

int configure_server(){
    int server_fd;
    addrlen = sizeof(address);
    int opt = 1;

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("init socket failed");
        exit(EXIT_FAILURE);
    }

    if ((setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))) {
        perror("failed set socket options");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind error");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, NUM_CLIENTS) < 0) {
        perror("listen error");
        exit(EXIT_FAILURE);
    }
    return server_fd;
}

void server() {
    int new_socket;
    pthread_t thread_id;
    int server_fd = configure_server();
    while((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))) {
        //if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        //    perror("cannot accept");
        //    exit(EXIT_FAILURE);
        //}
        pthread_create(&thread_id, NULL, clientHandler, (void *) &new_socket);
        //pthread_join(thread_id, NULL);
    }
}