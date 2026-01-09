#include <stdint.h>
#include <stdio.h> 
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define MAP_H		10
#define MAP_W		20

#define AIR		0
#define WALL		1
#define PACMAN		2
#define GHOST		3

#define RESET		"\033[0m"
#define CLEAR		"\033[2J\033[H"

#define C_AIR		"\033[48;2;0;0;0m"
#define C_WALL		"\033[48;2;0;46;255m"
#define C_PACMAN	"\033[38;2;255;255;0m"C_AIR
#define C_GHOST		""C_AIR
#define C_BEAN		""C_AIR

#define CH_AIR		" "
#define CH_WALL		" "
#define CH_PACMAN	"Cc"
#define CH_GHOST	"Mm"
#define CH_BEAN		"*"

#define UP		0
#define RIGHT		1
#define DOWN		2
#define LEFT		3	

#define PX	st.px
#define PY	st.py

#define POS_PACMAN map[PY][PX]

#define RUNNING st.running
#define DEAD	st.dead
#define WIN	st.win

#define INIT_PX 1
#define INIT_PY 1

#define OP	st.op

union cel {
	uint8_t d;

	struct {
		uint8_t type     : 2;
		uint8_t bean     : 1;
		uint8_t reserved : 5;
	};
};

#define BEANLEFT st.bean_cnt
#define G1	 0

#define CHASE	0
#define RUNAWAY 1
#define WALK	2
#define CURMODE st.curmode

int8_t gx[1] = {9};
int8_t gy[1] = {9};

typedef struct {
        int8_t x, y;
} Node;



struct {
	uint8_t running;
	uint8_t bean_cnt;
	uint8_t op;
	int8_t px;
	int8_t py;
	uint8_t curmode;
	uint8_t dead;
	uint8_t win;
} st;

uint8_t ani[4] = {0, 0, 0, 0};

union cel map[MAP_H][MAP_W];

const char *sty[] = {
	C_AIR    ,
	C_WALL   ,
	C_PACMAN ,
	C_GHOST  ,
};

const char *ch[] = {
	CH_AIR,
	CH_WALL,
	CH_PACMAN,
	CH_GHOST
};

void prtch(int t)
{
	if (ch[t][ani[t]] == '\0')
		ani[t] = 0;

	printf("%s%c" RESET, sty[t], ch[t][ani[t]]);
	ani[t]++;
}

void change(void)
{
	printf("\n");
}
static int trace(int8_t sx, int8_t sy, int8_t tx, int8_t ty)
{
        if (sx == tx && sy == ty) return -1;

        int8_t came_from[MAP_H][MAP_W];
        memset(came_from, -1, sizeof(came_from));

        Node queue[MAP_H * MAP_W];
        int head = 0, tail = 0;

        int8_t dx[] = {0, 1, 0, -1};
        int8_t dy[] = {-1, 0, 1, 0};

        queue[tail++] = (Node){sx, sy};
        came_from[sy][sx] = 4;

        bool found = false;
        while (head < tail) {
                Node curr = queue[head++];
                if (curr.x == tx && curr.y == ty) {
                        found = true;
                        break;
                }
                for (int i = 0; i < 4; i++) {
                        int8_t nx = curr.x + dx[i];
                        int8_t ny = curr.y + dy[i];
                        if (nx >= 0 && nx < MAP_W && ny >= 0 && ny < MAP_H) {
                                if (map[ny][nx].type != WALL && came_from[ny][nx] == -1) {
                                        came_from[ny][nx] = i;
                                        queue[tail++] = (Node){nx, ny};
                                }
                        }
                }
        }

        if (found) {
                int8_t last_dir = -1;
                int8_t cx = tx, cy = ty;
                while (cx != sx || cy != sy) {
                        last_dir = came_from[cy][cx];
                        int8_t move_dir = last_dir;
                        cx -= dx[move_dir];
                        cy -= dy[move_dir];
                }
                return last_dir;
        }
        return -1;
}

int findpath(int n)
{
	return trace(gx[n], gy[n], PX, PY);
}

void init(void)
{
	for (int i = 0; i < MAP_H; i++) {
		for (int j = 0; j < MAP_W; j++) {
			map[i][j].d = AIR;
		}
	}
	

	map[INIT_PY][INIT_PX].type = PACMAN;
	map[9][19].type = WALL;
	map[gy[G1]][gx[G1]].type = GHOST;
	map[INIT_PY][INIT_PX + 5].type = WALL;	
	RUNNING = 1;
	PX = INIT_PX;
	PY = INIT_PY;
	OP = RIGHT;
	CURMODE = CHASE;
	BEANLEFT = 1;
	map[INIT_PY][INIT_PX + 4].bean = 1;
	/*
	for (int i = 0; i < MAP_H; i++) {
		for (int j = 0; j < MAP_W; j++) {
			if (map[i][j].type == AIR) {
				map[i][j].bean = 1;
				BEANLEFT++;
			}
		}
	}
	*/
}


#define DEBUG 1
void render(void)
{
	printf(CLEAR);
	for (int i = 0; i < MAP_H; i++) {
		for (int j = 0; j < MAP_W; j++) {
			union cel *t = &map[i][j];
			if (t->type == GHOST && t->bean == 1) {
				prtch(t->type);
			} else if (t->bean == 1) {
				printf(C_BEAN CH_BEAN);
			} else {
				prtch(t->type);
			}
		}
		change();
	}

	if (DEBUG) {
		printf("PX:%d, PY:%d\n", PX, PY);
		printf("BEANLEFT:%d\n", BEANLEFT);
		printf("findpath:%d\n", findpath(G1));
	}
	
	if (WIN == 1)
		printf("you win!\n");
	if (DEAD == 1)
		printf("you died...\n");
}


#define MFINE 0
#define MDEAD 1
int move(void)
{
	int8_t tx = PX;
	int8_t ty = PY;
	if (OP == UP) {
		ty--;
	} else if (OP == RIGHT) {
		tx++;
	} else if (OP == DOWN) {
		ty++;
	} else if (OP == LEFT) {
		tx--;
	}
	if (tx > 0 && tx < MAP_W - 1) {
		if (map[ty][tx].type == WALL)	
			return MFINE;
	}

	POS_PACMAN.type = AIR;	
	PX = tx;
	PY = ty;

	if (PX < 0)
		PX = MAP_W - 1;
	else if (PX > MAP_W - 1)
		PX = 0;

	if (PY < 0)
		PY = MAP_H - 1;
	else if (PY > MAP_H - 1)
		PY = 0;

	if (POS_PACMAN.type == GHOST)
		return MDEAD;
	if (POS_PACMAN.bean == 1) {
		BEANLEFT--;
		POS_PACMAN.bean = 0;
	}	
	POS_PACMAN.type = PACMAN;
	return MFINE;
}

int ghmove(int n, int dir)
{
	int8_t x = gx[n];
	int8_t y = gy[n];

	map[y][x].type = AIR;

	if (dir == UP)
		y--;
	else if (dir == RIGHT)
		x++;
	else if (dir == DOWN)
		y++;
	else if (dir == LEFT)
		x--;

	if (x < 0)
		x = MAP_W - 1;
	else if (x > MAP_W - 1)
		x = 0;

	if (y < 0)
		y = MAP_H - 1;
	else if (y > MAP_H - 1)
		y = 0;
	
	if (map[y][x].type == PACMAN)
		RUNNING = 0;
	
	map[y][x].type = GHOST;
	gx[n] = x;
	gy[n] = y;
}

void run(void)
{
	if (move() == MDEAD) {
		RUNNING = 0;
		DEAD	= 1;
	}

	if (CURMODE == CHASE)	
		ghmove(G1, findpath(G1));


	if (BEANLEFT == 0) {
		RUNNING = 0;
		WIN	= 1;
	}
}

int main(void)
{
	init();
	
	while (RUNNING) {
		run();
		render();
		sleep(1);
	}


	return 0;
}
