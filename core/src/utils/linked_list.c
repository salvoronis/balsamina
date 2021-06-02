#include <linked_list.h>
#include <malloc.h>
#include <string.h>

void list_add(struct LinkedList ** head, int fd, char * name){
    struct LinkedList * tmp = (struct LinkedList*) malloc(sizeof(struct LinkedList));
    tmp->connection_fd = fd;
    tmp->name = (char *) malloc(strlen(name));
    strcpy(tmp->name, name);
    tmp->next = (*head);
    (*head) = tmp;
}

char * list_get_by_conn(struct LinkedList *head, int conn){
    while (head != NULL) {
        if(head->connection_fd == conn){
            return head->name;
        }
        head = head->next;
    }
    return NULL;
}

int connection_by_name(struct LinkedList *head, char * name){
    while (head != NULL) {
        if(strcmp(head->name, name) == 0){
            return head->connection_fd;
        }
        head = head->next;
    }
    return -1;
}

void list_delete_node(struct LinkedList ** head, int conn){
    struct LinkedList * tmp = *head;
    while (tmp->next != NULL){
        if (tmp->next->connection_fd == conn){
            struct LinkedList * next = tmp->next->next;
            free(tmp->next);
            tmp->next = next;
            return;
        }
        tmp = tmp->next;
    }
    return;
}