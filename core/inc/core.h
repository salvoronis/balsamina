#ifndef BALSAMINA_CORE_H
#define BALSAMINA_CORE_H
#define empty(array) memset(array, 0, sizeof (array))
void server(int port);
int client(int port);
void up();
void down();
void auth(char * name);
void send_message(char * message);
#endif //BALSAMINA_CORE_H
