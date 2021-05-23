#ifndef BALSAMINA_LINKED_LIST_H
#define BALSAMINA_LINKED_LIST_H
struct LinkedList {
    int connection_fd;
    char * name;
    struct LinkedList * next;
};
void list_add(struct LinkedList ** head, int fd, char * name);
char * list_get_by_conn(struct LinkedList *head, int conn);
#endif //BALSAMINA_LINKED_LIST_H