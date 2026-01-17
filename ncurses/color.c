#include <ncurses.h>
#include <stdlib.h>
#include <stdbool.h>

int main(void)
{
        int choice;
        int highlight = 1;

        const int n_choices = 3;
        
        char *choices[] = {
                " 1. Start",
                " 2. Load ",
                " 3. Quit ",
        };
        
        initscr();
        clear();
        noecho();
        cbreak();
        keypad(stdscr, true);

        if (has_colors()) {
                start_color();
                init_pair(1, COLOR_CYAN, COLOR_BLACK);
                init_pair(2, COLOR_YELLOW, COLOR_RED);
                init_pair(3, COLOR_WHITE, COLOR_BLACK);
        }

        while (1) {
                attron(COLOR_PAIR(1) | A_BOLD);
                mvprintw(2, 5, "COLOR");
                attroff(COLOR_PAIR(1) | A_BOLD);

                for (int i = 0; i < n_choices; i++) {
                        if (highlight == i + 1) {
                                attron(COLOR_PAIR(2));
                                mvprintw(i + 5, 8, "%s", choices[i]);
                                attroff(COLOR_PAIR(2));
                        } else {
                                attron(COLOR_PAIR(3));
                                mvprintw(i + 3, 8, "%s", choices[i]);
                                attroff(COLOR_PAIR(3));
                        }
                }

                refresh();

                choice = getch();
                switch (choice) {
                        case KEY_UP:
                                if (highlight == 1)
                                        highlight = n_choices;
                                else
                                        highlight--;
                                break;
                        case KEY_DOWN:
                                if (highlight == n_choices)
                                        highlight = 1;
                                else
                                        highlight++;
                                break;
                        case 10:
                                goto end_loop;
                        default:
                                break;
                }
        }

end_loop:
        refresh();
        getch();
        endwin(); // important

        return 0;
}
