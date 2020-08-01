#include <ncurses.h>
#include "nm_wrapper.h"
#include "ca.pem.h"

#define MAX_CONS 32
#define MAX_COLUMNS 80
#define SPEED_OFFSET 3
#define STRENGTH_OFFSET 15
#define ENCODING_OFFSET 25

int cursor_pos, con_loaded;
connection con_buff[MAX_CONS];
bool enabled;

void draw() {
    clear();
    int max_rows, max_columns;
    getmaxyx(stdscr, max_rows, max_columns);
    int width = (MAX_COLUMNS < max_columns) ? MAX_COLUMNS: max_columns;
    int ssid_width = width - 36;
    // create body
    WINDOW *body = newwin(con_loaded+4, width, 0, 0);
    refresh();
    box(body, 0, 0);
    start_color();
    wattron(body, A_BOLD);
    mvwprintw(body, 1, 2, "SSID");
    mvwprintw(body, 1, ssid_width+SPEED_OFFSET, "SPEED");
    mvwprintw(body, 1, ssid_width+STRENGTH_OFFSET, "STRENGTH");
    mvwprintw(body, 1, ssid_width+ENCODING_OFFSET, "ENDCODING");
    wattroff(body, A_BOLD);
    for (int i = 0; i < con_loaded; i++) {
        if (con_buff[i].active) {
            wattron(body, COLOR_PAIR(1));
        } else if (con_buff[i].available && con_buff[i].configured) {
            wattron(body, COLOR_PAIR(4));
        } else if (con_buff[i].available) {
            wattron(body, COLOR_PAIR(3));
        } else if (con_buff[i].configured) {
            wattron(body, COLOR_PAIR(2));
        }
        if (i == cursor_pos) {
            wattron(body, A_REVERSE);
        }
        mvwprintw(body, i+2, 2, con_buff[i].ssid);
        if (i ==cursor_pos) {
            wattroff(body, A_REVERSE);
        }
        if (con_buff[i].rate) {
            mvwprintw(body, i+2, ssid_width+SPEED_OFFSET, "%3d Mbit/s", con_buff[i].rate);
        } else {
            mvwprintw(body, i+2, ssid_width+SPEED_OFFSET, "-");
        }
        if (con_buff[i].signal) {
            mvwprintw(body, i+2, ssid_width+STRENGTH_OFFSET, "%2d%%", con_buff[i].signal);
        } else {
            mvwprintw(body, i+2, ssid_width+STRENGTH_OFFSET, "-");
        }
        mvwprintw(body, i+2, ssid_width+ENCODING_OFFSET, con_buff[i].security);
        if (con_buff[i].active) {
            wattroff(body, COLOR_PAIR(1));
        } else if (con_buff[i].available && con_buff[i].configured) {
            wattroff(body, COLOR_PAIR(4));
        } else if (con_buff[i].available) {
            wattroff(body, COLOR_PAIR(3));
        } else if (con_buff[i].configured) {
            wattroff(body, COLOR_PAIR(2));
        }

    }
    if (!enabled) {
        mvwprintw(body, con_loaded+2, 2, "[E]nable" );
    } if (con_buff[cursor_pos].active) {
        mvwprintw(body, con_loaded+2, 2, "[q]uit [l]isten [d]isconnect [r]emove" );
    } else if (con_buff[cursor_pos].available && con_buff[cursor_pos].configured) {
        mvwprintw(body, con_loaded+2, 2, "[q]uit [l]isten [c]onnect [r]emove" );
    } else if (con_buff[cursor_pos].available) {
        mvwprintw(body, con_loaded+2, 2, "[q]uit [l]isten [a]dd " );
    } else if (con_buff[cursor_pos].configured) {
        mvwprintw(body, con_loaded+2, 2, "[q]uit [l]isten [r]emove " );
    }
    wrefresh(body);

}
void load() {
    con_loaded = nm_load_connections(con_buff, MAX_CONS);
    enabled = nm_enabled();
    draw();
}

void connect() {
    if (con_buff[cursor_pos].active || !con_buff[cursor_pos].configured) {
        return;
    }
    nm_connect(&con_buff[cursor_pos]);
}

void disconnect() {
    if (!con_buff[cursor_pos].active) {
        return;
    }
    nm_disconnect(&con_buff[cursor_pos]);
}

void remove_con() {
    if (!con_buff[cursor_pos].configured) {
        return;
    }
    nm_delete_connection(&con_buff[cursor_pos]);
}

void get_information(char *information, bool encrypt, char * what) {
    int max_rows, max_columns;
    getmaxyx(stdscr, max_rows, max_columns);
    int width = (MAX_COLUMNS < max_columns) ? MAX_COLUMNS: max_columns;
    WINDOW *pwd_window = newwin(3, width, con_loaded+3, 0);
    refresh();
    box(pwd_window, 0, 0);
    mvwprintw(pwd_window,1, 2, what);
    wrefresh(pwd_window);
    char c;
    int counter = 0;
    while ((c = getch()) != 27) { // not ESCAPE
        if (c == 10) { // ENTER
            information[counter] = '\0';
            return;
        } else if (c == 8 || c == 127) { // BACKSPACE
            if (counter > 0) {
               counter--;
               int cursory, cursorx;
               getyx(pwd_window, cursory, cursorx);
               mvwprintw(pwd_window, cursory, cursorx-1, " ");
               wmove(pwd_window, cursory, cursorx-1);
               wrefresh(pwd_window);
           }
        } else {
            information[counter++] = c;
            if (encrypt) {
                wprintw(pwd_window, "*");
            } else {
                wprintw(pwd_window, "%c", c);
            }
            wrefresh(pwd_window);
        }
    }
}

void add_con() {
    // connection already exists
    if (con_buff[cursor_pos].configured) {
        return;
    }

    // get password
    char password[256];
    get_information(password, true, "Password: ");

    // add connection
    nm_add_connection(con_buff[cursor_pos].ssid, password);
}

void add_con_eduroam() {
    // connection already exists
    if (con_buff[cursor_pos].configured) {
        return;
    }

    // write ca.pem
    write_ca_pem();

    // get mail
    char identity[256];
    get_information(identity, false, "Identity: ");

    // get password
    char password[256];
    get_information(password, true, "Password: ");

    // add connection
    nm_add_connection_eduroam(con_buff[cursor_pos].ssid, identity, password);
}

void action_handler() {
    char c;
    while (1) {
        c = getch();
        switch (c) {
            case 'q':
                return;
            case 'j':
                cursor_pos = (cursor_pos + 1) % con_loaded;
                draw();
                break;
            case 'k':
                cursor_pos = (cursor_pos + con_loaded - 1) % con_loaded;
                draw();
                break;
            case 'l':
                load();
                break;
            case 'c':
                connect();
                load();
                break;
            case 'd':
                disconnect();
                load();
                break;
            case 'r':
                remove_con();
                load();
                break;
            case 'a':
                if (!strcmp(con_buff[cursor_pos].ssid, "eduroam")) {
                    add_con_eduroam();
                } else {
                    add_con();
                }
                load();
                break;
            case 'E':
                nm_enable();
                load();
                break;
            case 'D':
                nm_disable();
                break;
            case 'A':
                add_con_eduroam();
                load();
                break;
        }
    }
}

int main(int argc, char *argv[]) {
    // initialize
    initscr();
    noecho();
    raw();
    cursor_pos = 0;
    start_color();
    // define colors
    init_pair(0, COLOR_WHITE, COLOR_BLACK);
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_pair(2, COLOR_YELLOW, COLOR_BLACK);
    init_pair(3, COLOR_BLUE, COLOR_BLACK);
    init_pair(4, COLOR_CYAN, COLOR_BLACK);
    // start programm
    load();
    action_handler();
    endwin();
    return 0;
}
