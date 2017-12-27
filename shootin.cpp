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
        timer.it_interval.tv_sec = 0;
        timer.it_interval.tv_nsec = nsecs;
        /* Start a virtual timer. It counts down whenever this process is
        *    executing. */

	timer_create(CLOCK_REALTIME, &sevp, &timerid);
	timer_settime(timerid, 0, &timer, NULL);
}

int get_thread_number()
{
	int tnum;
	if (rootThread->id = -1) 
	{
		tnum = rand() % MAX_PROJECTILE;
	rootThread->id = rand() % MAX_PROJECTILE;
	}
	else do tnum = rand() % MAX_PROJECTILE; while (!addNode(rootThread, tnum));
	return tnum;
}

// Pass by value as character COORDinates mustn't be modified
void *smallProjectile(void *cPVals) 
{
	TreeNode *parent, *child;
	int tnum = ((projectileVals *) cPVals)->tnum;
	////////
	int x = ((projectileVals *) cPVals)->x, 
		y = ((projectileVals *) cPVals)->y;
	short dir = ((projectileVals *) cPVals)->dir;

	int limit;
	short collision = 0;

	switch (dir) 
	{
		case NORTH: 
			limit = TEST_Y + 1;
			break;
		case EAST: 
			limit = TEST_X + 1;
			break; 
		case SOUTH: 
			limit = -1; 
			break; 
		case WEST: 
			limit = -1; 
			break; 
		default: 
			perror("smallProjectile: switch(dir)");	
			return NULL; 
	} 
	
	if (dir & 1) //WEST or EAST
	{			
		while ((x != limit) && !collision)
		{ 
			// clear former projectile location
			field[COORD(y, x)] = ' ';
			// 1: WEST, 0: EAST 
			x = (dir & 2) ? x - 1 : x + 1; 
			field[COORD(y, x)] = '.';
			usleep(PROJECTILE_SPEED);
		}
	} else {
		while ((y != limit) && !collision)
		{ 
			field[COORD(y, x)] = ' ';
			// 1: SOUTH 0: NORTH
			y = (dir & 2) ? y + 1 : y - 1; 
			field[COORD(y, x)] = '.';
			usleep(PROJECTILE_SPEED);
		}
	}

	// deallocate resources 
	if (rootThread->id == tnum)
		rootThread->id = -1;
	else
	{
		parent = findParent(rootThread, tnum);
		if (parent->left && parent->left->id == tnum)
			parent->left = remNode(parent->left);
		else 
			parent->right = remNode(parent->right);
	}

	pthread_exit(NULL);
}

int collisionCheck(int x, int y, short dir)
{
	if (y == 0 && dir == NORTH) return 0; // memory protection to prevent SIGSEGV (exceptional case)
	if (field[COORD_CHK(y, x, dir)] != ' ')
		return 0;
	return 1;
}

void actionPoll(int startX, int startY, short startDir)
{
	// internals
	int threadCount = 0;	
	int curPress;
	projectileVals *curPVals;

	// external values
	int mainCharX = startX, mainCharY = startY, 
	    mainCharPrvX = startX, mainCharPrvY = startY;
	short mainCharDir = startDir, mainCharPrvDir = startDir;

	while (!GAME_OVER)
	{
		threadCount = (threadCount == MAX_PROJECTILE) ? 0 : threadCount;
	    	mainCharPrvY = mainCharY;
		mainCharPrvX = mainCharX;
		mainCharPrvDir = mainCharDir;
		if (kbhit())
		{
			curPress = wgetch(window);
			switch(curPress)
			{
				case KEY_UP: 
					if (collisionCheck(mainCharX, mainCharY - 1, NORTH))
						mainCharY--;
					mainCharDir = NORTH;
					break;
				case KEY_DOWN:
					if (collisionCheck(mainCharX, mainCharY + 1, SOUTH))
						mainCharY++;
					mainCharDir = SOUTH;
					break;
				case KEY_LEFT:
					if (collisionCheck(mainCharX - 1, mainCharY, WEST))
						mainCharX--;	
					mainCharDir = WEST;
					break;
				case KEY_RIGHT:
					if (collisionCheck(mainCharX + 1, mainCharY, EAST))
						mainCharX++;
					mainCharDir = EAST;
					break;
				case 'z':
					// check thread count bounds
					if (threadCount == MAX_PROJECTILE) break;

					curPVals = (projectileVals *)
						malloc(sizeof(projectileVals));
					curPVals->dir = mainCharDir; 
					curPVals->tnum = get_thread_number();
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
					if (pthread_create(&projectile[curPVals->tnum],
						NULL, &smallProjectile,
						(void *) curPVals) != 0)
					{
						threadCount++; // TODO mutex
						perror("pthread_create() error");
						exit(1);
					}
					break;
				case 'x':
					break;
				case KEY_BACKSPACE:
					kill((pid_t) ((int) getpid() - 1), GO_SIG); // send to parent signal informing of termination
					exit(0); // end process on escape
				default:
					break;
			}
		}
		// once new COORDinates have been determined, draw the screen
		// drawPlayField(NULL);	 
		updateMainChar(mainCharX, mainCharY, mainCharPrvX, mainCharPrvY, mainCharDir, mainCharPrvDir); 
	}
}

void updateMainChar(int x, int y, int prvX, int prvY, short dir, short prvDir)
{
	field[COORD(prvY, prvX)] = ' ';
	field[COORD(y, x)] = 'M';
	field[COORD_CHK(prvY, prvX, prvDir)] = ' ';
	field[COORD_CHK(y, x, dir)] = (dir & 1) ? ((dir & 2) ? '<' : '>') : ((dir & 2) ? '-' : '^');
}
	
void drawPlayField(int signum) 
{
	int i, j = 0;

	wnoutrefresh(window);
	// printf("SIGNAL %d", signum);
	wborder(window, '|', '|', '-', '-', '+', '+', '+', '+');
	for ( ; j < TEST_Y; j++)
		for (i = 0; i < TEST_X; i++) 
			mvwprintw(window, START_Y + j, START_X + i, 
					"%c", field[COORD(j, i)]);
	// printw("TEST MUTABLE: %d", TEST_MUTABLE++);
	// mvwprintw(window, TEST_Y - 1, TEST_X - 1, "Field @ TEST_Y - 1, TEST_X - 1: %c", field[COORD(TEST_Y - 1, TEST_X - 1)]);
	// mvwprintw(window, TEST_Y - 2, TEST_X - 1, "Current field adx: %d", &field);
	doupdate();
}

void initPlayField(int map) 
{
	// initialize barriers (map dependent) (TODO)
	int i = 0, j = 0;
	switch (map) 
	{
		case FALSE: // TEST MAP		
			for ( ; i < TEST_X; i++)
				field[COORD(TEST_Y - 1, i)] = '-'; 
			for ( ; j < TEST_Y; j++)
				field[COORD(j, TEST_X - 1)] = '-';
			break;
		default:
			break;
	}
	// define maps as functions (TODO)
}

// provided ncruses sample code////////////////////////////////////
WINDOW *create_newwin(int height, int width, int starty, int startx)
{
	WINDOW *local_win;

	local_win = newwin(height, width, starty, startx);
	wborder(local_win, '|', '|', '-', '-', '+', '+', '+', '+');
	wrefresh(local_win);
	return local_win;
}

void destroy_win(WINDOW *local_win)
{
	wborder(local_win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
	wrefresh(local_win);
	delwin(window);
}
//////////////////////////////////////////////////////////////////

void set_goflag(int signum)
{
	GAME_OVER = 1;
}

int main(int argc, char *argv[1]) 
{
	GAME_OVER = 0;
	TEST_MUTABLE = 0;	
	int winwidth, winheight, fieldsize;

	// check parameters
	if (argc != 2)
	{
		perror("Usage: ./shootin [-s -m -l]");
		exit(1);
	}

	switch (argv[1][1])
	{
		case 's':
			winwidth = 100;
			winheight = 100;
			break;
		case 'm':
			winwidth = 200;
			winheight = 200;
		       	break;
		case 'f':
			winwidth = 300;
			winheight = 300;
			break;
		case 't': // test case
			winwidth = TEST_X;
			winheight = TEST_Y;
			break;
		default:
			perror("Usage: ./shootin [-s -m -l]");
			exit(1);
	}
	// initiate game window
	initscr();
	window = create_newwin(winheight, winwidth, START_X, START_Y);
	// initiate ncurses values
	cbreak();
	keypad(window, TRUE);
	noecho(); // TODO: ...how in the hell does this work?????
	nonblock(NB_ENABLE);
	refresh();

	// instantiate shared memory
	fieldsize = sizeof(u_char) * winwidth * winheight;
	field = (u_char *) mmap(NULL, (size_t) fieldsize,
			PROT_READ | PROT_WRITE,
			MAP_SHARED | MAP_ANONYMOUS,
			-1, 0);
	memset(field, ' ', (size_t) fieldsize);

	// set root for thread managment
	rootThread = (TreeNode *) malloc(sizeof(TreeNode));
	rootThread->id = -1; 
		
	// set new rseed, split draw/calculation processes, begin
	srand(time(NULL));
	actionProc = fork();
	if (actionProc == 0) 
	{
		initPlayField(FALSE);
		actionPoll(rand() % winwidth, rand() % winheight, (short) rand() % 4);
	} 
	else 
	{
		set_timer(REFRESH_RATE);
		if (signal(GO_SIG, set_goflag) == SIG_ERR)
			perror("Cannot trap child process signal");
		while (!GAME_OVER);
		// while((w = wait(&wstatus)) > 0);
		timer_delete(timerid);
	}

	// cleanup
	destroy_win(window);
	munmap(field, sizeof(field));
	nonblock(NB_DISABLE);
	refresh();
	endwin();

	// system("reset");

	return 0;
}

