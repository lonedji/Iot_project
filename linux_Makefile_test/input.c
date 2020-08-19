#include "cal.h"

int input()
{
	int result = 0;
	puts("Calculator..start......");
	fputs("Operand count : ", stdout);
	scanf("%d", &cal.opnd_cnt);

	for (int i=0; i<cal.opnd_cnt; i++)
	{
		printf("Operand %d: ", i+1);
		scanf("%d", &cal.num[i]);
	}
	fgetc(stdin);
	fputs("Operator: ", stdout);
	scanf("%c", &cal.op);
	result = calculate(cal.opnd_cnt, cal.num, cal.op);
	printf("Operation result : %d\n", result);

}
