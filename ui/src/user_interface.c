#include <esc.h>
#include <ui.h>
#include <string.h>
#include <core.h>
#include <pthread.h>

static void * read_message(void * args);
static void message_up();

int current_line;
char messages[20][80];
pthread_mutex_t lock_screen;

void initialize_ui(){
    current_line = 0;
    if (pthread_mutex_init(&lock_screen, NULL) != 0) {
        puts("problem with mutex");
        return;
    }
    home();
    clear_screen();
    gotoxy(30, 10);
    char username[20];
    printf("username: ");
    scanf("%[^\n]%*c", username);
    auth(username);
    clear_screen();
    pthread_t messages_thread;
    pthread_create(&messages_thread, NULL, read_message, NULL);
    pthread_join(messages_thread, NULL);
}

static void * read_message(void * args){
    char message[80];
    while (strcmp(message, "close") != 0) {
        empty(message);
        gotoxy(1, 21);
        printf("message: ");
        scanf("%[^\n]%*c", message);
        send_message(message);
        gotoxy(9, 21);
        clear_cur_line();
    }
    home();
    clear_screen();
}

void print_message(char * message){
    if (current_line == 20) {
        message_up();
    }
    memcpy(&messages[current_line], message, strlen(message));
    pthread_mutex_lock(&lock_screen);
    gotoxy(1, current_line+1);
    printf("%s", message);
    current_line++;
    pthread_mutex_unlock(&lock_screen);
}

static void message_up(){
    clear_screen();
    for (int i = 1; i < 20; i++){
        gotoxy(1, i);
        memcpy(messages[i-1], messages[i], 80);
        printf("%s",messages[i-1]);
    }
    gotoxy(1, current_line);
    current_line = 19;
}