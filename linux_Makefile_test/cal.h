#ifndef __CAL_H__
#define __CAL_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct calculator{
	int opnd_cnt;
	int num[100];
	char op;
}cal;

extern int calculate(int opnum, int opnds[], char op);
extern int input();

#endif


