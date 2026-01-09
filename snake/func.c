void init(void)
{	
	for (int i = 0; i < MAP_W; i++) {
		for (int j = 0; j < MAP_H; j++) {
			map[i][j].val = 0;
		}
	}
	srand(time(NULL));	
	INIT_POS.type = HEAD;
	OP      = RIGHT;
	RUNNING = 1;
	TICK    = 0;
	TAILPOS = 2;
	map[HEAD_X - 1][HEAD_Y].type = BODY;
		
	HEAD_X  = MAP_W / 2;
	HEAD_Y  = MAP_H / 2;
	TAIL_X  = HEAD_X - 1;
	TAIL_Y  = HEAD_Y;
	spawn();
	// set terminal.	
	setrawmode();
	atexit(quit);
};

void setrawmode(void)
{
	struct termios new_termios;
	tcgetattr(STDIN_FILENO, &original_termios);
	new_termios = original_termios;
	new_termios.c_lflag &= ~(ICANON | ECHO);
	new_termios.c_cc[VMIN] = 1;    new_termios.c_cc[VTIME] = 0;     
	tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);
}


void restoreter(void)
{
	tcsetattr(STDIN_FILENO, TCSANOW, &original_termios);
}


int getch_nonblock(void) {
	if (kbhit()) {
		return getchar();
	}
	return -1;
}

int kbhit(void)
{
	struct timeval tv = {0L, 0L};
	fd_set fds;

	FD_ZERO(&fds);
	FD_SET(STDIN_FILENO, &fds);

	return select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);
}

void old_ui(void)
{
	printf(CLEAR);
	printf("\n");
	for (int i = 0; i < MAP_H; i++) {
		for (int j = 0; j < MAP_W; j++) {
			uint8_t c = map[j][i].type;
			if (c == AIR)
				printf("%s", C_AIR);
			else if (c == HEAD)
				printf("%s", C_HEAD);
			else if (c == BODY)
				printf("%s", C_BODY);
			else if (c == FRUIT)
				printf("%s", C_FRUIT);
			else if (c == POSION)
				printf("%s", C_POSION);
			
		}
		printf("\n");
	}
	debug();
	printf("\nlength: %d\n", TAILPOS);
}


#define M_FAILED -1
#define M_SUCCEED 0

int move(void)
{
	// Set TAIL -> AIR
	map[TAIL_X][TAIL_Y].type = AIR;

	// Change the snake stack value	
	for (int i = TAILPOS; i > 0; i--) {
		map[i + 1][0].data = map[i][0].data;
		map[i + 1][1].data = map[i][1].data;
	}
	
	// Set HEAD -> BODY
	map[HEAD_X][HEAD_Y].type = BODY;

	// Update HEAD_X & HEAD_Y
	if (OP == UP) {
		if (HEAD_Y == 0)
			goto failed;
		HEAD_Y--;
	} else if (OP == RIGHT) {
		if (HEAD_X == MAP_W - 1)
			goto failed;
		HEAD_X++;
	} else if (OP == DOWN) {
		if (HEAD_Y == MAP_H - 1)
			goto failed;
		HEAD_Y++;
	} else if (OP == LEFT) {
		if (HEAD_X == 0)
			goto failed;
		HEAD_X--;
	}
	
	// Check FRUIT
	if (map[HEAD_X][HEAD_Y].type == FRUIT) {
		TAILPOS++;
		map[TAIL_X][TAIL_Y].type = BODY;
		spawn();
	} else if (map[HEAD_X][HEAD_Y].type == POSION || 
		map[HEAD_X][HEAD_Y].type == BODY) {
		goto failed;
	}

	// Set new HEAD
	map[HEAD_X][HEAD_Y].type = HEAD;	
	return M_SUCCEED;

failed:
	return M_FAILED;
}

void debug(void)
{
	if (DEBUG) {
		printf("TAIL.POS :%d\n", TAILPOS);
		printf("HEAD.X :%d\n", HEAD_X);
		printf("HEAD.Y :%d\n", HEAD_Y);
		printf("TAIL.X :%d\n", TAIL_X);
		printf("TAIL.Y :%d\n", TAIL_Y);
	}
}

void spawn(void)
{
	int x, y, i;
	int types[] = { FRUIT, POSION };

	for (i = 0; i < 2; i++) {
		do {
			x = rand() % MAP_W;
			y = rand() % MAP_H;
		} while (map[x][y].type != AIR);
		map[x][y].type = types[i];
	}
}


#include "ui.c"
void run(void)
{
	char c = ' ';

	while(RUNNING) {
		if(kbhit()) {
			c = getchar();
			if (move() == -1)
				goto die;
			ui();
		}

		if (c == 27)
			RUNNING = 0;
		if (TAILPOS == 15)
			goto succeed;

		if (c == 'w' && OP != DOWN)
			OP = UP;
		else if (c == 'd' && OP != LEFT)
			OP = RIGHT;
		else if (c == 's' && OP != UP)
			OP = DOWN;
		else if (c == 'a' && OP != RIGHT)
			OP = LEFT;

		TICK++;
		if (TICK == 0) {	
			if (move() == -1)
				goto die;
			ui();
		}
		usleep(FRAME_TIME);
	}



die:
	printf("HAHAHAHA, you dead...\n");
	return;
succeed:
	printf("Oh my gosh, you won...\n");
};


void quit(void)
{
	printf("BBBBBBBBBBBye from snake ~\n");
	restoreter();
	
	for (int i = 0; i < 15; i++)
		printf("%s", C_BODY);
	printf("%s\n", C_HEAD);
}
