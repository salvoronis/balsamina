#include <core.h>
#include <stdio.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <linked_list.h>

#define NUM_CLIENTS 10
#define MESSAGE_BODY 5

void *notify(void *var_group);

struct sockaddr_in address;
int addrlen;
//TODO fucking mutex when using users
struct LinkedList * users = NULL;

struct message_wrapper {
    char * data;
};

struct message_wrapper * create_mes_wrap(char * data, int len){
    struct message_wrapper * wrapper = malloc(sizeof(struct message_wrapper));
    wrapper->data = malloc(len);
    memcpy(wrapper->data, data, len);
    return wrapper;
}

void *clientHandler(void *vargp){
    int * new_client = (int*)vargp;
    char buffer[80];
    char ans_buf[100];
    char * real_message = buffer + MESSAGE_BODY;
    char * cmd = (char *) calloc(1, 6);
    char * ok = "ctrl:OK";
    pthread_t thread;
    while (recv(*new_client, buffer, sizeof(buffer), 0) > 0) {
        strncpy(cmd, buffer, 5);
        //disconnect user
        if (strcmp(cmd, "exit:") == 0) {
            puts("close");
            list_delete_node(&users, *new_client);
            sprintf(ans_buf, "noti:%s %s", list_get_by_conn(users, *new_client), "disconnected");
            //notify all users about %username% disconnection
            pthread_create(&thread, NULL, notify, create_mes_wrap(ans_buf, strlen(ans_buf)+1));
            pthread_join(thread, NULL);
            free(cmd);
            return 0;
        }
        //message from user
        else if (strcmp(cmd, "mess:") == 0) {
            sprintf(ans_buf, "noti:%s: %s", list_get_by_conn(users, *new_client), real_message);
            puts(ans_buf);
            //notify other users about message
            pthread_create(&thread, NULL, notify, create_mes_wrap(ans_buf, strlen(ans_buf)+1));
            pthread_join(thread, NULL);
            send(*new_client, ok, strlen(ok), 0);
        }
        //connect user to the chat
        else if (strcmp(cmd, "auth:") == 0) {
            list_add(&users, *new_client, real_message);
            printf("new user -> %s\n", real_message);
            send(*new_client, ok, strlen(ok), 0);
        }
        empty(buffer);
        empty(ans_buf);
    }
    return (void*)-1;
}

void *notify(void *var_group) {
    struct LinkedList * tmp = users;
    struct message_wrapper * wrapper = (struct message_wrapper *) var_group;
    //TODO fix broken pipe
    while (tmp != NULL) {
        send(tmp->connection_fd, wrapper->data, strlen(wrapper->data)+1, 0);
        tmp = tmp->next;
    }
    return NULL;
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
        pthread_create(&thread_id, NULL, clientHandler, (void *) &new_socket);
        //pthread_join(thread_id, NULL);
    }
}