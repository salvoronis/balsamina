#include <netinet/in.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>
#include <core.h>
#include <stdlib.h>
#include <pthread.h>
#include <ui.h>

static void *receive(void * args);
int sock;

int client() {
    sock = 0;
    struct sockaddr_in serv_addr;
    pthread_t thread_id;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Cannot create socket\n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8080);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("Invalid address/Address not supported\n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("Connection failed\n");
        return -1;
    }

    pthread_create(&thread_id, NULL, receive, NULL);

    return sock;
}

void send_message(char * message){
    char * exit = "exit:";
    if (memcmp(message, "close", 5) == 0) {
        send(sock, exit, strlen(exit), 0);
        return;
    }
    char msg[85] = {0};
    strcat(msg, "mess:");
    strcat(msg, message);
    send(sock, msg, strlen(msg), 0);
}

void auth(char * name){
    char message[80] = {0};
    strcat(message, "auth:");
    strcat(message, name);
    send(sock, message, strlen(message), 0);
}

static void *receive(void * args) {
    char * answer = malloc(80 * sizeof(char));
    char * cmd = (char *) calloc(1, 6);
    while (recv(sock, answer, 80, 0) > 0) {
        strncpy(cmd, answer, 5);
        print_message(answer + 5);
        empty(answer);
    }
}