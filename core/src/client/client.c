#include <netinet/in.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <core.h>
#include <stdlib.h>

static void auth(int sock, char * name);

void client() {
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char * hello = "mess:Oh shit I'm sorry";
    char * hello2 = "mess:Sorry for what?";
    char * exit = "exit:";
    char buffer[1024] = {0};

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
    char answer[10] = {0};
    char * real_message = message + 5;

    auth(sock, "mock");

    while (strcmp(answer, "closed") != 0) {
        scanf("%[^\n]%*c", real_message);
        if (memcmp(real_message, "close", 5) == 0) {
            send(sock, exit, strlen(exit), 0);
            read(sock, answer, 10);
            return;
        }
        memcpy(message, "mess:", 5);
        send(sock, message, strlen(message), 0);
        read(sock, answer, 10);
        empty(message);
        empty(answer);
    }
}

static void auth(int sock, char * name){
    char answer[10] = {0};
    char message[80] = {0};
    strcat(message, "auth:");
    strcat(message, name);
    send(sock, message, strlen(message), 0);
    read(sock, answer, 10);
}