#include "shootin.h"

// Allow nonblocking terminal polling
int kbhit() 
{
        struct timeval tv;
	fd_set fds;
        tv.tv_sec = 0;
        tv.tv_usec = 0; // disable timeout for structure (sec and usec to 0)
        FD_ZERO(&fds);
        FD_SET(STDIN_FILENO, &fds); // file descriptor or stdin
        select(STDOUT_FILENO, &fds, NULL, NULL, &tv); // stdout monitors stdin with no timeout
        return FD_ISSET(STDIN_FILENO, &fds); // status check
}

void nonblock(int state) 
{
        struct termios ttystate;
        // get terminal state
        tcgetattr(STDIN_FILENO, &ttystate);

        if (state == NB_ENABLE) 
	{
	        // turn off canonical mode and echo
		ttystate.c_lflag &= ~(ICANON | ECHO);
	        // after a single character is read, read() returns
	        ttystate.c_cc[VMIN] = 1;
	} 
	else if (state == NB_DISABLE) 
	{
       	        // turn on canonical mode once done to reset terminal back to original I/O state              
		ttystate.c_lflag |= (ICANON | ECHO);
	}
	tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);
}

void set_timer(double nsecs) 
{ 
	timer_t timerid;
	struct sigevent sevp;
	struct sigaction sa;
        struct itimerspec timer;

        /* Install timer_handler as the signal handler for SIGVTALRM. */
        memset(&sevp, 0, sizeof(struct sigevent));
	sevp.sigev_value.sival_ptr = &timer;
	sevp.sigev_notify = SIGEV_SIGNAL;
	sevp.sigev_notify_attributes = NULL;
	sevp.sigev_signo = SIGVTALRM;

        sa.sa_handler = &drawPlayField;
	sa.sa_flags = 0;
        sigaction(SIGVTALRM, &sa, NULL);

        /* Configure the timer to expire after 1 sec... */
        timer.it_value.tv_sec = 0;
        timer.it_value.tv_nsec = 100;
        /* ... and every 1 sec after that. */
        timer.it_interval.tv_sec = 1;
        timer.it_interval.tv_nsec = 0;
        /* Start a virtual timer. It counts down whenever this process is
        *    executing. */

	timer_create(CLOCK_REALTIME, &sevp, &timerid);
	timer_settime(timerid, 0, &timer, NULL);
}

// Pass by value as character coordinates mustn't be modified
void *smallProjectile(void *cPVals) 
{
	int x = ((projectileVals *) cPVals)->x, 
		y = ((projectileVals *) cPVals)->y;
	short dir = ((projectileVals *) cPVals)->dir;

	int limit;
	short collision = 0;

	switch (dir) 
	{
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
	
	if (dir & 1) //WEST or EAST
	{			
		while ((x != limit) && !collision)
		{ 
			// clear former projectile location
			field[coord(y, x)] = ' ';
			// 1: WEST, 0: EAST 
			x = (dir & 2) ? x - 1 : x + 1; 
			field[coord(y, x)] = '.';
			usleep(PROJECTILE_SPEED);
		}
	} else {
		while ((y != limit) && !collision)
		{ 
			field[coord(y, x)] = ' ';
			// 1: SOUTH 0: NORTH
			y = (dir & 2) ? y + 1 : y - 1; 
			field[coord(y, x)] = '.';
			usleep(PROJECTILE_SPEED);
		}
	}

	pthread_exit(NULL);
}

int collisionCheck(int x, int y, short dir)
{
	return 1;
}

void actionPoll(int startX, int startY, short startDir)
{
	// internals
	int threadCount = 0;	
	int curPress;
	projectileVals *curPVals;

	// external values
	int mainCharX = startX, mainCharY = startY;
	short mainCharDir = startDir;

	while (!GAME_OVER)
	{
		threadCount = (threadCount == MAX_PROJECTILE) ? 0 : threadCount;
		updateMainChar(mainCharX, mainCharY, mainCharDir, 1);
		if (kbhit())
		{
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
						(void *) curPVals) != 0)
					{
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
		// drawPlayField(NULL);	 
		updateMainChar(mainCharX, mainCharY, mainCharDir, 0); 
	}
}

void updateMainChar(int x, int y, short dir, short clear)
{
	field[coord(y, x)] = 'M';
	/* field[y][x] = (clear) ? ' ' : 'M';
	field[y + (dir & 1) ? ((dir & 2) ? 1 : -1) : 0][x + (dir & 1) ? 0 : ((dir & 2) ? 1 : -1)] = (clear) ? ' ' :
			((dir & 1) ? ((dir & 2) ? '-' : '^') : ((dir & 2) ? '<' : '>')); */
}

// TODO: single function ON SOFTWARE TIMER (maybe?)
// {
void drawPlayField(int signum) 
{
	int i, j = 0;

	// printf("SIGNAL %d", signum);
	for ( ; j < FIELD_Y; j++)
		for (i = 0; i < FIELD_X; i++)
			mvprintw(j, i, "%c", field[coord(j, i)]);
		//	mvprintw(j, i, "+");
	// printw("TEST MUTABLE: %d", TEST_MUTABLE++);
	mvprintw(FIELD_Y - 1, FIELD_X - 1, "Field @ FIELD_Y - 1, FIELD_X - 1: %c", field[coord(FIELD_Y - 1, FIELD_X - 1)]);
	refresh();
}
// }

void initPlayField(int map) 
{
	// initialize barriers (map dependent) (TODO)
	int i = 0, j = 0;
	switch (map) 
	{
		case FALSE: // TEST MAP		
			for ( ; i < FIELD_X; i++)
				field[coord(FIELD_Y - 1, i)] = '-'; 
			for ( ; j < FIELD_Y; j++)
				field[coord(j, FIELD_X - 1)] = '-';
			break;
		default:
			break;
	}
	// define maps as functions (TODO)
}

int main() 
{
	TEST_MUTABLE = 0;

	// initiate ncurses values
	initscr();
	cbreak();
	keypad(stdscr, TRUE);
	noecho();
	nonblock(NB_ENABLE);
	// instantiate shared memory
	field = (u_char *) mmap(NULL, sizeof(u_char) * FIELD_Y * FIELD_X, 
			PROT_READ | PROT_WRITE,
			MAP_SHARED | MAP_ANONYMOUS, 
			-1, 0);

	// while (1) 
	// {
	initPlayField(FALSE);
	set_timer(REFRESH_RATE);
	// }

	srand(time(NULL));
	actionProc = fork();
	if (actionProc == 0) 
	{
		actionPoll(rand() % FIELD_X, rand() % FIELD_Y, (short) rand() % 4);
		exit(0);
	} 
	else 
	{
		while (1);
	}

	munmap(field, sizeof(field));
	nonblock(NB_DISABLE);	
	endwin();

	return 0;
}

