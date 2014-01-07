#include "sockssl.h"
/* init sockssl by default,the 'size' is the length of sockssl */
int sockssl_init(SOCKSSL *sockssl,int size)
{
	int i;
	for (i = 0; i < size; i++) {
		sockssl[i].sockfd = INVAILD;	
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
	/* add to invaild item,INVAILD was define at ssl_wrap.h */
	i = sockssl_search(sockssl,size,INVAILD);
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
		sockssl[i].sockfd = INVAILD;	
		sockssl[i].ssl = NULL;
		return 0;
	}
	return -1;
}
