#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/select.h>
#include <termios.h>
#include <stdlib.h>
#include <time.h>

#define AIR    0
#define HEAD   1
#define BODY   2
#define FRUIT  3
#define POSION 4

#define DEBUG  0

#define C_AIR        "  "
#define C_HEAD       "ö "
#define C_BODY       "• "
#define C_FRUIT      "€ "
#define C_POSION     "× "

#define MAP_H 16
#define MAP_W 16

#define INIT_POS map[MAP_H / 2][MAP_W / 2]

#define UP    0
#define RIGHT 1
#define DOWN  2
#define LEFT  3

#define CLEAR		"\033[2J\033[H"

#define RUNNING 	map[0][0].data
#define OP      	map[0][1].data


#define FPS 		60
#define FRAME_TIME 	1000000/FPS
#define TICK		map[0][2].data

#define TAILPOS		map[0][3].data
#define PREV_X		map[TAILPOS-1][0].data
#define PREV_Y		map[TAILPOS-1][1].data
#define NEXT_X		map[TAILPOS+1][0].data
#define NEXT_Y		map[TAILPOS+1][1].data
#define HEAD_X  	map[1][0].data
#define HEAD_Y  	map[1][1].data

#define TAIL_X		map[TAILPOS][0].data
#define TAIL_Y		map[TAILPOS][1].data

union block {
	uint8_t val;

	struct {
		uint8_t type     : 3;	
		uint8_t data     : 5;
	};
};

union block map[MAP_W][MAP_H];
static struct termios original_termios;

/* Function Definations */
void init(void);
void run(void);
void quit(void);
int  move(void);
void ui(void);
void setrawmode(void);
void restoreter(void);
int kbhit(void);
void debug(void);
void spawn(void);
#include "func.c"

int main(void)
{
	init();
	run();
	restoreter();
	return 0;
}



