#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "server.h"

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
