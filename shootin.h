#include <errno.h>
#include <ncurses.h>
#include <pthread.h>
#include <signal.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

extern "C" 
{
#include "bintree.h" // used to store projectile IDs 
}

#ifndef SHOOTIN_H_
#define SHOOTIN_H_

#define GO_SIG		20 	// kill() value sent by child; clean proc disposal
#define NB_ENABLE	1
#define NB_DISABLE	0

#define START_X		1  	// hardcoded window start coods
#define START_Y		1

#define TEST_X		50
#define TEST_Y		10

#define NORTH		0	// 00 -> 11
#define EAST		1
#define SOUTH		2
#define WEST		3

#define REFRESH_RATE		16666667	// 60 FPS (1/60 = 16.6k usecs)
#define MAX_PROJECTILE		100
#define PROJECTILE_SPEED	100000  	// 100 ms

// #define usleep(a) (sleep((double) a / 1000000)) 
#define COORD(y, x) (TEST_X * (y) + x)  // TODO TODO TODO
#define COORD_CHK(y, x, dir) (COORD(y + ((dir & 1) ? 0 : ((dir & 2) ? 1 : -1)), x + ((dir & 1) ? ((dir & 2) ? -1 : 1) : 0)))
// static int coord(int y, int x) { return FIELD_X * y + x; }

typedef struct projectileVals {
	int x, y;
	short dir;
	int tnum; // thread number crucial in cleanup
} projectileVals;

WINDOW *window;
timer_t timerid; // window and matching refresh timer
static u_char *field; // SHARED FIELD (between game loop and field draw)

pid_t actionProc, w;
u_char GAME_OVER;
int TEST_MUTABLE, wstatus; // mutable, test only; child status int

TreeNode *rootThread;
int get_thread_number();
pthread_mutex_t proj_mutex; // mutex for projectile bintree
pthread_t projectile[MAX_PROJECTILE]; 
int threadCount; // total thread quantity, modifiable by individual threads
//

int kbhit();
void nonblock(int);
void *smallProjectile(void *);
void actionPoll(int, int, short);
void updateMainChar(int, int, int, int, short, short);
void initPlayField(int);
void drawPlayField(int);

WINDOW *create_newwin(int, int, int, int);
void destroy_win(WINDOW *);

#endif
