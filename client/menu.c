#include "menu.h"
#include "../share/ui.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../share/sftpack.h"
int get_order(int n,int min,int max)
{
	int order;	

	if((n >= min) && (n <= max))
	{
		order = order_set[n];
	}
	else
	{
		printf(RED"Please input a number between %d and %d ! Or input a negative to quit.\n"NONE,min,max);
		order = CNULL;
	}
	return order;
}
/* get the option number */
int Moption(void)
{
	int n;
	printf(RED"Please input a number: "NONE);
	scanf("%d",&n);
	getchar();
	return n;
}
/* print login menu */
int Mlogin(void)
{
	int order = CNULL;
	do
	{
		start_ui();
		int n = Moption();	
		order = get_order(n,1,2);
		assert(order);
	}while(order == CNULL);
	return order;
}
/* print the main menu */
int Mmain(void)
{
	  int order = CNULL;
	  int n;
	  do
	  {
		primary_ui();
		n = Moption() + 2;	
		if( n > 2 && n < 9)
			order = order_set[n];
		assert(order);
	  }while(order == CNULL);
	return order;
}
int Mlist(void)
{
	int order = CNULL;	
	int n;
	do
	{
		list_ui();
		n = Moption() + 8;
		printf("Mlist:n=%d\n",n);
		if(n > 8&& n < 13)
		{
			order = order_set[n];
		}
		else
		{
			printf(RED"Please input a number between %d and %d ! Or input a negative to quit.\n"NONE,1,3);
		}
		assert(order);
	}while(order == CNULL);
	return order;
}
