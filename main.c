#include <ncurses.h>

int main(void) {
    initscr();
    raw();
    noecho();
    keypad(stdscr, TRUE);

    printw("Hello! Press any key to exit");

    refresh();

    getch();

    endwin();
    return 0;
}
