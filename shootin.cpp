#include "shootin.h"

// Allow nonblocking terminal polling
int kbhit() {
        struct timeval tv;
	fd_set fds;
        tv.tv_sec = 0;
        tv.tv_usec = 0; // disable timeout for structure (sec and usec to 0)
        FD_ZERO(&fds);
        FD_SET(STDIN_FILENO, &fds); // file descriptor or stdin
        select(STDOUT_FILENO, &fds, NULL, NULL, &tv); // stdout monitors stdin with no timeout
        return FD_ISSET(STDIN_FILENO, &fds); // status check
}

void nonblock(int state) {
        struct termios ttystate;
        // get terminal state
        tcgetattr(STDIN_FILENO, &ttystate);

        if (state == NB_ENABLE) {
	        // turn off canonical mode and echo
		ttystate.c_lflag &= ~(ICANON | ECHO);
	        // after a single character is read, read() returns
	        ttystate.c_cc[VMIN] = 1;
	} else if (state == NB_DISABLE) {
       	        // turn on canonical mode once done to reset terminal back to original I/O state              
		ttystate.c_lflag |= (ICANON | ECHO);
	}
	tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);
}

// Pass by value as character coordinates mustn't be modified
void *smallProjectile(void *cPVals) {
	int x = ((projectileVals *) cPVals)->x, 
		y = ((projectileVals *) cPVals)->y;
	short dir = ((projectileVals *) cPVals)->dir;

	int limit;
	short collision = 0;

	switch (dir) {
		case NORTH: 
			limit = FIELD_Y;
			break;
		case EAST: 
			limit = FIELD_X;
			break;
		case SOUTH: 
			limit = 0;
			break;
		case WEST: 
			limit = 0;
			break;
		default: 
			perror("smallProjectile: switch(dir)");	
			return NULL;
			
	}

	mvprintw(1, 1, "TEST: %d", dir);

	if (dir & 1) {	//WEST or EAST
		while ((x != limit) && !collision) { 
			// clear former projectile location
			mvprintw(y, x, " ");
			// 1: WEST, 0: EAST 
			x = (dir & 2) ? x - 1 : x + 1; 
			mvprintw(y, x, ".");
			usleep(PROJECTILE_SPEED);
		}
	} else {
		while ((y != limit) && !collision) { 
			mvprintw(y, x, " ");
			// 1: SOUTH 0: NORTH
			y = (dir & 2) ? y + 1 : y - 1; 
			mvprintw(y, x, ".");
			usleep(PROJECTILE_SPEED);
		}
	}

	pthread_exit(NULL);
}

int collisionCheck(int x, int y, short dir) {
	return 1;
}

void actionPoll(int startX, int startY, short startDir) {
	// internals
	int threadCount = 0;	
	int curPress;
	projectileVals *curPVals;

	// external values
	int mainCharX = startX, mainCharY = startY;
	short mainCharDir = startDir;

	while (!GAME_OVER) {
		threadCount = (threadCount == MAX_PROJECTILE) ? 0 : threadCount;
		drawMainChar(mainCharX, mainCharY, mainCharDir, 1);
		if (kbhit()) {
			curPress = getch();
			switch(curPress){
				case KEY_UP: 
					if (collisionCheck(mainCharX, mainCharY - 1, NORTH))
						mainCharY--;
						mainCharDir = NORTH;
					break;
				case KEY_DOWN:
					if (collisionCheck(mainCharX, mainCharY + 1, NORTH))
						mainCharY++;
						mainCharDir = SOUTH;
					break;
				case KEY_LEFT:
					if (collisionCheck(mainCharX - 1, mainCharY, NORTH))
						mainCharX--;	
						mainCharDir = WEST;
					break;
				case KEY_RIGHT:
					if (collisionCheck(mainCharX - 1, mainCharY, NORTH))
						mainCharX++;
						mainCharDir = EAST;
					break;
				case 'z':
					curPVals = (projectileVals *)
						malloc(sizeof(projectileVals));
					curPVals->dir = mainCharDir; 
					switch(mainCharDir) // slightly redundant initialization 
					{
						case NORTH:
							curPVals->x = mainCharX;
							curPVals->y = mainCharY - 1;
							break;
						case EAST:
							curPVals->x = mainCharX + 1;
							curPVals->y = mainCharY;
							break;
						case SOUTH:
							curPVals->x = mainCharX;
							curPVals->y = mainCharY + 1;
							break;
						case WEST:
							curPVals->x = mainCharX - 1;
							curPVals->y = mainCharY;
							break;
						default:
							perror("actionPoll: switch(curPress)");
							exit(1);
							break;
					}
					if (pthread_create(&projectile[threadCount++],
						NULL, &smallProjectile,
						(void *) curPVals) != 0) {
						perror("pthread_create() error");
						exit(1);
					}
					break;
				case 'x':
					break;
				default:
					break;
			}
		}
		// once new coordinates have been determined, draw the screen
		drawPlayField(NULL);	
		drawMainChar(mainCharX, mainCharY, mainCharDir, 0);
		refresh();
	}
}

void drawMainChar(int x, int y, short dir, short clear) {
	mvprintw(y, x, clear ? " " : "M");
	switch(dir) {
		case NORTH: 	
			mvprintw(y - 1, x, clear ? " " : "^");
			break;	
		case EAST: 	
			mvprintw(y, x + 1, clear ? " " : ">");
			break;
		case SOUTH: 	
			mvprintw(y + 1, x, clear ? " " : "_");
			break;
		case WEST:	
			mvprintw(y, x - 1, clear ? " " : "<");
			break;
		default: 
			printw("Invalid direction (smallProjectile())");	
			return;
	}
}

void drawPlayField(void *state) {
	int i = 0, j = 0;

	if (state == NULL) {
		for ( ; i < FIELD_X; i++) {
			mvprintw(FIELD_Y - 1, i, "-");
		}
		for ( ; j < FIELD_Y; j++) {
			mvprintw(j, i, "-");
		}
		GAME_OVER = 0;
	} else {
		// TODO: this will be changed later
		drawPlayField(NULL);
	}
}

int main() {
	initscr();
	cbreak();
	keypad(stdscr, TRUE);
	noecho();
	nonblock(NB_ENABLE);
	// while (1) {
	drawPlayField(NULL);
	refresh();
	// }

	srand(time(NULL));
	actionProc = fork();
	if (actionProc == 0) {
		actionPoll(rand() % FIELD_X, rand() % FIELD_Y, (short) rand() % 4);
		exit(0);
	} else {
		while (1);
	}

	nonblock(NB_DISABLE);	
	endwin();

	return 0;
}


