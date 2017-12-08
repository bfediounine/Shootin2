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
#include <termios.h>
#include <time.h>
#include <unistd.h>

#ifndef SHOOTIN_H_
#define SHOOTIN_H_

#define NB_ENABLE	1
#define NB_DISABLE	0

#define FIELD_X		50
#define FIELD_Y		10

#define NORTH		0	// 00 -> 11
#define EAST		1
#define SOUTH		2
#define WEST		3

#define REFRESH_RATE		16666667	// 60 FPS (1/60 = 16.6k usecs)
#define MAX_PROJECTILE		100
#define PROJECTILE_SPEED	100000  	// 100 ms

// #define usleep(a) (sleep((double) a / 1000000)) 
// #define coord(y, x) (FIELD_X * y + x)
static int coord(int y, int x) { return FIELD_X * y + x; }

typedef struct projectileVals {
	int x, y;
	short dir;
} projectileVals;

static u_char *field; // SHARED FIELD (between game loop and field draw)

pid_t actionProc;
pthread_t projectile[MAX_PROJECTILE];
u_char GAME_OVER;
int TEST_MUTABLE; // mutable, test only

int knhit();
void nonblock(int);
void *smallProjectile(void *);
void actionPoll(int, int, short);
void updateMainChar(int, int, int, int, short, short);
void initPlayField(int map);
void drawPlayField(int signum);

#endif
