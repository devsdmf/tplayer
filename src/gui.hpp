
#ifndef GUI_H
#define GUI_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_NCURSES_H
#include <ncurses.h>
#elif defined HAVE_CURSES_H
#include <curses.h>
#elif defined HAVE_NCURSES_CURSES_H
#include <ncurses/curses.h>
#endif

void start();

#endif

