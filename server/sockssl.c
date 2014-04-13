/*
 * ========================================================================
 *
 *       Filename:  sockssl.c
 *
 *    Description:  bind a socket and it's ssl session 
 *
 *        Version:  1.0
 *           Date:  04/12/14 08:11:42 PM
 *       Compiler:  gcc-4.6.3
 *
 *         Author:  Populus Huang 
 *          Email:  thoughtsonlive@gmail.com
 *
 * ========================================================================
 */

#include "sockssl.h"
#include "sftpack.h"
/* init sockssl by default,the 'size' is the length of sockssl */
int sockssl_init(SOCKSSL *sockssl,int size)
{
	int i;
	for (i = 0; i < size; i++) {
		sockssl[i].sockfd = INVALID;	
		sockssl[i].ssl = NULL;
	}
}
/* find the index of 'sockfd' in sockssl array */
int sockssl_search(SOCKSSL *sockssl,int size,int sockfd)
{
	int i;
	for (i = 0; i < size ; i++) {
		if(sockssl[i].sockfd == sockfd)	
		{
			return i;
		}
	}
	return -1;
}
/* bind socket and ssl */
int sockssl_bind(SOCKSSL *sockssl,int size,SSL *ssl,int sockfd)
{
	int i;	
	/* add to invaild item,INVALID was define at ssl_wrap.h */
	i = sockssl_search(sockssl,size,INVALID);
	if( i >= 0)
	{
		sockssl[i].sockfd = sockfd;	
		sockssl[i].ssl = ssl;
		return 0;
	}
	return -1;
}
/* unbind sockfd and ssl*/
int sockssl_unbind(SOCKSSL *sockssl,int size,int sockfd)
{
	int i;
	i = sockssl_search(sockssl,size,sockfd);
	if(i >= 0)
	{
		sockssl[i].sockfd = INVALID;	
		sockssl[i].ssl = NULL;
		return 0;
	}
	return -1;
}
