#include "cut_str.h"
#include <string.h>
/* cut blank character */
void cut_blank(const char *src,char *dest)
{
	cut_char(src,dest,' ');
}
void cut_char(const char *src,char *dest,char del_char)
{
	int i,j,n;
	n = strlen(src);
	for(i = 0,j = 0;i < n;i++)
	{
		if(src[i] == del_char)
		{
			continue;
		}
		else
		{
			dest[j++] = src[i];
		}
	}
}
