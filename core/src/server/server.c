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
void send_history(void *var_group);

struct sockaddr_in address;
int addrlen;
//TODO fucking mutex when using users
struct LinkedList * users = NULL;
char ** history_buf;
unsigned int history_n = 0;
pthread_mutex_t lock_history;

struct message_wrapper {
    char * data;
};

struct history_wrapper {
    unsigned int    cur_pointer;
    int             user;
};

struct message_wrapper * create_mes_wrap(char * data, int len){
    struct message_wrapper * wrapper = malloc(sizeof(struct message_wrapper));
    wrapper->data = malloc(len);
    memcpy(wrapper->data, data, len);
    return wrapper;
}

void *clientHandler(void *vargp){
    int new_client;
    memcpy(&new_client, vargp, sizeof(int));
    char buffer[80];
    char ans_buf[100];
    char * real_message = buffer + MESSAGE_BODY;
    char * cmd = (char *) calloc(1, 6);

    unsigned int user_pointer = 0;

    pthread_t thread;
    while (recv(new_client, buffer, sizeof(buffer), 0) > 0) {
        strncpy(cmd, buffer, 5);
        //disconnect user
        if (strcmp(cmd, "exit:") == 0) {
            puts("close");
            list_delete_node(&users, new_client);
            sprintf(ans_buf, "noti:%s %s", list_get_by_conn(users, new_client), "disconnected");
            return 0;
        }
        //message from user
        else if (strcmp(cmd, "mess:") == 0) {
            if (strstr(real_message, "@")) {
                char * tmp = calloc(1, strlen(real_message) + 1);
                memcpy(tmp, real_message, strlen(real_message));
                char * to = strtok(tmp, " ");
                int to_client = connection_by_name(users, to + 1);
                sprintf(ans_buf, "noti:%s: %s", list_get_by_conn(users, new_client), real_message);
                send(new_client, ans_buf, strlen(ans_buf)+1, 0);
                send(to_client, ans_buf, strlen(ans_buf)+1, 0);
                empty(ans_buf);
                continue;
            }
            sprintf(ans_buf, "noti:%s: %s", list_get_by_conn(users, new_client), real_message);
            puts(ans_buf);
        }
        //connect user to the chat
        else if (strcmp(cmd, "auth:") == 0) {
            list_add(&users, new_client, real_message);
            sprintf(ans_buf, "auth:%s %s", list_get_by_conn(users, new_client), "joined chat");
            puts(ans_buf);
            struct history_wrapper wrapper = {history_n, new_client};
            send_history((void *)&wrapper);
        }
        else if (strcmp(cmd, "upto:") == 0) {
            if (user_pointer == 0) {
                user_pointer = history_n;
            }
            if (user_pointer <= 20) {
                continue;
            }
            user_pointer--;
            sprintf(ans_buf, "upto:%s", (history_buf[user_pointer - 20]) + 5);
            send(new_client, ans_buf, strlen(ans_buf)+1, 0);
            continue;
        }
        else if (strcmp(cmd, "down:") == 0) {
            if (user_pointer == 0) {
                user_pointer = history_n;
            }
            if (user_pointer >= history_n) {
                continue;
            }
            sprintf(ans_buf, "down:%s", (history_buf[user_pointer]) + 5);
            user_pointer++;
            send(new_client, ans_buf, strlen(ans_buf)+1, 0);
            continue;
        }
        pthread_mutex_lock(&lock_history);
        history_buf = realloc(history_buf, (history_n + 1) * sizeof(char*));
        history_buf[history_n] = calloc(1, strlen(ans_buf) + 1);
        strcpy(history_buf[history_n], ans_buf);
        history_n++;
        pthread_mutex_unlock(&lock_history);
        pthread_create(&thread, NULL, notify, create_mes_wrap(ans_buf, strlen(ans_buf)+1));
        pthread_join(thread, NULL);
        empty(buffer);
        empty(ans_buf);
    }
    return (void*)-1;
}

void send_history(void *var_group){
    struct history_wrapper * history = (struct history_wrapper*) var_group;
    unsigned int from = 0;
    if (history->cur_pointer == 0) {
        return;
    }
    if (history->cur_pointer > 20) {
        from = history->cur_pointer - ((history->cur_pointer/20) * 20);
    }
    pthread_mutex_lock(&lock_history);
    for (unsigned int i = from; i < history->cur_pointer; i++){
        send(history->user, history_buf[i], strlen(history_buf[i])+1, 0);
    }
    pthread_mutex_unlock(&lock_history);
}

void *notify(void *var_group) {
    struct LinkedList * tmp = users;
    struct message_wrapper * wrapper = (struct message_wrapper*) var_group;
    //TODO fix broken pipe
    while (tmp != NULL) {
        send(tmp->connection_fd, wrapper->data, strlen(wrapper->data)+1, 0);
        tmp = tmp->next;
    }
    return NULL;
}

int configure_server(int port){
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
    address.sin_port = htons(port);

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

void server(int port) {
    if (pthread_mutex_init(&lock_history, NULL) != 0) {
        puts("problem with mutex");
        return;
    }
    int new_socket;
    pthread_t thread_id;
    int server_fd = configure_server(port);
    history_buf = malloc(1);
    while((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))) {
        pthread_create(&thread_id, NULL, clientHandler, (void *) &new_socket);
    }
}