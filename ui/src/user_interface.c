#include <esc.h>
#include <ui.h>
#include <string.h>
#include <core.h>

static void read_message();
static void message_up();

int current_line;
char messages[20][80];

void initialize_ui(){
    current_line = 0;
    home();
    clear_screen();
    gotoxy(30, 10);
    char username[20];
    printf("username: ");
    scanf("%[^\n]%*c", username);
    auth(username);
    clear_screen();
    read_message();
}

static void read_message(){
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
    gotoxy(1, current_line+1);
    if (current_line == 20) {
        message_up();
    }
    memcpy(&messages[current_line], message, strlen(message));
    printf("%s", message);
    current_line++;
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