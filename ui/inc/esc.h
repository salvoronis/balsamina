#ifndef BALSAMINA_ESC_H
#define BALSAMINA_ESC_H
#include <stdio.h>
#define ECS "\033"

//Format text
#define RESET       0
#define BRIGHT      1
#define DIM         2
#define UNDERSCORE  3
#define BLINK       4
#define REVERSE     5
#define HIDDEN      6

//Foreground
#define F_BLACK     30
#define F_RED       31
#define F_GREEN     32
#define F_YELLOW    33
#define F_BLUE      34
#define F_MAGENTA   35
#define F_CYAN      36
#define F_WHITE     37

//Background
#define B_BLACK     40
#define B_RED       41
#define B_GREEN     42
#define B_YELLOW    43
#define B_BLUE      44
#define B_MAGENTA   45
#define B_WHITE     46

//commands
#define home()                      printf(ECS "[H") //to 1,1
#define clear_screen()              printf(ECS "[2J")
#define clear_cur_line()            printf(ECS "[2K")
#define gotoxy(x,y)                 printf(ECS "[%d;%dH",y,x)
#define visible_cursor()            printf(ESC "[?251")
#define reset_color()               printf(ESC "[0m")
#define set_display_atrib(color)    printf(ESC "[")
#endif //BALSAMINA_ESC_H
