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

#ifndef SHOOTIN_H_
#define SHOOTIN_H_

#define GO_SIG		20 	// kill() value sent by child; clean proc disposal
#define NB_ENABLE	1
#define NB_DISABLE	0

#define START_X		0  	// hardcoded window start coods
#define START_Y		0

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
#define coord(y, x) (TEST_X * (y) + x)  // TODO TODO TODO
// static int coord(int y, int x) { return FIELD_X * y + x; }

typedef struct projectileVals {
	int x, y;
	short dir;
} projectileVals;

WINDOW *window;
timer_t timerid; // window and matching refresh timer
static u_char *field; // SHARED FIELD (between game loop and field draw)

pid_t actionProc, w;
pthread_t projectile[MAX_PROJECTILE];
u_char GAME_OVER;
int TEST_MUTABLE, wstatus; // mutable, test only; child status int

int knhit();
void nonblock(int);
void *smallProjectile(void *);
void actionPoll(int, int, short);
void updateMainChar(int, int, int, int, short, short);
void initPlayField(int map);
void drawPlayField(int signum);

WINDOW *create_newwin(int, int, int, int);
void destroy_win(WINDOW *);

#endif
