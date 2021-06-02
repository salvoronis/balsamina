#include <esc.h>
#include <ui.h>
#include <string.h>
#include <core.h>
#include <pthread.h>
#include <sys/select.h>
#include <unistd.h>
#include <termios.h>

static void message_up();

int current_line;
char messages[20][80];
pthread_mutex_t lock_screen;

static struct termios stored_settings;

void set_keypress(void)
{
    struct termios new_settings;

    tcgetattr(0,&stored_settings);

    new_settings = stored_settings;

    new_settings.c_lflag &= (~ICANON & ~ECHO);
    new_settings.c_cc[VTIME] = 0;
    new_settings.c_cc[VMIN] = 1;

    tcsetattr(0,TCSANOW,&new_settings);
    return;
}

void reset_keypress(void)
{
    tcsetattr(0,TCSANOW,&stored_settings);
    return;
}

void initialize_ui(int client_fd){
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

    set_keypress();

    char message[80];
    fd_set s_rd;
    struct timeval time;
    int retval;
    FD_ZERO(&s_rd);
    FD_SET(STDIN_FILENO, &s_rd);
    while (strcmp(message, "close") != 0) {
        time.tv_sec = 0;
        time.tv_usec = 0;
        retval = select(STDIN_FILENO + 1, &s_rd, NULL, NULL, &time);
        if (retval == 0) {
            empty(message);
            gotoxy(1, 21);
            printf("message: ");
            scanf("%[^\n]%*c", message);
            send_message(message);
            gotoxy(9, 21);
            clear_cur_line();
        }
        usleep(100000);
    }
    reset_keypress();
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