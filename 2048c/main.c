#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

#define DO_LINE(r, c, expr) for (c = 0; c < 4; c++) expr
#define DO_COL(r, c, expr) for (r = 0; r < 4; r++) expr
#define REV(c) (3 - (c))
#define REV_R(r) (3 - (r))

#define CLEAR	"\033[2J\033[H"

#define GET_LINE(r, ln) do {				\
	for (int _c = 0; _c < 4; _c++)			\
		ln[_c] = get(r, _c);			\
} while (0)

#define SET_LINE(r, ln) do {				\
	for (int _c = 0; _c < 4; _c++)			\
		set(r, _c, ln[_c]);			\
} while (0)

#define GET_LINE_REV(r, ln) do {			\
	for (int _c = 0; _c < 4; _c++)			\
		ln[REV(_c)] = get(r, _c);		\
} while (0)

#define SET_LINE_REV(r, ln) do {			\
	for (int _c = 0; _c < 4; _c++)			\
		set(r, _c, ln[REV(_c)]);		\
} while (0)

#define GET_COL(c, ln) do {				\
	for (int _r = 0; _r < 4; _r++)			\
		ln[_r] = get(_r, c);			\
} while (0)

#define SET_COL(c, ln) do {				\
	for (int _r = 0; _r < 4; _r++)			\
		set(_r, c, ln[_r]);			\
} while (0)

#define GET_COL_REV(c, ln) do {				\
	for (int _r = 0; _r < 4; _r++)			\
		ln[REV_R(_r)] = get(_r, c);		\
} while (0)

#define SET_COL_REV(c, ln) do {				\
	for (int _r = 0; _r < 4; _r++)			\
		set(_r, c, ln[REV_R(_r)]);		\
} while (0)

#define PROCESS_AND_SET(r, ln, get_macro, set_macro) do {	\
	get_macro(r, ln);					\
	if (proc(ln)) {						\
		m = 1;						\
		set_macro(r, ln);				\
	}							\
} while (0)

/* Runtime */
uint64_t board;
int run;

void init(void);
void loop(void);
void quit(void);
void show(void);
void add(void);
int move(int);
int over(void);
int get(int, int);
void set(int, int, int);
int proc(uint8_t *);

void init(void)
{
	board = 0;
	add();
	add();
	run = 1;
}

int get(int r, int c)
{
	return (board >> ((r * 4 + c) * 4)) & 0xF;
}

void set(int r, int c, int v)
{
	int p = r * 4 + c;
	board = (board & ~(0xFULL << (p * 4))) | ((uint64_t)v << (p * 4));
}

void show(void)
{
	int r, c, v, p;
	printf(CLEAR);
	printf("\n");
	for (r = 0; r < 4; r++) {
		DO_LINE(r, c, {
			v = get(r, c);
			p = v ? 1 << v : 0;
			printf("%*s", (c == 0) ? 2 : 3, p ? p < 100 ? 
					p < 10 ? " " : "" : "" : "");
			printf(p ? "%d" : "*", p);
		});

		printf("\n");
	}
}

void add(void)
{
	int pos[16], cnt = 0, i;
	for (i = 0; i < 16; i++)
		if (!get(i / 4, i % 4))
			pos[cnt++] = i;
	if (cnt) {
		i = pos[rand() % cnt];
		set(i / 4, i % 4, (rand() % 10) ? 1 : 2);
	}
}

int proc(uint8_t *ln)
{
	uint8_t res[4] = { 0 };
	int i, j, idx = 0, m = 0;
	for (i = 0; i < 4; i++)
		if (ln[i])
			res[idx++] = ln[i];
	for (i = 0; i < 4; i++)
		if (ln[i] != res[i])
			m = 1;
	for (i = 0; i < 3; i++) {
		if (res[i] && res[i] == res[i + 1]) {
			res[i]++;
			for (j = i + 1; j < 3; j++)
				res[j] = res[j + 1];
			res[3] = 0;
			m = 1;
		}
	}
	for (i = 0; i < 4; i++)
		ln[i] = res[i];
	return m;
}

int move(int dir)
{
	uint8_t ln[4];
	int r, m = 0;

	if (dir == 0) {
		for (r = 0; r < 4; r++)
			PROCESS_AND_SET(r, ln, GET_LINE, SET_LINE);
	} else if (dir == 1) {
		for (r = 0; r < 4; r++)
			PROCESS_AND_SET(r, ln, GET_LINE_REV, SET_LINE_REV);
	} else if (dir == 2) {
		for (r = 0; r < 4; r++)
			PROCESS_AND_SET(r, ln, GET_COL, SET_COL);
	} else if (dir == 3) {
		for (r = 0; r < 4; r++)
			PROCESS_AND_SET(r, ln, GET_COL_REV, SET_COL_REV);
	}

	return m;
}

int over(void)
{
	int i, j, v1;

	for (i = 0; i < 16; i++)
		if (!get(i / 4, i % 4))
			return 0;

	for (i = 0; i < 4; i++)
		for (j = 0; j < 3; j++)
			if ((v1 = get(i, j)) && v1 == get(i, j + 1))
				return 0;

	for (j = 0; j < 4; j++)
		for (i = 0; i < 3; i++)
			if ((v1 = get(i, j)) && v1 == get(i + 1, j))
				return 0;

	return 1;
}

void loop(void)
{
	char cmd;
	while (run) {
		show();
		printf("move: wasd, quit: q:");
		scanf(" %c", &cmd);
		if (cmd == 'q')
			run = 0;
		else if (cmd == 'a' || cmd == 'd' || cmd == 'w' || cmd == 's') {
			int dir = (cmd == 'a') ? 0 : (cmd == 'd') ? 1 :
				  (cmd == 'w') ? 2 : 3;

			if (move(dir)) {
				add();
				if (over()) {
					show();
					printf("over\n");
					run = 0;
				}
			}
		}
	}
}

int main(void)
{
	srand(time(NULL));
	init();
	loop();
	return 0;
}
