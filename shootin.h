#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <ncurses.h>
#include <stdlib.h>
#include <stdio.h>
#include <termios.h>
#include <errno.h>

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

#define MAX_PROJECTILE		100
#define PROJECTILE_SPEED	200000 // 200 ms

// #define usleep(a) (sleep((double) a / 1000000)) 

typedef struct projectileVals {
	int x, y;
	short dir;
} projectileVals;

pid_t actionProc;
pthread_t projectile[MAX_PROJECTILE];
u_char GAME_OVER;

int knhit();
void nonblock(int);
void *smallProjectile(void *);
void actionPoll(int, int, short);
void drawMainChar(int, int, short, short);
void drawPlayField(void *state);

#endif
