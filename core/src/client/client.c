#include <netinet/in.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <core.h>
#include <stdlib.h>
#include <pthread.h>

static void auth(int sock, char * name);

_Noreturn static void *receive(void * args);
int sock;

void client() {
    sock = 0;
    struct sockaddr_in serv_addr;
    char * exit = "exit:";
    pthread_t thread_id;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Cannot create socket\n");
        return;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8080);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("Invalid address/Address not supported\n");
        return;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("Connection failed\n");
        return;
    }

    char message[80] = {0};
    char * real_message = message + 5;

    auth(sock, "mock");

    pthread_create(&thread_id, NULL, receive, NULL);

    while (strcmp(real_message, "close") != 0) {
        scanf("%[^\n]%*c", real_message);
        if (memcmp(real_message, "close", 5) == 0) {
            send(sock, exit, strlen(exit), 0);
            return;
        }
        memcpy(message, "mess:", 5);
        send(sock, message, strlen(message), 0);
        empty(message);
    }
}

static void auth(int sock, char * name){
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
        if (strcmp(cmd, "noti:") == 0) {
            puts(answer + 5);
        }
        empty(answer);
    }
}