#include <netinet/in.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

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

    send(sock, hello, strlen(hello), 0);
    valread = read(sock, buffer, 1024);
    send(sock, hello2, strlen(hello2), 0);
    valread = read(sock, buffer, 1024);
    send(sock, exit, strlen(exit), 0);
    valread = read(sock, buffer, 1024);
    printf("%s\n", buffer);
    return;
}