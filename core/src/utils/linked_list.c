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