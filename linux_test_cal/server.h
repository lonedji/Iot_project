#ifndef __SERVER_H__
#define __SERVER_H__

#define BUF_SIZE 1024
#define OPSZ 4
#define RLT_SIZE 4

extern int calculate(int opunm, int opnds[], char op);
void error_handling(char *message);

#endif





