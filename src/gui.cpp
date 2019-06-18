
#include <iostream>
#include <string>
#include <unistd.h>

#ifndef GUI_H
#include <gui.hpp>
#endif

using namespace std;

void start() {
    //cout << "Hello!";

    initscr();
    printw("Hello World!\n");
    refresh();
    getch();
    endwin();
}

