#include <ncurses.h>
#include <stdlib.h>
#include <stdbool.h>

void initcolor(void)
{
        start_color();
        init_pair(1, COLOR_WHITE, COLOR_BLACK);
        init_pair(2, COLOR_BLACK, COLOR_WHITE);
}

struct choice {
        char *n;
        bool active;
};

#define POS_TITLE  2,5
#define POS_BASE_X 2
#define POS_BASE_Y 5
int main(void)
{
        int n = 3;
        int idx = 0;
        struct choice c[] = {
                {.n = "option 1", .active = true},
                {.n = "option 2", .active = false},
                {.n = "option 3", .active = false}
        };

        int input;
 
        // Init ncurses
        initscr();
        clear();
        noecho();
        cbreak();
        keypad(stdscr, true);
        curs_set(0);

        if (has_colors())
                initcolor();

        while (1) {
                attron(COLOR_PAIR(1));
                mvprintw(POS_TITLE, "menu list, idx = %d", idx);
                attroff(COLOR_PAIR(1));
                
                for (int i = 0; i < 3; i++) {
                        if (c[i].active) {
                                attron(COLOR_PAIR(2) | A_BOLD);
                                mvprintw(POS_BASE_Y + i, POS_BASE_X, c[i].n);
                                attroff(COLOR_PAIR(2) | A_BOLD);
                        } else {
                                attron(COLOR_PAIR(1));
                                mvprintw(POS_BASE_Y + i, POS_BASE_X, c[i].n);
                                attroff(COLOR_PAIR(1));
                        }
                }
                refresh();

                input = getch();
                int tmp = idx;
                switch (input) {
                        case KEY_UP: {
                                idx--;
                                if (idx < 0)
                                        idx = 0;
                                break;
                        };
                        case KEY_DOWN: {
                                idx++;
                                if (idx > 2)
                                        idx = 2;
                                break;
                        }
                        case 10:
                                goto end;
                        default:
                                break;
                }

                if (tmp != idx) {
                        c[idx].active = true;
                        c[tmp].active = false;
                }
        }
end: 
        endwin();
        return 0;
}
