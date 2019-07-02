
#include <iostream>
#include <string>
#include <unistd.h>

#ifndef GUI_H
#include <gui.hpp>
#endif

using namespace std;

void start() {

    initscr();
    printw("Hello World!\n");
    refresh();

    int ch = getch();
    if (ch == KEY_F(1)) {
        printw("F1 key pressed");
    } else {
        printw("The pressed key is ");
        attron(A_BOLD);
        printw("%c", ch);
        attroff(A_BOLD);
    }
    refresh();
    getch();

    endwin();
}

