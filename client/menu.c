#include "menu.h"
#include <stdio.h>
#include <string.h>
#include "../share/sftpack.h"
/* get the option number */
int Moption(void)
{
	int n;
	printf("please input a number:");
	scanf("%d",&n);
	getchar();
	return n;
}
/* print login menu */
int Mlogin(void)
{
	int order = CNULL;
loop:printf("\n\033[31m+@@@@@@@@@@@@@@@@@@@@@@@@@+\033[0m\n");
	 printf("\033[31m|  1. login               |\033[0m\n");
	 printf("\033[31m|  2. register            |\033[0m\n");
	 printf("\033[31m+@@@@@@@@@@@@@@@@@@@@@@@@@+\033[0m\n");
	int n = Moption();	
#if 1
	if ( n < 3 && n > 0)
	{
		/* get the order code */
		order = order_set[n];
	}
	else
	{
		printf("error,please input number 1 or 2!\n");	
		goto loop;
	}
#endif
	return order;
}
/* print the main menu */
int Mmain(void)
{
	  int order = CNULL;
loop: printf("\n\033[31m+@@@@@@@@@@@@@@@@@@@@@@@@@@@@@+\033[0m\n");
	  printf("\033[31m|  1. explore local files     |\033[0m\n");
	  printf("\033[31m|  2. explore server files    |\033[0m\n");
	  printf("\033[31m|  3. upload files            |\033[0m\n");
	  printf("\033[31m|  4. download files          |\033[0m\n");
	  printf("\033[31m|  5. logout                  |\033[0m\n");
	  printf("\033[31m+@@@@@@@@@@@@@@@@@@@@@@@@@@@@@+\033[0m\n");
	int n = Moption() + 2;	
#if 1
	if (n < 8 && n > 2)
	{
		order = order_set[n];
	}
	else
	{
		printf("error,please input a number between 1 and 5!\n");	
		goto loop;
	}
#endif
	return order;
}
